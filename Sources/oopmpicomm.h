//
// Autor: F�io Amaral de Castro, RA: 991722
//
// E-mail: facastro99@yahoo.com
//
// Arquivo: com_mpi.h
//
// Classe: TMpiComManager
//
// Descr.: Classe para comunicacao utilizando MPI
//
// Versao: 12 / 2002
//
#ifndef _COM_MPIHH_
#define _COM_MPIHH_
#ifdef OOP_MPI
#include "mpi.h"
#endif

#include "oopmpistorage.h"
#include "oopcommmanager.h"

#include <semaphore.h>

class   OOPMPISendStorage;

/**
   Non abstract class which implements the OOPar Communicator Manager using the MPI (Message Passing Interface) communication libray.
*/
class   OOPMPICommManager:public OOPCommunicationManager
{
public:
  
	OOPMPICommManager ();
  /**
   * Constructor 
   */
	OOPMPICommManager (int &argc, char **argv);
  /**
   * Simple destructor
   */
	       ~OOPMPICommManager ();
  /**
   * Opens Communication, initializing all the processes. 
   * If this processor was created by user, returns the 
   * number of opened processes. Else, returns 0
   * @param *process_name
   * @param num_of_process
   */
private:	
	int     Initialize (char * argv, int argc);//(int arg_c, char **arg_v);
public:
        void UnlockReceiveBlocking();
	/* Sends all messages in all buffers
	*/
  virtual int SendMessages ();
  /** 
   * Nonblocking receive. If there is a posted message to 
   * receive, receives it and returns 1. Else, returns 0
   */
	int     ReceiveMessages ();
        int ReceiveMessagesBlocking();
  /**
   * Blocking receive. Execution stops and waits until a 
   * posted message is received
   */
	int     ReceiveBlocking ();
	/**
	 * Multithreading blocking receive
	 */
	static void * ReceiveMsgBlocking (void *t);
	static void * ReceiveMsgNonBlocking (void *t);
  /**
   * Retorna 0 se o processo n� tiver sido disparado 
   * pelo console. What does it really mean?
   */
        int     IAmTheMaster ();
  /**
   * Used for error management
   */
        char   *ClassName ();
        
      protected:
	/**
	 * Terminates MPI execution
	 */
	 void Finish(char * msg);
  /**
   * Unpacks the received message
   * @param msg Received message to be unpacked
   */
	int     ProcessMessage (OOPMPIStorageBuffer & msg);
  /**
   * Function called by TCommunicationManager::
   * SendTask(TTask*). Packs the message to be sent to
   * a SendStorage Buffer.
   * @param *pTask Pointer to TTask object to be packed.
   */
	int     SendTask (OOPTask * pTask);
  /** Array of send buffers */
	//OOPMPISendStorage f_sendbuffer;
  /** Reception object for non blocking receive */
	//OOPMPIStorageBuffer f_receivebuffer;
  /** Communication argument */
  /**
   * Send and receive buffer are the same
   */
    OOPMPIStorageBuffer f_buffer;
	int     f_argc;
  /** Communication argument */
	char  **f_argv;
	bool fReceiveThreadExists;
	pthread_t fReceiveThread;
        pthread_mutex_t fReceiveMutex;
        pthread_cond_t fReceiveCond;
        sem_t fReceiveSemaphore;
        bool fKeepReceiving;
};

#endif
