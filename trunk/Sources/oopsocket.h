//
// C++ Interface: oopsocket
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef OOPSOCKET_H
#define OOPSOCKET_H

#include <pthread.h>
//#include <semaphore.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket/socket.h"

#include "oopsocketstorage.h"
class OOPSocketStorageBuffer;

#include <vector>
//128
#define RECEIVE_BUFFER_SIZE  128
#define SEND_NUM_THREADS     10

#define SOCKET_ANY_SOURCE   -1

#define SOCKET_ANY_TAG      -1

#define SOCKET_PACKED        0
#define SOCKET_INT           1
#define SOCKET_DOUBLE        2
#define SOCKET_CHAR          3

#define SOCKET_SUCCESS       0
#define SOCKET_ERR_COMM     -1
#define SOCKET_ERR_COUNT    -2
#define SOCKET_ERR_TYPE     -3
#define SOCKET_ERR_TAG      -4
#define SOCKET_ERR_RANK     -5

typedef struct
{
  int source;
  int tag;

} SOCKET_Status;

typedef struct
{
    OOPSocketStorageBuffer *buf;
    int dtype;
    int dest;
    int tag;
} SOCKET_Thread_Message;

using namespace std;

/**
	@author Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>
*/
class OOPSocket{
private:
    // lista de elementos do buffer de recepcao
    vector<OOP_SOCKET_Envelope> *receiveBuffer;
    // semaforos globais
		boost::interprocess::interprocess_semaphore * mutex, * barrier;
    //sem_t mutex, barrier;
    pthread_mutex_t sendrecv;

    int threadRunning;          // variavel para verificar se a thread esta ou nao em execucao

    int sSocket;
    int sPort;
    int size,rank;

    // atributo da thread de controle de recebimento.
    pthread_t receiverT;
    struct processTable pTable[MAX_PROC];

    // thread para recebimento dos evelopes
    static void *receiver(void *);

    // mutex de receptores
    vector<pthread_mutex_t> *receivers;
    // vetor de threads de send
    vector<pthread_t> *threads;
    // vetor de avaliacao da thread se esta executando ou em espera
    vector<bool> *isRunning;
    // vetor de notificacao de threads
    vector<pthread_mutex_t> *notifyThreads;
    // lista de mensagens das threads
    vector<SOCKET_Thread_Message> *messages;

    // thread para o envio de mensagens
    static void *sender(void *);

public:
    OOPSocket();

    ~OOPSocket();

    int Barrier();

    void Finalize();

    int Probe(int source, int tag, SOCKET_Status *st);

    int Get_count(SOCKET_Status *st, int dtype, int *count);

    int Recv(void *buf, int count, int dtype, int src, int tag, SOCKET_Status *st);

    int Send(void *buf, int count, int dtype, int dest, int tag);

    int Send(OOPSocketStorageBuffer *buf, int dtype, int dest, int tag);

    int Comm_size();

    int Comm_rank();

    void Init_thread(int *argc=NULL, char ***argv=NULL);

    static int Pack_size(int incount, int dtype, int *size);

    static int Pack(void *inb, int insize, int dtype, void *outb, int outsize, int *pos);

    static int Unpack(void *inb, int insize, int *pos, void *outb, int outsize, int dtype);

    int Test(int *request, int *flag, SOCKET_Status *st);
};

#endif
