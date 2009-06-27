//
// C++ Implementation: oopsocket
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopsocket.h"

#include <sstream>
#include <iostream>

using namespace std;

// Corpo dos metodos das classes

void *OOPSocket::receiver(void * data)
{
    OOPSocket *SOCKET = static_cast<OOPSocket*>(data);
    int cSocket, nbytes;
    OOP_SOCKET_Envelope envelope;
    struct sockaddr_in cAddr;
    socklen_t len;

    while (SOCKET->threadRunning)
    {
        len = sizeof(struct sockaddr_in);
        if ((cSocket = accept(SOCKET->sSocket, (struct sockaddr*) &cAddr, &len)) < 0)
        {
            fprintf(stderr,"SOCKET: Error on accepting connections\n"); fflush(stderr);
            perror("");
            exit(1);
        }

        nbytes = recv(cSocket, (char*)&envelope, sizeof(OOP_SOCKET_Envelope),0);
        if (nbytes < 0)
        {
            fprintf(stderr,"SOCKET: Error receiving data envelope\n"); fflush(stderr);
            perror("");
            exit(1);
        } else if(nbytes == 0)
        {
            fprintf(stderr,"SOCKET Error receiving: The peer has performed an orderly shutdown\n"); fflush(stderr);
            exit(1);
        }

        envelope.cSocket = cSocket;
        // tratamento da mensagem
        switch(envelope.idOp)
        {
            case FINALIZE:
                pthread_mutex_lock(&SOCKET->mutex);
                SOCKET->threadRunning = 0; // finaliza a thread;
                pthread_mutex_unlock(&SOCKET->mutex);
                continue;
                break;
            case SENDRECV: // manipulacao do buffer
            case BARRIER:
                pthread_mutex_lock(&SOCKET->mutex);
                SOCKET->receiveBuffer->push_back(envelope);
                pthread_mutex_unlock(&SOCKET->mutex);
                if(envelope.idOp == BARRIER)
                	SOCKET->barrier->post();
                else {
                    pthread_mutex_lock(&SOCKET->notifyAll_mutex);
                    pthread_cond_broadcast(&SOCKET->notifyAll);
                    pthread_mutex_unlock(&SOCKET->notifyAll_mutex);
                }
                break;
            default:
                fprintf(stderr, "SOCKET: Error on envelope operation!"); fflush(stderr);
                exit(1);
                break;
        }
    }
    return 0;
}


pthread_mutex_t fThread = PTHREAD_MUTEX_INITIALIZER;
int idThread = 0;
void *OOPSocket::sender(void * data)
{
    int myId;
    pthread_mutex_lock(&fThread);
    myId = idThread;
    idThread++;
    pthread_mutex_unlock(&fThread);

    OOPSocket *SOCKET = static_cast<OOPSocket*>(data);
    pthread_mutex_lock(&(SOCKET->mutex));
    while (SOCKET->threadRunning)
    {
        pthread_mutex_unlock(&(SOCKET->mutex));

        pthread_mutex_lock((*SOCKET->notifyThreads_mutex)[myId]);
        SOCKET_Thread_Message m = (*(SOCKET->messages))[myId];
        if(m.buf != NULL)
        {
            pthread_mutex_lock((*SOCKET->receivers)[m.dest]);
            SOCKET->Send(&(*m.buf)[0], m.buf->Size(), m.dtype, m.dest, m.tag);
            pthread_mutex_unlock((*SOCKET->receivers)[m.dest]);
            delete m.buf;
            (*(SOCKET->messages))[myId].buf = NULL;
            (*isRunning)[thread] = false;
        } else {
        	pthread_cond_wait((*SOCKET->notifyThreads)[myId],(*SOCKET->notifyThreads_mutex)[myId]);
        }
        pthread_mutex_unlock((*SOCKET->notifyThreads_mutex)[myId]);
        pthread_mutex_lock(&(SOCKET->mutex));
    }
    return 0;
}


OOPSocket::OOPSocket()
{
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_init(&notifyAll, NULL);
    pthread_mutex_init(&notifyAll_mutex, NULL);

    barrier = new boost::interprocess::interprocess_semaphore(0);

    threadRunning = 1;
    receiveBuffer = new vector<OOP_SOCKET_Envelope>();

    uint i;
    // criando mutex para notificacao de threads
    notifyThreads_mutex = new vector<pthread_mutex_t*>();
    for(i=0; i<SEND_NUM_THREADS; i++)
    {
        pthread_mutex_t *t;
        t = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(t, NULL);
        notifyThreads_mutex->push_back(t);
    }
    // criando sinais de notificacao de threads
    notifyThreads = new vector<pthread_cond_t*>();
    for(i=0; i<SEND_NUM_THREADS; i++)
    {
        pthread_cond_t *t;
        t = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
        pthread_cond_init(t, NULL);
        notifyThreads->push_back(t);
    }
    // criando vetor de mensagens das threads
    messages = new vector<SOCKET_Thread_Message>();
    for(i=0; i<SEND_NUM_THREADS; i++)
    {
        SOCKET_Thread_Message m;
        m.buf = NULL;
        messages->push_back(m);
    }
    // criando as threads de envio
    threads = new vector<pthread_t>();
    isRunning = new vector<bool>();
    for(i=0; i<SEND_NUM_THREADS; i++)
    {
        pthread_t s;
        if(pthread_create(&s, NULL, sender, this))
        {
            printf("SOCKET Erro: Could not create send thread."); fflush(stdout);
            exit(1);
        }
        threads->push_back(s);
        isRunning->push_back(false);
    }
}


OOPSocket::~OOPSocket()
{
    // joining das threads
    for(uint i=0; i<threads->size(); i++)
    {
        (*isRunning)[i] = true;
        SOCKET_Thread_Message m;
        m.buf = NULL;
        pthread_mutex_lock((*notifyThreads_mutex)[i]);
        (*messages)[i] = m;
        pthread_cond_signal((*notifyThreads)[i]);
        pthread_mutex_unlock((*notifyThreads_mutex)[i]);
        pthread_join((*threads)[i], NULL);
    }
    delete isRunning;
    delete messages;
    delete threads;
    // destruindo os mutex de notificacao de threads
    vector<pthread_mutex_t*>::iterator it_t;
    it_t = notifyThreads_mutex->begin();
    while(it_t != notifyThreads_mutex->end())
    {
        pthread_mutex_t *mt = *it_t;
        pthread_mutex_destroy(mt);
        free(mt);
        it_t++;
    }
    delete notifyThreads_mutex;
    // destruindo os sinais de notificacao de threads
    vector<pthread_cond_t*>::iterator it_c;
    it_c = notifyThreads->begin();
    while(it_c != notifyThreads->end())
    {
        pthread_cond_t *mt = *it_c;
        pthread_cond_destroy(mt);
        free(mt);
        it_c++;
    }
    delete notifyThreads;

    pthread_cond_destroy(&notifyAll);
    pthread_mutex_destroy(&notifyAll_mutex);

    pthread_mutex_destroy(&mutex);
    delete barrier;
}

pthread_mutex_t gethostbyname_mutex = PTHREAD_MUTEX_INITIALIZER;
int OOPSocket::Barrier()
{
    int i, count = size - 1;
    int cSockets[size];
    struct sockaddr_in cAddr;
    struct hostent *host;
    OOP_SOCKET_Envelope msgSend;
    int cSocket;
    vector<OOP_SOCKET_Envelope>::iterator iter;

    if(rank)
    {
        if ((cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            stringstream sout;
            sout << "SOCKET Error: Could not create socket\n";
            cout << sout.str().c_str();
            cout.flush();
            exit(1);
        }
        bzero((char *) &cAddr, sizeof(cAddr));
        cAddr.sin_family = AF_INET;
        cAddr.sin_port = htons(pTable[0].port);

        pthread_mutex_lock(&gethostbyname_mutex);
        host = gethostbyname(pTable[0].hostName);
        if (host == NULL)
        {
            stringstream sout;
            sout << "SOCKET Error: Could not resolve host\n";
            cout << sout.str().c_str();
            cout.flush();
            exit(1);
        }

        memcpy(&cAddr.sin_addr, host->h_addr, host->h_length);
        pthread_mutex_unlock(&gethostbyname_mutex);

        if (connect(cSocket,(struct sockaddr*)&cAddr, sizeof(struct sockaddr_in)) < 0)
        {
            fprintf(stderr,"SOCKET Error: BARRIER. Could not connect to host\n"); fflush(stderr);
            perror("");
            exit(1);
        }
        msgSend.idOp = BARRIER;
        msgSend.idSender = rank;

        if (send(cSocket, &msgSend, sizeof(OOP_SOCKET_Envelope), 0) < 0)
        {
            fprintf(stderr,"SOCKET Error: Error sending notify message\n"); fflush(stderr);
            perror("");
            exit(1);
        }

        if (recv(cSocket, (char*)&msgSend, sizeof(OOP_SOCKET_Envelope), 0) < 0)
        {
            fprintf(stderr,"SOCKET Error (Barrier): Error receiving notify message\n"); fflush(stderr);
            perror("");
            exit(1);
        }

        if(close(cSocket)<0)
            perror("Barrier 1 close():");
    }
    else // Rank == 0
    {
        while(count > 0)
        {
        	barrier->wait();
            pthread_mutex_lock(&mutex);
            iter = receiveBuffer->begin();
            while(iter != receiveBuffer->end())
            {
                if(iter->idOp == BARRIER)
                {
                    cSockets[iter->idSender] = iter->cSocket;
                    receiveBuffer->erase(iter);
                    count--;
                    break;
                }
                iter++;
            }
            pthread_mutex_unlock(&mutex);
        }
        for(i=1;i<size;i++)
        {
            msgSend.idOp = BARRIER;
            msgSend.idSender = 0;
            cSocket = cSockets[i];

            if (send(cSocket, &msgSend, sizeof(OOP_SOCKET_Envelope), 0) < 0)
            {
                fprintf(stderr,"SOCKET Error: Error sending notify message\n"); fflush(stderr);
                perror("");
                exit(1);
            }

            if(close(cSocket)<0)
                perror("Barrier 2 close():");
        }
    }
    return SOCKET_SUCCESS;
}

void OOPSocket::Finalize()
{
    struct sockaddr_in cAddr;
    struct hostent *host;
    struct srunEnvelope msgSend;
    int cSocket;

    if ((cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
       stringstream sout;
       sout << "SOCKET Error: Could not create socket\n";
       cout << sout.str().c_str();
       cout.flush();
       exit(1);
    }
    bzero((char *) &cAddr, sizeof(cAddr));
    cAddr.sin_family = AF_INET;
    cAddr.sin_port = htons(SPD_PORT);

    pthread_mutex_lock(&gethostbyname_mutex);
    host = gethostbyname("localhost");
    if (host == NULL)
    {
       stringstream sout;
       sout << "SOCKET Error: Could not resolve SPD host\n";
       cout << sout.str().c_str();
       cout.flush();
       exit(1);
    }

    memcpy(&cAddr.sin_addr, host->h_addr, host->h_length);
    pthread_mutex_unlock(&gethostbyname_mutex);
    //mAddr.sin_addr.s_addr = inet_addr(pTable[i].hostName);

    if (connect(cSocket,(struct sockaddr*)&cAddr, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr,"SOCKET Error: FINALIZE. Could not connect to SPD host\n"); fflush(stderr);
        perror("");
       exit(1);
    }
    msgSend.msgType = FINALIZE;
    sprintf(msgSend.message,"%d %d",sPort, rank);

    if (send(cSocket, (char*)&msgSend, sizeof(struct srunEnvelope), 0) < 0)
    {
        fprintf(stderr,"SOCKET Error: Error sending notify message\n"); fflush(stderr);
        perror("");
        exit(1);
    }

    if(close(cSocket)<0)
        perror("Finalize 1 close():");

    // destruindo os mutex dos receptores
    vector<pthread_mutex_t*>::iterator it;
    it = receivers->begin();
    while(it != receivers->end())
    {
        pthread_mutex_t *mt = *it;
        pthread_mutex_destroy(mt);
        it++;
    }
    delete receivers;

    pthread_join(receiverT, NULL);
    if(close(sSocket)<0)
        perror("Finalize 2 close();");
}


int OOPSocket::Probe(int src, int tag, SOCKET_Status *st)
{
    int found=0;
    OOP_SOCKET_Envelope msgRecv;
    vector<OOP_SOCKET_Envelope>::iterator iter;

    while(!found)
    {
        pthread_mutex_lock(&mutex);
        iter = receiveBuffer->begin();
        while(iter != receiveBuffer->end())
        {
            if(iter->idOp == SENDRECV)
            {
                if ((iter->tag == tag || tag == SOCKET_ANY_TAG) && (iter->idSender== src || src == SOCKET_ANY_SOURCE))
                {
                    msgRecv = *iter;
                    st->source = msgRecv.idSender;
                    st->tag = iter->tag;
                    pthread_mutex_unlock(&mutex);
                    return SOCKET_SUCCESS;
                }
            }
            iter++;
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&notifyAll_mutex);
        pthread_cond_wait(&notifyAll, &notifyAll_mutex);
        pthread_mutex_unlock(&notifyAll_mutex);
    }
    return SOCKET_ERR_COMM;
}


int OOPSocket::Get_count(SOCKET_Status *st, int dtype, int *count)
{
    vector<OOP_SOCKET_Envelope>::iterator iter;

    pthread_mutex_lock(&mutex);
    iter = receiveBuffer->begin();
    while(iter != receiveBuffer->end())
    {
        if(iter->idOp == SENDRECV)
        {
            if ((iter->tag == st->tag || st->tag == SOCKET_ANY_TAG) && (iter->idSender== st->source || st->source == SOCKET_ANY_SOURCE) && (iter->dtype == dtype))
            {
                *count = iter->eCount;
                pthread_mutex_unlock(&mutex);
                return SOCKET_SUCCESS;
            }
        }
        iter++;
    }
    pthread_mutex_unlock(&mutex);
    return SOCKET_ERR_TYPE;
}


int OOPSocket::Recv(void *buf, int count, int dtype, int src, int tag, SOCKET_Status *st)
{
    int cSocket, found=0;
    int datasize, nbytes, total;
    OOP_SOCKET_Envelope msgRecv;
    vector<OOP_SOCKET_Envelope>::iterator iter;

    while(!found)
    {
        pthread_mutex_lock(&mutex);
        iter = receiveBuffer->begin();
        while(iter != receiveBuffer->end())
        {
            if(iter->idOp == SENDRECV)
            {
                if ((iter->tag == tag || tag == SOCKET_ANY_TAG) && (iter->idSender== src || src == SOCKET_ANY_SOURCE))
                {
                    msgRecv = *iter;
                    receiveBuffer->erase(iter);
                    found=1;
                    break;
                }
            }
            iter++;
        }
        pthread_mutex_unlock(&mutex);
        if(!found) {
            pthread_mutex_lock(&notifyAll_mutex);
            pthread_cond_wait(&notifyAll, &notifyAll_mutex);
            pthread_mutex_unlock(&notifyAll_mutex);
        }
    }
    cSocket = msgRecv.cSocket;
    st->source = msgRecv.idSender;
    st->tag = msgRecv.tag;

    msgRecv.idSender = rank;
    msgRecv.idOp = SENDRECV;

    //printf("RECV open sckt %d - rk %d - thrd %d\n",cSocket,rank,pthread_self()); fflush(stdout);

    if (send(cSocket, (char*)&msgRecv, sizeof(OOP_SOCKET_Envelope), 0) < 0)
    {
        fprintf(stderr,"RECV SOCKET Error: Error sending notify message\n"); fflush(stderr);
        perror("");
        fflush(stdout);
        exit(1);
    }

    switch(dtype)
    {
        case SOCKET_INT:
            datasize=sizeof(int)*count;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*count;
            break;
        case SOCKET_CHAR:
            datasize=sizeof(char)*count;
            break;
        case SOCKET_PACKED:
            datasize=msgRecv.eCount;
            break;
    }
    total=0;
    do
    {
        nbytes = recv(cSocket, ((char*) buf)+total, datasize-total, 0);
        //printf("RECV nb %d - tot %d - sz %d\n",nbytes,total,datasize);fflush(stdout);
        if (nbytes < 0)
        {
            fprintf(stderr,"RECV SOCKET Error: Error on receiving data message\n"); fflush(stderr);
            fflush(stdout);
            exit(1);
        } if(nbytes == 0)
        {
            fprintf(stderr,"RECV SOCKET: The peer has performed an orderly shutdown\n"); fflush(stderr);
            fflush(stdout);
            exit(1);
        }
        total+=nbytes;
    } while(total<datasize);
    close(cSocket);

    return SOCKET_SUCCESS;
}

int OOPSocket::Send(void *buf, int count, int dtype, int dest, int tag)
{
    int cSocket;
    int datasize, nbytes, total;
    struct sockaddr_in cAddr;
    struct hostent *host;
    OOP_SOCKET_Envelope msgSend;

    if ((cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        stringstream sout;
        sout << "SEND SOCKET Error: Could not create socket\n";
        cout << sout.str().c_str();
        cout.flush();
        exit(1);
    }

    bzero((char *) &cAddr, sizeof(cAddr));
    cAddr.sin_family = AF_INET;
    cAddr.sin_port = htons(pTable[dest].port);
    pthread_mutex_lock(&gethostbyname_mutex);
    host = gethostbyname(pTable[dest].hostName);
    if (host == NULL)
    {
        stringstream sout;
        sout << "SEND SOCKET Error: Could not resolve host\n";
        cout << sout.str().c_str();
        sout << "rank "<<rank<<" dest "<<dest<<" host "<<pTable[dest].hostName<<"\n";
        cout << sout.str().c_str();
        cout.flush();
        exit(1);
    }

    memcpy(&cAddr.sin_addr, host->h_addr, host->h_length);
    pthread_mutex_unlock(&gethostbyname_mutex);
    if (connect(cSocket,(struct sockaddr*)&cAddr, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr,"SEND SOCKET Error: SEND. Could not connect to host\n"); fflush(stderr);
        perror("");
        exit(1);
    }
    msgSend.idOp = SENDRECV;
    msgSend.idSender = rank;
    msgSend.tag = tag;
    msgSend.eCount = count;
    msgSend.dtype = dtype;

    //printf("SEND open sckt %d - rk %d - thrd %d\n",cSocket,rank,pthread_self()); fflush(stdout);
    if (send(cSocket, &msgSend, sizeof(OOP_SOCKET_Envelope), 0) < 0)
    {
        fprintf(stderr,"SEND SOCKET Error: Error sending notify message\n"); fflush(stderr);
        perror("");
        exit(1);
    }

    //printf("Send: rank: %d, tag: %d, count: %d, dtype: %d\n",rank,tag,count,dtype);fflush(stdout);
    nbytes = recv(cSocket, &msgSend, sizeof(OOP_SOCKET_Envelope), 0);
    if (nbytes < 0)
    {
        fprintf(stderr,"SEND SOCKET Error: Error receiving notify message\n"); fflush(stderr);
        perror("");
        exit(1);
    } else if (nbytes == 0)
    {
        fprintf(stderr,"SEND SOCKET Error: The peer has performed an orderly shutdown\n"); fflush(stderr);
        exit(1);
    }

    // preparar para enviar efetivamente os dados
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*count;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*count;;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*count;
            break;
        case SOCKET_PACKED:
            datasize=count;
            break;
    }
    //printf("enviando a grande mensagem\n"); fflush(stdout);
    total = 0;
    do
    {
        nbytes = send(cSocket, ((char*)buf)+total, datasize-total, 0);
        //printf("SEND nb %d - tot %d - sz %d\n",nbytes,total,datasize);fflush(stdout);
        if (nbytes < 0)
        {
            fprintf(stderr,"SEND SOCKET: Error sending data\n"); fflush(stderr);
            perror("");
            exit(1);
        }
        total += nbytes;
    } while (total < datasize);
    close(cSocket);
    return 0;
}


pthread_mutex_t fSend = PTHREAD_MUTEX_INITIALIZER;
int OOPSocket::Send(OOPSocketStorageBuffer *buf, int dtype, int dest, int tag)
{
    pthread_mutex_lock(&fSend);
    int thread=0;
    while(true)
    {
        if((*isRunning)[thread] == false)
        {
            // thread se torna ocupada
            (*isRunning)[thread] = true;
            // guarda a mensagem no vetor
            SOCKET_Thread_Message m;
            m.buf = buf;
            m.dtype = dtype;
            m.dest = dest;
            m.tag = tag;
            // notifica que tem nova mensagem
            pthread_mutex_lock((*notifyThreads_mutex)[thread]);
            (*messages)[thread] = m;
            pthread_cond_signal((*notifyThreads)[thread]);
            pthread_mutex_unlock((*notifyThreads_mutex)[thread]);
            break;
        }
        thread = (thread+1)%(isRunning->size());
        if(thread == 0)
        {
            printf("%d - Thread capacity full: sleeping...\n", rank); fflush(stdout);
            sleep(1);
        }
    }
    pthread_mutex_unlock(&fSend);
    return 0;
}


int OOPSocket::Comm_size()
{
  return size;
}


int OOPSocket::Comm_rank()
{
  return rank;
}


void OOPSocket::Init_thread(int *argc, char ***argv)
{
    struct sockaddr_in cAddr, sAddr;
    struct hostent *host;
    struct srunEnvelope msgSend, msgRecv;
    int nbytes, bufsize	;
    socklen_t length;
    int cSocket;

    // 1a fase - criar socket para entrar em contato com SPD
    if ( (cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        fprintf(stderr,"SOCKET Error: Could not create socket\n"); fflush(stderr);
        perror("");
        exit(1);
    }
    bzero((char *) &cAddr, sizeof(cAddr));
    cAddr.sin_family = AF_INET;
    cAddr.sin_port = htons(SPD_PORT);

    pthread_mutex_lock(&gethostbyname_mutex);
    host = gethostbyname("localhost");
    if (host == NULL)
    {
        fprintf(stderr,"SOCKET Error: Could not resolve  host\n"); fflush(stderr);
        exit(1);
    }
    memcpy(&cAddr.sin_addr, host->h_addr, host->h_length);
    pthread_mutex_unlock(&gethostbyname_mutex);
    //mAddr.sin_addr.s_addr = inet_addr(pTable[i].hostName);

    //printf("THREAD_INIT: Conectando com SPD\n"); fflush(stdout);
    if (connect(cSocket,(struct sockaddr*)  &cAddr, sizeof(struct sockaddr_in)) < 0)
    {
         fprintf(stderr,"SOCKET Error: Startup Error. Could not connect to host \n"); fflush(stderr);
         perror("");
         exit(1);
    }

    // FIXME - completar path do binario.
    // Token separador dos elementos do binario e seus argumentos eh o espaco em branco
    msgSend.msgType = GETPORT;
    //strcpy(msgSend.message,"Teste");


    //printf("THREAD_INIT: Msg: %s\n",msgSend.message); fflush(stdout);
    if (send(cSocket, &msgSend, sizeof(struct srunEnvelope), 0) < 0)
    {
        fprintf(stderr,"SOCKET Error: Error sending startup message\n"); fflush(stderr);
        perror("");
        exit(1);
    }
    //printf(" Enviada mensagem de requisicao da porta\n");fflush(stdout);
    if (recv(cSocket, (char*)&msgRecv, sizeof(struct srunEnvelope), 0) < 0)
    {
        fprintf(stderr,"SOCKET Error: Error receiving por number form SPD\n"); fflush(stderr);
        perror("");
        exit(1);
    }
    sscanf(msgRecv.message,"%d", &sPort);

    //printf("THREAD_INIT:  Port Number: %d\n",sPort ); fflush(stdout);
    if(close(cSocket)<0)
        perror("Init_thread 1 close():");


    // fase 2 --> criar o socket para ouvir (socket servidor)
    if ((sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) )< 0)
    {
        fprintf(stderr,"SOCKET Error: Could not create server socket\n"); fflush(stderr);
        perror("");
        exit(1);
    }
    int yes =1;
    if (setsockopt(cSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) <0)
        perror("Init_thread-setsockopt() so_reuseaddr failed");

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(sPort);
    sAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(sAddr.sin_zero), '\0', 8);

    //bind da porta
    if (bind(sSocket, (struct sockaddr *)&sAddr, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr,"SOCKET Error: Error on binding listening address\n"); fflush(stderr);
        perror("");
        exit(1);
    }

    // listen da fila
    if (listen(sSocket,MAX_PROC) < 0)
    {
        fprintf(stderr,"SOCKET Error: Error on defining listening queue\n"); fflush(stderr);
        perror("");
        exit(1);
    }

    // fase 2a --> receber a lista de processos

    //printf("THREAD_INIT: - Fase 2a Aguardando solicitacao\n");fflush(stdout);
    length=sizeof(struct sockaddr_in);
    if((cSocket = accept(sSocket, (struct sockaddr*) &cAddr, &length)) < 0)
    {
         fprintf(stderr,"SOCKET Error: Error creating socket\n"); fflush(stderr);
         perror("");
         exit(1);
    }

    //printf("SOCKET-passou\n");fflush(stdout);

    if ((nbytes = recv(cSocket, (char*)&msgRecv, sizeof(struct srunEnvelope), 0)) < 0)
    {
        fprintf(stderr, "SOCKET Error: Error receiving process table Size\n"); fflush(stderr);
        perror("");
        exit(1);
    }

    sscanf(msgRecv.message,"%d %d %d" ,&bufsize, &size, &rank);
    //printf("THREAD_INIT: -> %d %d\n", msgRecv.msgType, size); fflush(stdout);

    if ((nbytes = recv(cSocket, (char*)&pTable, bufsize, 0)) < 0)
    {
        fprintf(stderr,"SOCKET Error:\n"); fflush(stderr);
        perror("");
        exit(1);
    }

    if(close(cSocket)<0)
        perror("Init_thread 2 close():");

    // imprimindo a tabela de processos
    //printf("**TABELA DE PROCESSOS**\n");fflush(stdout);
    //for (i=0;i<size;i++)
    //   printf("RANK %d --> %d %s %d %c\n",rank,i,pTable[i].hostName, pTable[i].port, pTable[i].inUse); fflush(stdout);

    // criando os mutex dos receptores
    receivers = new vector<pthread_mutex_t*>();
    for(int i=0; i<size; i++)
    {
        pthread_mutex_t *t;
        t = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(t, NULL);
        receivers->push_back(t);
    }

    // controla a execucao da thread Receiver
    if(pthread_create(&receiverT, NULL, receiver, this))
    {
        printf("SOCKET Erro: Could not create receive thread."); fflush(stdout);
        exit(1);
    }
    Barrier();
}


int OOPSocket::Pack_size(int incount, int dtype, int *size)
{
    int datasize;
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*incount;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*incount;;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*incount;
            break;
        case SOCKET_PACKED:
            datasize=incount;
            break;
        default:
            printf("SOCKET.Pack_size: Undefined SOCKET type: %d\n",dtype); fflush(stdout);
            return SOCKET_ERR_TYPE;
    }
    (*size)=datasize;
    return SOCKET_SUCCESS;
}


int OOPSocket::Pack(void *inb, int insize, int dtype, void *outb, int outsize, int *pos)
{
    int datasize;
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*insize;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*insize;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*insize;
            break;
        case SOCKET_PACKED:
            datasize=insize;
            break;
        default:
            printf("SOCKET.Pack: Undefined SOCKET type: %d\n",dtype); fflush(stdout);
            return SOCKET_ERR_TYPE;
    }
    if((*pos)+datasize > outsize)
    {
        printf("SOCKET.Pack: Outbuffer size out of bounds: %d\n",outsize); fflush(stdout);
        return SOCKET_ERR_COUNT;
    }
    memcpy(((char*)outb)+(*pos),inb,datasize);
    (*pos) = (*pos) + datasize;
    return SOCKET_SUCCESS;
}


int OOPSocket::Unpack(void *inb, int insize, int *pos, void *outb, int outsize, int dtype)
{
    int datasize;
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*outsize;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*outsize;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*outsize;
            break;
        case SOCKET_PACKED:
            datasize=outsize;
            break;
        default:
            printf("SOCKET.Unpack: Undefined SOCKET type: %d\n",dtype); fflush(stdout);
            return SOCKET_ERR_TYPE;
    }
    if((*pos)+datasize > insize)
    {
        printf("SOCKET.Unpack: Inbuffer size out of bounds: %d+%d=%d?\n",*pos,datasize,insize); fflush(stdout);
        return SOCKET_ERR_COUNT;
    }
    memcpy(outb,((char*)inb)+(*pos),datasize);
    (*pos) = (*pos) + datasize;
    return SOCKET_SUCCESS;
}


/// You can call MPI_TEST with a null or inactive request argument. The
/// operation returns flag = true and empty status.
/// http://www.physics.usyd.edu.au/~nigel/cosc3012/parallel/docs/tutorial_files/man/MPI_Test.txt
int OOPSocket::Test(int *request, int *flag, SOCKET_Status *st)
{
#ifndef BLOCKING
#error Nao ha tratamento para nao bloqueante
#endif
    (*flag)=1;
    return 1;
}

