
#ifndef _COM_MPIHH_
#define _COM_MPIHH_
#ifdef OOP_MPI
#include "mpi.h"
#endif

#include "oopmpistorage.h"
#include "oopcommmanager.h"

#include <semaphore.h>

 
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
  void SetKeepReceiving(bool go);
 private:	
  /**
   * Opens Communication, initializing all the processes. 
   * If this processor was created by user, returns the 
   * number of opened processes. Else, returns 0
   * @param *process_name
   * @param num_of_process
   */
  int     Initialize (char * argv, int argc);//(int arg_c, char **arg_v);
 public:
#ifdef MTSEND
  static void * SendTaskMT(void * Data); 
#endif
  void UnlockReceiveBlocking();
  /* Sends all messages in all buffers
   */
  virtual int SendMessages();
  /**
   * Blocking receive. Execution stops and waits until a 
   * posted message is received. This methos triggers listening thread, which is implemented by 
   * ReceiveMsgBlocking.
   */
  int ReceiveMessagesBlocking();
  /**
   * Multithreading blocking receive
   */
  static void * ReceiveMsgBlocking (void *t);
  
  /**
   * Retorna 0 se o processo n� tiver sido disparado 
   * pelo console. What does it really mean?
   */
  int IAmTheMaster();
  /**
   * Used for error management
   */
  char *ClassName();
        
 protected:
  /**
   * Terminates MPI execution
   */
  void Finish(char * msg);
  /**
   * Unpacks the received message
   * @param msg Received message to be unpacked
   */
  int ProcessMessage (OOPMPIStorageBuffer & msg);
  /**
   * Function called by TCommunicationManager::
   * SendTask(TTask*). Packs the message to be sent to
   * a SendStorage Buffer.
   * @param *pTask Pointer to TTask object to be packed.
   */
  int SendTask (OOPTask * pTask);
  /**
   * Receive buffer.
   */
  OOPMPIStorageBuffer m_RecvBuffer;

  /** Communication argument */
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


//
