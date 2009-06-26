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
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"

#define STILL_RUNNING 	1
#define FINISHED	0

struct processTable pTable[MAX_PROC];   // tabela de processos em uso
int OOParSize;                          // quantidade de total de hosts disponiveis
int np;					// quantidade inicial de participantes

void help()
{
     // imprime msg de help
    fprintf(stderr,"Usage: srun <parameters> programName [-args arg1 arg2...]\n");
    fprintf(stderr,"       parameters\n");
    fprintf(stderr,"       -np numprocs       : number of processes to startup\n");
    fprintf(stderr,"       -hostfile filename : file containing host names (default hostfile.oopar)\n");
    fprintf(stderr,"For more information oopar@gmail.com\n");
    exit(1);
}
void parseHostfile(char *hostFile)
{
   int nfields;
   FILE *f;
   char hostName[BUFFER_SIZE];
   f = fopen(hostFile, "r");
   if (f == NULL)
   {
         fprintf(stderr,"SRUN Error: failed to open hostfile\n");
         exit(1);
   }
   OOParSize=0;
   while (!feof(f))
   {
      nfields = fscanf(f,"%s",hostName);
      if (nfields != 0)
      {
         strcpy(pTable[OOParSize].hostName,hostName);
         pTable[OOParSize].inUse = 0;
         pTable[OOParSize].port = -1;
         OOParSize++;
      }
   }
   fclose(f);
}

void showHosts()
{
   int i;
   printf("SRUN Debug Mode\n");
   for (i=0; i<OOParSize; i++)
     printf("%d %s %d %d\n",i,pTable[i].hostName,pTable[i].inUse, pTable[i].port);
}



int main(int argc, char *argv[])
{
   // estruturas para comunicacao
   struct sockaddr_in sAddr, mAddr, cAddr;          // estruturas de endereco em modo servidor e cliente
   int sSocket, cSocket, mSocket;                   // sSocket - modo servidor - ouve
				                    // cSocket - comunicacao em modo servidor - comunica-se com cliente
				                    // mSocket - modo mensageiro - para encaminhar mensagens
   struct hostent *host;
   char hostName[256];
   int ret, nbytes, length, bufsize, tmpPort, tmpRank;
   struct srunEnvelope msgSend, msgRecv;           // envelope para comunicacao
   // final das estruturas

   // manipulacao de parametros
   int np, i, j, binIndex;
   char hostFile[256] = "hostfile.oopar";
   char bin[256], binArgs[256] = "";

   // variaveis de controle
   int condition=STILL_RUNNING,       	// condicao para continuar o loop
       totalNp=0;			// numero total de processos que enviaram o finalize - quando for igual ao NP, termina


   //---Tratamento dos parametros de linha de comando----------

   if(argc < 4)
      help();

   if(strcmp(argv[1],"-np")!=0)
      help();
   np = atoi(argv[2]);
   if(np<=0)
      help();

   if(strcmp(argv[3],"-hostfile")!=0)
   {
      binIndex=3;
   }
   else
   {
      if(argc<6)
         help();
      strcpy(hostFile,argv[4]);
      binIndex=5;
   }
   if(argc > binIndex+1)
   {
      if(argc < binIndex+3)
         help();
      if(strcmp(argv[binIndex+1],"-args")!=0)
         help();
      strcpy(binArgs,argv[binIndex+2]);
   }
   strcpy(bin,argv[binIndex]);

   printf("-np %d\n",np);
   printf("-hostfile %s\n", hostFile);
   printf("%s",bin);
   printf(" %s\n\n",binArgs);
   //-------------


   parseHostfile(hostFile);
#if DEBUG
   showHosts();
#endif

   // preparacao do socket para ficar ouvindo em modo servidor
   // cria socket
   if ( (sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
   {
      fprintf(stderr,"SRUN Error: Error creating listening socket\n");
      exit(1);
   }

   sAddr.sin_family = AF_INET;
   sAddr.sin_port = htons(SRUN_PORT);
   sAddr.sin_addr.s_addr = INADDR_ANY;
   memset(&(sAddr.sin_zero), '\0', 8);

   //bind da porta
   if (bind(sSocket, (struct sockaddr *)&sAddr, sizeof(struct sockaddr)) < 0)
   {
       fprintf(stderr,"SRUN Error: Error on binding listening address %d \n", errno);
       exit(1);
   }

   // listen da fila
   if (listen(sSocket,MAX_QUEUE) < 0)
   {
      fprintf(stderr,"SRUN Error: Error on defining listening queue\n");
      exit(1);
   }
   //----------------------------------------

   // FASE 1
   // entrar em contato com cada um dos spds dos participantes para iniciar o processo de startup
   for(i=0; i<np; i++)
   {
      if ((mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      {
         fprintf(stderr,"SRUN Error: Could not create socket\n");
         exit(1);
      }
      mAddr.sin_family = AF_INET;
      mAddr.sin_port = htons(SPD_PORT);

      host = gethostbyname(pTable[i].hostName);
      if (host == NULL)
      {
         fprintf(stderr,"SRUN Error: Could not resolve %s host\n",pTable[i].hostName);
         exit(1);
      }

      memcpy(&mAddr.sin_addr, host->h_addr, host->h_length);
      //mAddr.sin_addr.s_addr = inet_addr(pTable[i].hostName);

      printf("Conectando com SPD em %s %d\n", pTable[i].hostName, SPD_PORT); fflush(stdout);

      if (connect(mSocket,(struct sockaddr*)  &mAddr, sizeof(struct sockaddr_in)) < 0)
      {
         fprintf(stderr,"SRUN Error: Startup Error. Could not connect to host %s\n",pTable[i].hostName);
         exit(1);
      }

      // FIXME - completar path do binario.
      // Token separador dos elementos do binario e seus argumentos eh o espaco em branco
      msgSend.msgType = STARTUP;
      strcpy(msgSend.message,bin);
      strcat(msgSend.message," ");
      strcat(msgSend.message,binArgs);

      printf("Msg: %s\n",msgSend.message); fflush(stdout);
      if (send(mSocket, (char*)&msgSend, sizeof(struct srunEnvelope), 0) < 0)
      {
         fprintf(stderr,"SRUN Error: Error sending startup message\n");
         exit(1);
      }
      if (recv(mSocket, (char*)&msgRecv, sizeof(struct srunEnvelope), 0) < 0)
      {
         fprintf(stderr,"SRUN Error: Error receiving por number form SPD\n");
         exit(1);
      }
      // unmarshalling do numero da porta na mensagem
      if (msgRecv.msgType == SETPORT)
      {

         sscanf(msgRecv.message,"%d",&pTable[i].port);
         printf("SRUN - Recebi mensagem de Port : %d\n",pTable[i].port);   fflush(stdout);

      }
      else
      {
         fprintf(stderr,"SRUN Error: Protocol error\n");
         exit(1);
      }
      close(mSocket);
   }
   // --Notificacao de todos os demais processos para a cria'c~ao da tabela de processos.
   // descomentar quando houver o INIT

   // passo 1 - criacao da lista de processos
   // tamanho do buffer = quantidade de processos
   bufsize =  np * sizeof(struct processTable);

   msgSend.msgType = NOTIFYALL;

   // passo 2 - enviar a estrutura para os participantes
   printf("FASE 2 -----------\n");fflush(stdout);

   showHosts();

   for(i=0; i<np; i++)
   {
      sprintf(msgSend.message, "%d %d %d" , bufsize, np, i);

      if ((mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      {
         fprintf(stderr,"SRUN Error: Could not create socket\n");
         exit(1);
      }
      mAddr.sin_family = AF_INET;
      mAddr.sin_port = htons(pTable[i].port);

      host = gethostbyname(pTable[i].hostName);
      if (host == NULL)
      {
         fprintf(stderr,"SRUN Error: Could not resolve %s host\n",pTable[i].hostName);
         exit(1);
      }

      memcpy(&mAddr.sin_addr, host->h_addr, host->h_length);
      //mAddr.sin_addr.s_addr = inet_addr(pTable[i].hostName);

      printf("Conectando com PROCESSO em %s %d\n", pTable[i].hostName, pTable[i].port); fflush(stdout);

      while (connect(mSocket,(struct sockaddr*)  &mAddr, sizeof(struct sockaddr_in)) < 0) usleep(250000);

      //essa parte tem que ser espera ocupada para garantir a sincronizacao entre as entidades
      /*{
         fprintf(stderr,"SRUN Error: Startup Error. Could not connect to host %s\n",pTable[i].hostName);
         exit(1);
      }*/

      printf("passou - %d %d\n", msgSend.msgType, bufsize);fflush(stdout);


      if (send(mSocket, (char*)&msgSend, sizeof(struct srunEnvelope), 0) < 0)
      {
         fprintf(stderr,"SRUN Error: Error sending notify message\n");
         exit(1);
      }
      if (send(mSocket, (char*)&pTable, bufsize, 0) < 0)
      {
         fprintf(stderr," SRUN Error: Error sending process table configuration message\n");
         exit(1);
      }

      close(mSocket);

   }



   // FASE 3
   // loop para aguardar mensagens

   printf(" Aguardando mensagens de alguem \n"); fflush(stdout);

   while(condition == STILL_RUNNING)
   {
      length = sizeof(struct sockaddr_in);
      cSocket = accept(sSocket, (struct sockaddr*) &cAddr, &length);

      if (nbytes = recv(cSocket, (char*)&msgRecv, sizeof(struct srunEnvelope), 0) < 0)
      {
         fprintf(stderr,"SRUN Error: Error receiving message\n");
         exit(1);
      }
      printf("SRUN -> Received %d\n", msgRecv.msgType); fflush(stdout);
      switch (msgRecv.msgType)
      {
         case NEWPROC:

            // --> FASE 1 -- enviar a mensagem ao SPD responsavel pela criacao do respectivo processo
            if (mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) < 0)
            {
               fprintf(stderr,"SRUN Error: Could not create socket\n");
               exit(1);
            }
            mAddr.sin_family = AF_INET;
            mAddr.sin_port = htons(SPD_PORT);

            host = gethostbyname(pTable[np].hostName);
            if (host == NULL)
            {
               fprintf(stderr,"SRUN Error: Could not resolve %s host\n",pTable[np].hostName);
               exit(1);
            }
            memcpy(&mAddr.sin_addr, host->h_addr_list[0], host->h_length);

            if (connect(mSocket,(struct sockaddr*)  &mAddr, sizeof(struct sockaddr_in)) < 0)
            {
               fprintf(stderr,"SRUN Error: Startup Error. Could not connect to host %d\n",pTable[i].hostName);
               exit(1);
            }

            // FIXME - completar path do binario.
            // Token separador dos elementos do binario e seus argumentos eh o espaco em branco
            msgSend.msgType = STARTUP;
            strcpy(msgSend.message,bin);
            strcpy(msgSend.message," ");
            strcpy(msgSend.message,binArgs);

            if (send(mSocket, (char*)&msgSend, sizeof(struct srunEnvelope), 0) < 0)
            {
               fprintf(stderr,"SRUN Error: Error sending startup message\n");
               exit(1);
            }
            if (recv(mSocket, (char*)&msgRecv, sizeof(struct srunEnvelope), 0) < 0)
            {
               fprintf(stderr,"SRUN Error: Error receiving por number form SPD\n");
               exit(1);
            }
            // unmarshalling do numero da porta na mensagem
            if (msgRecv.msgType == SETPORT)
               sscanf(msgRecv.message,"%d",&pTable[np].port);
            else
            {
               fprintf(stderr,"SRUN Error: Protocol error\n");
               exit(1);
            }
            np++;   		//incremento o numero de processos
            close(cSocket);
            close(mSocket);
            break;
         case FINALIZE:

            totalNp++;

            if (totalNp == np)
               condition=FINISHED;
            close(cSocket);
            break;

         default:
            fprintf(stderr,"SRUN Error: Protocol error - Invalid Message\n");
            exit(1);

      }

   }
   close(sSocket);


   return 0;
}
