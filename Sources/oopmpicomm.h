//
// Autor: Fábio Amaral de Castro, RA: 991722
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
#include "mpi.h"
#include "oopmpistorage.h"
#include "oopcommmanager.h"
class   OOPMPISendStorage;
class   OOPMPIReceiveStorage;

/**
   Non abstract class which implements the OOPar Communicator Manager using the MPI (Message Passing Interface) communication libray.
*/
class   OOPMPICommManager:public OOPCommunicationManager
{
      public:
  /**
   * Constructor 
   */
	OOPMPICommManager (int argc, char **argv);
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
	int     Initialize (char * argv, int argc);//(int arg_c, char **arg_v); 
	/* Sends all messages in all buffers
	*/
	virtual int SendMessages () {return 0;}
  /**
   * Nonblocking receive. If there is a posted message to 
   * receive, receives it and returns 1. Else, returns 0
   */
	int     ReceiveMessages ();
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
   * Retorna 0 se o processo não tiver sido disparado 
   * pelo console. What does it really mean?
   */
	int     IAmTheMaster ()
	{
		return (f_num_proc > 0);
	}
  /**
   * Used for error management
   */
	char   *ClassName ()
	{
		return ("TMpiComManager::");
	}
      protected:
	/**
	 * Terminates MPI execution
	 */
	 void Finish(char * msg);
  /**
   * Unpacks the received message
   * @param msg Received message to be unpacked
   */
	int     ProcessMessage (OOPMPIReceiveStorage & msg);
  /**
   * Function called by TCommunicationManager::
   * SendTask(TTask*). Packs the message to be sent to
   * a SendStorage Buffer.
   * @param *pTask Pointer to TTask object to be packed.
   */
	int     SendTask (OOPTask * pTask);
  /** Array of send buffers */
	OOPMPISendStorage f_sendbuffer;
  /** Reception object for non blocking receive */
	OOPMPIReceiveStorage f_receivebuffer;
  /** Communication argument */
	int     f_argc;
  /** Communication argument */
	char  **f_argv;
	bool fReceiveThreadExists;
	pthread_t fReceiveThread;
};

#endif
