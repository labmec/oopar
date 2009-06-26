#include "../oopsocket.h"

/*#define Envio_MatA 100
#define Envio_MatB 101
#define Envio_MatC 102
#define SIZE 1000

double matA[SIZE][SIZE]; /* matriz 
double matB[SIZE][SIZE];
double matC[SIZE][SIZE];

int main(int argc,char *argv[])
{
   int i,j,k;
   int myrank,size,fator;

   SOCKET_Status status; /*variavel que precisa ser declarada porque ela � usada como argumento pelo MPI_Recv */

   SOCKET.Init_thread(&argc,&argv); /* faz o disparo nos processos MPI para cada n� */

   size = SOCKET.Comm_size(); /* fun��o que retorna o n�mero de n�s que est�o participando da execu��o. Este retorno fica guardado na vari�vel size */

   double trab[SIZE/size][SIZE];
   fator = SIZE/size;

   myrank = SOCKET.Comm_rank(); /* fun��o que retorna o rank de cada n�. Este rank fica guardado na vari�vel myrank. Esta fun��o � importante porque � apartir deste rank que � poss�vel identificar os n�s para poder distribuir as tarefas e fazer uma divis�o do c�digo. */

   //printf("MULTIMAT - Rank: %d\n",myrank);
   //printf("MULTIMAT - Size: %d\n",size);
   fflush(stdout);

/* At� este ponto todos os n�s est�o executando o c�digo acima */

   if(myrank==0)  /* se eu estou no n� com o rank 0 */
   {
      /* inicializando a matriz */

      for(i=0;i<SIZE;i++)
	  {
         for(j=0;j<SIZE;j++)
		 {
		    matA[i][j]=0.1;
			matB[i][j]=0.5;
		 }
	  }
      /* matrizes inicializadas */

      /* Nesta parte � feita a distribui��o das partes da matriz para os demais n�s. A matriz � dividida em quatro partes e 3 partes s�o enviadas. Uma pate � enviada para o n� com o rank 1, outra para o rank 2 e outra para o rank 3. A outra parte � executada pelo n� com o rank 0. Esta parte n�o precisa ser enviada porque � este n� que inicializou a matriz. Portanto ele j� possui esta parte */

      for(i=1;i<size;i++)
      {

         /* Argumentos do MPI_Send(posi��o inicial do dado a ser enviado, tamanho, tipo do dado, destino(Rank do n� do destino), endere�o do pacote(Tag), comunica��o do MPI( o padr�o � MPI_COMM_WORLD) ). */
         //printf("MULTIMAT - %d enviando para %d\n",myrank,i);fflush(stdout);
         SOCKET.Send(&matA[0][0]+(i*fator),fator*SIZE,SOCKET_DOUBLE,i,Envio_MatA);
         SOCKET.Send(&matB[0][0],SIZE*SIZE,SOCKET_DOUBLE,i,Envio_MatB);
      }

      //printf("MULTIMAT - Mestre - Antes da 1a barreira\n");fflush(stdout);
      SOCKET.Barrier();
      //printf("MULTIMAT - Mestre - Depois da 1a barreira\n"); fflush(stdout);


      /* Esta parte � a multiplica��o. */

      for(i=0;i<fator;i++)
	  {
        for(j=0;j<SIZE;j++)
		{
		   matC[i][j]=0;
           for(k=0;k<SIZE;k++)
		   {
              matC[i][j]+=matA[i][k]*matB[k][j];
		   }
		}
	  }
          printf("A: %f - B: %f - C: %f\n",matA[fator-1][SIZE-1],matB[SIZE-1][SIZE-1],matC[fator-1][SIZE-1]);fflush(stdout);

      /* Neste for a seguir o n� com o rank 0 fica recebendo os valores que s�o enviados pelos outros n�s. */

      /* MPI_Recv � uma rotina de recebimento de mensagem bloqueante. O MPI_Recv deve estar casado com o MPI_Send */

      /* Argumentos do MPI_Recv(endere�o da vari�vel que vai receber o dado, tamanho do dado recebido, tipo do dado, n� que enviou a mensagem( rank do remetente), endere�o do pacote (TAG), comunica��o do MPI, status da mensagem ). */
      //printf("MULTIMAT - Mestre - Antes da 2a barreira\n");fflush(stdout);
      SOCKET.Barrier();
      //printf("MULTIMAT - Mestre - Depois da 2a barreira\n");fflush(stdout);

      for(i=1;i<size;i++)
      {
          SOCKET.Recv(&matC[0][0]+(i*fator*SIZE),fator*SIZE,SOCKET_DOUBLE,i,Envio_MatC,&status);
        //printf("MULTIMAT - Mestre recebeu de %d\n" ,i); fflush(stdout);
      }

      printf("MULTIMAT - Valor da celula [99,99]: %f\n",matC[99][99]); fflush(stdout);

   }else  /* Se eu estou nos n�s com rank diferente de 0. Ou seja, eu estou nos n�s escravos. Todos os n�s escravos executam esta parte. */
   {

      /* Fica bloqueado at� receber a sua parte da matriz. Parte esta que � enviada pelo n� com rank 0. */
      printf("MULTIMAT - Escravo %d - Fator: %d\n",myrank, fator);
      fflush(stdout);

      //printf("MULTIMAT - Escravo %d antes de receber A e B\n" ,myrank); fflush(stdout);
      SOCKET.Recv(trab,fator*SIZE,SOCKET_DOUBLE,0,Envio_MatA,&status);
      SOCKET.Recv(matB,SIZE*SIZE,SOCKET_DOUBLE,0,Envio_MatB,&status);
      //printf("MULTIMAT - Escravo %d recebeu A e B\n",myrank);fflush(stdout);

      //printf("MULTIMAT - Escravo %d - Antes da 1a Barreira\n",myrank); fflush(stdout);
      SOCKET.Barrier();
      //printf("MULTIMAT - Escravo %d - Depois da 1a Barreira\n",myrank); fflush(stdout);
      /* Esta parte � a multiplicacao. */
      for(i=0;i<fator;i++)
	  {
        for(j=0;j<SIZE;j++)
		{
		   matA[i][j]=0;
		   for(k=0;k<SIZE;k++)
		   {
              matA[i][j]+=trab[i][k]*matB[k][j];
		   }
		}
	  }
          printf("A: %f - B: %f - C: %f\n",trab[fator-1][SIZE-1],matB[SIZE-1][SIZE-1],matA[fator-1][SIZE-1]);fflush(stdout);

      //printf("MULTIMAT - Escravo %d - Antes da 2a Barreira\n",myrank); fflush(stdout);
      SOCKET.Barrier();
      //printf("MULTIMAT - Escravo %d - Depois da 2a Barreira\n",myrank); fflush(stdout);
      SOCKET.Send(&matA[0][0],fator*SIZE,SOCKET_DOUBLE,0,Envio_MatC);
      //printf("MULTIMAT - Escravo %d - enviando matriz C\n",myrank); fflush(stdout);

   }

   /* rotina que finaliza a execu��o do processo MPI */

   //printf("MULTIMAT -  Todos - Antes da 3a barreira\n"); fflush(stdout);
   SOCKET.Barrier();
   //printf("MULTIMAT -  Todos - Depois da 3a barreira\n"); fflush(stdout);
   SOCKET.Finalize();
   //printf("MULTIMAT -  Finalizing....\n"); fflush(stdout);

   return 0;
}

#define TAMANHO 1000000
int elementos[TAMANHO];

int mainX(int argc, char* argv[])
{
    int size,rank,i;
    SOCKET_Status st;
    SOCKET.Init_thread(&argc, &argv);
    rank = SOCKET.Comm_rank();
    size = SOCKET.Comm_size();

    if (rank >= 2)
    {
        printf("RANK - %d Sou acima de 1 - fico em Barrier\n",rank);fflush(stdout);
        SOCKET.Barrier();
        SOCKET.Finalize();
        return 0;
    }

    printf("APLICACAO-Fiz alguma coisa: %d\n",sizeof(int));fflush(stdout);
    //printf("%d de %d Antes do Barrier", rank, size); fflush(stdout);
    //SOCKET.Barrier();
    //printf("%d - Depois do Barrier\n",rank); fflush(stdout);

    if (!rank)
    {
        for(i=0;i<TAMANHO;i++)
            elementos[i]=i+1;
        SOCKET.Send(elementos, TAMANHO, SOCKET_INT, 1, 0);
        printf("%d mandei\n" ,rank); fflush(stdout);
        for(i=0;i<10;i++)
            printf("-%d-",elementos[i]);
        printf("-%d-\n",elementos[TAMANHO-1]);

        SOCKET.Recv(elementos,TAMANHO, SOCKET_INT, 1, 1, &st);
        for(i=0;i<10;i++)
            printf("-%d-",elementos[i]);
        printf("-%d-\n",elementos[TAMANHO-1]);
        fflush(stdout);
    }
    else
    {
        SOCKET.Recv(elementos, TAMANHO, SOCKET_INT, 0, 0, &st);
        printf("%d recebi\n", rank);
        printf("-%d-%d-\n",elementos[0],elementos[TAMANHO-1]); fflush(stdout);
        for(i=0;i<TAMANHO;i++)
        {
              //printf("elementos[%d]=%d\n",i,elementos[i]);fflush(stdout);
              elementos[i]*=2;
        }
        SOCKET.Send(elementos,TAMANHO,SOCKET_INT, 0, 1);
    }
    SOCKET.Barrier();
    SOCKET.Finalize();
    return 0;
}
*/