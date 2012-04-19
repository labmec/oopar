/**
 * @file
 */

#ifndef OOPSOCKET_H
#define OOPSOCKET_H

#include <string.h>
#include <errno.h>
#include <pthread.h>
//#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket.h"

#include "oopsocketstorage.h"
#include "TPZSemaphore.h"
class OOPSocketStorageBuffer;

#include <vector>

/**
 * @addtogroup socket
 * @{
 */

const int RECEIVE_BUFFER_SIZE = 128;
const int SEND_NUM_THREADS   =  10;

const int SOCKET_ANY_SOURCE =  -1;

const int SOCKET_ANY_TAG   = -1;


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
 * @brief Implements the socket object into the oopar environment
 * @author Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>
 */
class OOPSocket{
private:
    // lista de elementos do buffer de recepcao
    vector<OOP_SOCKET_Envelope> *receiveBuffer;
	
    // notificacao de recebimento de mensagens do tipo SEND/RECV
    pthread_cond_t notifyAll;
    pthread_mutex_t notifyAll_mutex;
	
    // semaforos globais
    TPZSemaphore *barrier;
//    boost::interprocess::interprocess_semaphore *barrier;
    pthread_mutex_t mutex;
	
	
    int threadRunning;          // variavel para verificar se a thread esta ou nao em execucao
	
    int sSocket;
    int sPort;
    int size,rank;
	
    // atributo da thread de controle de recebimento.
    pthread_t receiverT;
    struct processTable pTable[MAX_PROC];
	
    // thread para recebimento dos evelopes
    static void *receiver(void *);
	
    // mutex de receptores (para envio individual e garantia de sequencia de envio)
    vector<pthread_mutex_t*> *receivers;
    // vetor de threads de sender
    vector<pthread_t> *threads;
    // vetor de mutex de threads de envio (sender)
    vector<pthread_mutex_t*> *notifyThreads_mutex;
    // vetor de notificacao de threads de envio (sender)
    vector<pthread_cond_t*> *notifyThreads;
    // lista de mensagens das threads de envio (sender)
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

/**
 * @}
 */

#endif
