#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include "srun.h" 


int main(int argc, char *argv[])
{
   // estruturas para comunicacao
   struct sockaddr_in sAddr, mAddr, cAddr;   // estruturas de endereco em modo servidor e cliente
   int sSocket, cSocket, mSocket;            // sSocket - modo servidor - ouve
				             // cSocket - comunicacao em modo servidor - comunica-se com cliente
				             // mSocket - modo mensageiro - para encaminhar mensagens
   struct hostent *host;
   char hostName[256], *buffer, tmpBuffer[262];
   int ret, nbytes, length, bufsize;
   struct srunEnvelope msgSend, msgRecv;           // envelope para comunicacao
   // final das estruturas

   // manipulacao de parametros
   int np, i, j, binIndex, porta;
   char hostFile[256] = "hostfile.oopar";
   char bin[256], binArgs[256] = "";
   
   // variaveis de controle

   printf("Teste Concluido\n");
   fflush(stdout);




      if ((mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      {
         fprintf(stderr,"SRUN Error: Could not create socket\n");
         exit(1);
      }
      mAddr.sin_family = AF_INET;
      mAddr.sin_port = htons(SPD_PORT);
      
      host = gethostbyname("localhost");
      if (host == NULL)
      {
         fprintf(stderr,"SRUN Error: Could not resolve  host\n");
         exit(1);
      }
      memcpy(&mAddr.sin_addr, host->h_addr, host->h_length);
      //mAddr.sin_addr.s_addr = inet_addr(pTable[i].hostName);

      if (connect(mSocket,(struct sockaddr*)  &mAddr, sizeof(struct sockaddr_in)) < 0)
      {
         fprintf(stderr,"SRUN Error: Startup Error. Could not connect to host \n");
         exit(1);
      }
      
      // FIXME - completar path do binario. 
      // Token separador dos elementos do binario e seus argumentos eh o espaco em branco
      msgSend.msgType = GETPORT;
      strcpy(msgSend.message,"Teste");
      //strcat(msgSend.message," ");
     // strcat(msgSend.message,binArgs);	
      
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
       sscanf(msgRecv.message,"%d", &porta); 
       printf("Port Nuumber: %d\n",porta ); fflush(stdout);

       return  0;
}
