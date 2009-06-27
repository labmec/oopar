/***************************************************************************
 *   Copyright (C) 2008 by OOPar Project   *
 *   oopar@oopar   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket.h"

struct spdProcess listProcess[1000];
int numberProcess = 0;
char *args[50], *test; // variaveis para a geracao do executavel e dos argumentos

/* metodo que coloca todas as portas da lista como -1 para indicar que elas estao disponiveis */
void resetList()
{
   int i = 0;

   for(i=0;i<1000;i++)
      listProcess[i].port = -1;
}

// gera a porta para a aplicacao
int setPort(int numProc)
{
   int i = 0;
   for(i = 0; i<1000; i++)
   {
      // verifica se a porta esta disponivel
      if(listProcess[numProc].port == -1)
         return numProc;
      else
         numProc = (numProc+1) % 1000;
   }
   return -1;
}

// libera a porta para o uso
void freePort(int numberPort)
{
   listProcess[numberPort - INIT_PORT].port = -1;
}

// coloca os argumentos do executavel
void putArgs(char *buffer)
{
   int i = 0;

   test = strtok( buffer, " " );
   //printf("args:%s:\n",buffer); fflush(stdout);
   while(test != NULL)
   {
      // printf("Entrei no while\n"); flush(stdout);
      args[i] = (char *)malloc(sizeof(char)*strlen(test));
      strcpy(args[i],test);
      // printf(" Arg %d - %s\n", i, test); fflush(stdout);
      i++;
      test = strtok( NULL, " " );
   }
   // printf("To aqui\n"); fflush(stdout);
   //args[i] = (char *)malloc(sizeof(char)*6);
   //strcpy(args[i],"\0");
   args[i] = NULL;
   //printf("Executavel = %s\n",args[0]);fflush(stdout);
}

int main(int argc, char *argv[])
{
   int sSocket, cSocket, mSocket; // sSocket : Server . cSocket: Client . mSocket: Mensager
   int pid;
   struct sockaddr_in sAddr;      // my address information
   struct sockaddr_in cAddr;      // connector's address information
   struct sockaddr_in mAddr;      // mensager's address information

   struct hostent *host;       // utilizada para os metodos gethostbyname , gethostbyport , etc
   struct srunEnvelope msg;    // buffer para alocar as mensagens do srun
   int sinSize, chosenPort = 0;
   int yes = 1, portTemp;
   int endPort, rankProc; // variaveis utilizadas para finalizar o processo

   if ((sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
   {
      fprintf(stderr,"SPD Error: Socket Creation\n");
      exit(1);
   }

   if (setsockopt(sSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
   {
      fprintf(stderr,"SPD Error: Setsockopt\n");
      exit(1);
   }

   sAddr.sin_family = AF_INET;         // host byte order
   sAddr.sin_port = htons(SPD_PORT);     // short, network byte order
   sAddr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
   memset(&(sAddr.sin_zero), '\0', 8); // zero the rest of the struct

   if (bind(sSocket, (struct sockaddr *)&sAddr, sizeof(struct sockaddr)) == -1)
   {
      fprintf(stderr,"SPD Error: Bind\n");
      exit(1);
   }

   if (listen(sSocket, MAX_QUEUE) == -1)
   {
      fprintf(stderr,"SPD Error: Listen\n");
      exit(1);
   }

   // inicia as portas como disponiveis
   resetList();

   while(1)
   {  // main accept() loop
      printf("-----------> SPD STARTUP <---------------\n");fflush(stdout);
      sinSize = sizeof(struct sockaddr_in);
      //printf("Estou travado no accept\n"); fflush(stdout);
      if ((cSocket = accept(sSocket, (struct sockaddr *)&cAddr, &sinSize)) == -1)
      {
         fprintf(stderr,"SPD Error: Accept\n");
         continue;
      }else{
         //printf("SPD Server: got connection from %s\n",inet_ntoa(cAddr.sin_addr));

         if (recv(cSocket, (struct srunEnvelope *)&msg, sizeof(struct srunEnvelope), 0) == -1)
         {
            fprintf(stderr,"SPD Error: Recv\n");
            exit(1);
         }
         printf("Number msg: %d\n", msg.msgType); fflush(stdout);

         switch(msg.msgType)
         {
            case STARTUP:
                // seleciona uma porta da lista
                chosenPort = setPort(numberProcess);
                printf("Porta escolhida: %d\n", INIT_PORT + chosenPort); fflush(stdout);
                if(chosenPort == -1)
                {
                   fprintf(stderr,"SPD Error: No avaliable port\n");
                   exit(1);
                }

                // atribui o processo com sua porta e srun na lista
                strcpy(listProcess[chosenPort].name,msg.message); // adiciono o processo na lista de processos
                listProcess[chosenPort].port = INIT_PORT + chosenPort; // defino o numero da porta
                strcpy(listProcess[chosenPort].srun,inet_ntoa(cAddr.sin_addr) ); // adiciono o srun
                                                     // TODO: melhorar a ocupacao das portas alocadas
                numberProcess = (numberProcess+1) %1000; // incremento a qtde de processos

                pid = fork();
                if (!pid)
                { // esse eh o processo filho
                   close(sSocket); // o processo filho nao precisa do sSocket

                   // coloca o executavel com os argumentos
                   putArgs(msg.message);

                   //printf("Estou na criacao do processo filho:%s:\n",args[0]); fflush(stdout);
                   // executa o processo
                   if(execv(args[0], args) < 0)
                   {
                       printf("SPD Error: Executing child.\n");
                       perror(""); fflush(stdout);
                   }

                  // execl("msg.message",0); // executa o processo
                   exit(0);
                }else
                { // eh o processo pai
                    listProcess[chosenPort].pid = pid;
                   //printf("Antes do pai...\n"); fflush(stdout);
                   sinSize = sizeof(struct sockaddr_in);
                   if ((mSocket = accept(sSocket, (struct sockaddr *)&mAddr, &sinSize)) == -1)
                   {
                      fprintf(stderr,"SPD Error: Mensager Accept\n");
                      continue;
                   }else
                   { // o processo pede a porta
                      if (recv(mSocket, (struct srunEnvelope *)&msg, sizeof(struct srunEnvelope), 0) == -1)
                      {
                         fprintf(stderr,"SPD Error: Recv\n");
                         exit(1);
                      }

                      if(msg.msgType == GETPORT) // se a mensagem for solicitando a porta
                      {
                          msg.msgType = SETPORT;
                          sprintf(msg.message,"%d",listProcess[chosenPort].port);
                          sscanf(msg.message,"%d",&portTemp);
                          //printf("porta enviada: %d %d\n",msg.message,portTemp); fflush(stdout);

                          // manda a msg para o processo com a porta
                          if (send(mSocket,(char *)&msg , sizeof(struct srunEnvelope), 0) == -1)
                          {
                             fprintf(stderr,"SPD Error: Send\n");
                             exit(1);
                          }
                          close(mSocket);

                          // manda a msg para o srun com a porta
                          if (send(cSocket, (char *)&msg , sizeof(struct srunEnvelope), 0) == -1)
                          {
                             fprintf(stderr,"SPD Error: Send\n");
                             exit(1);
                          }
                          close(cSocket);

                      }else
                      { // nao eh uma msg solicitando porta
                         fprintf(stderr,"SPD Error: Recv\n");
                         exit(1);
                      }

                   } // fim do else das portas

                } // fim do else do startup
                break;
            case FINALIZE: // finaliza a execucao - o processo fez a requisicao
                // le as informacoes da mensagem e coloca nas variaveis correspondentes
                sscanf (msg.message,"%d %d",&endPort,&rankProc);
                // mandando finalizacao para o Processo
                if ((mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                {
                    fprintf(stderr,"SPD Error: Socket Creation - SRUN\n"); fflush(stderr);
                    exit(1);
                }
                mAddr.sin_family = AF_INET;    // host byte order
                mAddr.sin_port = htons(endPort);  // short, network byte order
                //mAddr.sin_addr.s_addr = inet_addr(listProcess[endPort - INIT_PORT].srun); // ip

                //printf("SPD Conectando com SRUN em %s %d\n",listProcess[endPort - INIT_PORT].srun, SRUN_PORT); fflush(stdout);
                host = gethostbyname("localhost");
                if(host == NULL)
                {
                    fprintf(stderr,"SPD Error: Not host name."); fflush(stderr);
                    exit(1);
                }

                //memset(&(mAddr.sin_zero), '\0', 8);  // zero the rest of the struct
                memcpy(&mAddr.sin_addr, host->h_addr, host->h_length);
                if(connect(mSocket, (struct sockaddr *)&mAddr, sizeof(struct sockaddr)) < 0)
                {
                    fprintf(stderr,"SPD Error: Connection to Process (Finalizing)\n"); fflush(stderr);
                   exit(1);
                }
                OOP_SOCKET_Envelope fin;
                fin.idOp = FINALIZE;
                if (send(mSocket, (void *)&fin , sizeof(OOP_SOCKET_Envelope), 0) == -1)
                {
                    fprintf(stderr,"SPD Error: Send\n"); fflush(stderr);
                    exit(1);
                }
                close(mSocket);
                waitpid(listProcess[endPort-INIT_PORT].pid, NULL, 0);

                // libera a porta da lista
                freePort(endPort);

                // coloca o rank do proc no buffer para envio ao srun
                sprintf(msg.message,"%d",rankProc);

                msg.msgType = FINALIZE;

                // fazer a conexao com o srun
                // estou utilizando o mSocket
                if ((mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                {
                    fprintf(stderr,"SPD Error: Socket Creation - SRUN\n"); fflush(stderr);
                   exit(1);
                }

                mAddr.sin_family = AF_INET;    // host byte order
                mAddr.sin_port = htons(SRUN_PORT);  // short, network byte order
                //mAddr.sin_addr.s_addr = inet_addr(listProcess[endPort - INIT_PORT].srun); // ip

                //printf("SPD Conectando com SRUN em %s %d\n",listProcess[endPort - INIT_PORT].srun, SRUN_PORT); fflush(stdout);
                host = gethostbyname(listProcess[endPort - INIT_PORT].srun);
                if(host == NULL)
                {
                    fprintf(stderr,"SPD Error: Not host name."); fflush(stderr);
                   exit(1);
                }

                memset(&(mAddr.sin_zero), '\0', 8);  // zero the rest of the struct
                memcpy(&mAddr.sin_addr, host->h_addr, host->h_length);

                while (connect(mSocket, (struct sockaddr *)&mAddr, sizeof(struct sockaddr)) < 0) ;
                //{
                //   fprintf(stderr,"SPD Error: Connection to SRUN\n");
                //   exit(1);
                //}

                // manda a msg para o srun com o rank
                //printf("SPD FINALIZE --> %d %d\n",msg.msgType, rankProc);
                if (send(mSocket, (void *)&msg , sizeof(struct srunEnvelope), 0) == -1)
                {
                    fprintf(stderr,"SPD Error: Send\n"); fflush(stderr);
                   exit(1);
                }
                close(mSocket);
                close(cSocket);

                break;
            default:
                fprintf(stderr,"SPD Error: Invalid Msg Option\n"); fflush(stderr);
                break;
         } // fim do switch

      } // fim do else do accept

   } // fim do while
   return EXIT_SUCCESS;
}
