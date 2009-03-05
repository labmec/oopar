#include "../oopsocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/timeb.h>

#define tag 100
#define BUF_SIZE 30000

int *my_matrix, rows, cols, pos;
char pack_buf[BUF_SIZE];
//struct _timeb start_time, end_time;
double tot_time;

int main(int argc,char *argv[])
{
    int i,j;
    int myrank, size;


    SOCKET_Status status; /*variavel que precisa ser declarada porque ela � usada como argumento pelo SOCKET_Recv */

    OOPSocket SOCKET;

    SOCKET.Init_thread(&argc,&argv); /* faz o disparo nos processos SOCKET para cada n� */

    size = SOCKET.Comm_size(); /* fun��o que retorna o n�mero de n�s que est�o participando da execu��o. Este retorno fica guardado na vari�vel size */

    myrank = SOCKET.Comm_rank(); /* fun��o que retorna o rank de cada n�. Este rank fica guardado na vari�vel myrank. Esta fun��o � importante porque � apartir deste rank que � poss�vel identificar os n�s para poder distribuir as tarefas e fazer uma divis�o do c�digo. */

    printf("Rank: %d\n",myrank);
    printf("Size: %d\n",size);
    fflush(stdout);

    /* At� este ponto todos os n�s est�o executando o c�digo acima */

    if(myrank==0)  /* se eu estou no n� com o rank 0 */
    {
        /* inicializando a matriz */
        rows = 100;
        cols = 50;
        my_matrix  = (int *)malloc(rows * cols * sizeof(int));


        for(i=0;i<rows;i++)
        {
            for(j=0;j<cols;j++)
            {
                my_matrix[(i*cols)+j]=(i*cols)+j;
                   // printf("estou aqui %d\n", ((i*rows)+j) ); fflush(stdout);
            }
        }

        /* matrizes inicializadas */

        /* Nesta parte � feita a distribui��o das partes da matriz para os demais n�s. A matriz � dividida em quatro partes e 3 partes s�o enviadas. Uma pate � enviada para o n� com o rank 1, outra para o rank 2 e outra para o rank 3. A outra parte � executada pelo n� com o rank 0. Esta parte n�o precisa ser enviada porque � este n� que inicializou a matriz. Portanto ele j� possui esta parte */

  //    _ftime(&start_time);

        pos = 0;
        printf("%d - Empacotando linhas\n",myrank);fflush(stdout);
        SOCKET.Pack(&rows, 1, SOCKET_INT, pack_buf, BUF_SIZE, &pos);
        printf("%d - Empacotando colunas\n",myrank);fflush(stdout);
        SOCKET.Pack(&cols, 1, SOCKET_INT, pack_buf, BUF_SIZE, &pos);
        printf("%d - Empacotando matriz\n",myrank); fflush(stdout);

        SOCKET.Pack(my_matrix, rows*cols, SOCKET_INT, pack_buf, BUF_SIZE, &pos);

        printf("estou aqui\n"); fflush(stdout);
        for(i=1;i<size;i++)
        {

            /* Argumentos do SOCKET_Send(posi��o inicial do dado a ser enviado, tamanho, tipo do dado, destino(Rank do n� do destino), endere�o do pacote(Tag), comunica��o do SOCKET( o padr�o � SOCKET_COMM_WORLD) ). */

            SOCKET.Send(pack_buf, pos, SOCKET_PACKED, i, tag);
            printf("Enviei empacotado \n");fflush(stdout);
        }

        /* SOCKET_Recv � uma rotina de recebimento de mensagem bloqueante. O SOCKET_Recv deve estar casado com o SOCKET_Send */

        /* Argumentos do SOCKET_Recv(endere�o da vari�vel que vai receber o dado, tamanho do dado recebido, tipo do dado, n� que enviou a mensagem( rank do remetente), endere�o do pacote (TAG), comunica��o do SOCKET, status da mensagem ). */

        for(i=1;i<size;i++)
            SOCKET.Recv(&j,1,SOCKET_INT,i,tag,&status);

    //  _ftime(&end_time);

      //tot_time = end_time.time - start_time.time;
      //tot_time = tot_time + ((end_time.millitm - start_time.millitm)/(float)1000);

      //printf("Tempo de Recebimento: %f\n",tot_time);
      //fflush(stdout);

    /*  for(i=0;i<SIZE;i++)
        for(j=0;j<SIZE;j++)
        printf("MatC[%d][%d]: %f\n",i,j,matC[i][j]);
        fflush(stdout);
    */
    }else  /* Se eu estou nos n�s com rank diferente de 0. Ou seja, eu estou nos n�s escravos. Todos os n�s escravos executam esta parte. */
    {

        /* Fica bloqueado at� receber a sua parte da matriz. Parte esta que � enviada pelo n� com rank 0. */
        SOCKET.Recv(pack_buf, BUF_SIZE, SOCKET_PACKED, 0, tag, &status);
        printf("estou aqui2\n"); fflush(stdout);

        pos = 0;
        printf("%d - Desempacotando linhas\n", myrank); fflush(stdout);
        SOCKET.Unpack(pack_buf, BUF_SIZE, &pos, &rows, 1, SOCKET_INT);
        printf("%d - Desempacotando colunas\n", myrank); fflush(stdout);
        SOCKET.Unpack(pack_buf, BUF_SIZE, &pos, &cols, 1, SOCKET_INT);

        my_matrix  = (int *) malloc (rows * cols * sizeof(int));
        printf("%d - Desempacotando matriz\n", myrank); fflush(stdout);
        SOCKET.Unpack(pack_buf, BUF_SIZE, &pos, my_matrix, rows*cols, SOCKET_INT);

        /* Esta parte � a impressao. */

        printf("%d: Valor: %d\n",myrank,my_matrix[301]); fflush(stdout);

        SOCKET.Send(&pos,1,SOCKET_INT,0,tag);

    }

    /* rotina que finaliza a execu��o do processo SOCKET */

    SOCKET.Finalize();

    return 0;
}


