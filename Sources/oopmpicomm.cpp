// 
#include "mpi.h"
#include <stdio.h>

#include <string>
#include <pthread.h>
#include "oopmpicomm.h"
#include "ooptaskmanager.h"


#include <sstream>
#include <pzlog.h>

#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPar.OOPMPICommManager"));
#endif

using namespace std;
extern OOPTaskManager *TM;
pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;

OOPMPICommManager::OOPMPICommManager()
{
#ifdef LOGPZ
  LOGPZ_WARN(logger, "Empty Constructor should never be called!");
#endif
}
OOPMPICommManager::OOPMPICommManager (int &argc, char **argv)
{
  f_myself = -1;
  f_num_proc = 0;
  fReceiveThreadExists=false;
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Initializing MPICommManager" << std::endl;
    cout << sout.str().c_str();
    cout.flush();
  }
#endif
  int MT_Wanted = MPI_THREAD_MULTIPLE;
  int MT_Provided = 0;
  MPI_Init_thread(&argc,&argv, MT_Wanted, &MT_Provided);
  if(MT_Wanted != MT_Provided)
  {
    stringstream sout;
    sout << " ATTENTION ! Required Multi-Threading Level Differs from Provided MT Level\n";
    sout << "Wanted Level " << MT_Wanted << " Received Level " << MT_Provided << std::endl;
    cout << sout.str().c_str();//LOGPZ_INFO(logger, sout.str());
    cout.flush();
  }
#ifdef LOGPZ
  { 
    stringstream sout;
    sout << "MPI_Init_thread called. Provided MT Level = " << MT_Provided << std::endl;
    cout << sout.str().c_str();//LOGPZ_INFO(logger, sout.str());
    cout.flush();
  }
#endif
  
  Initialize((char*)argv, argc);
#ifdef OOP_MPE
  //MPE_Init_log();
  //MPE_Describe_state( 1, 2, "Running", "yellow" );
  //MPE_Describe_state( 3, 4, "Idle", "yellow" );
#endif
  f_argc = argc;
  f_argv = argv;
  cout << "MPI_Init Called\n";
  cout.flush();
  fReceiveThread = 0;
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Initializing Mutex, ConditionVariable and Semaphore variables";
    cout << sout.str().c_str();//LOGPZ_INFO(logger, sout.str());
  }
#endif
  pthread_mutex_init(&fReceiveMutex, NULL);
  pthread_cond_init(&fReceiveCond, NULL);
  sem_init(&fReceiveSemaphore, 0, 0);
  SetKeepReceiving(true);
}
void OOPMPICommManager::SetKeepReceiving(bool go)
{
  fKeepReceiving = go;
}
OOPMPICommManager::~OOPMPICommManager ()
{
  Finish("Terminating MPICommManager");
}
// Diferente de PVM, o argumento Destino em mpi nao eh o endereco absoluto
// do
// destino, mas o relativo. ou seja, o comando MPI_INIT inicializa os
// processos e para cada um destina um numero inteiro diferente, a partir
// de 0,
// que �o processo que inicializa os demais.
// Para inicializar, necessita de variaveis int argc e char **argv.
int OOPMPICommManager::Initialize (char * argv, int argc)//(int arg_c, char **arg_v)
{
  MPI_Comm_size (MPI_COMM_WORLD, &f_num_proc);
  MPI_Comm_rank (MPI_COMM_WORLD, &f_myself);

#ifdef LOGPZ
  {
    stringstream sout;
    sout << "MPIComm Initialize f_myself " << f_myself << " f_num_proc " << f_num_proc;
    LOGPZ_INFO(logger, sout.str().c_str());
  }
#endif
  if (f_myself == 0)
    return f_num_proc;
  else
    return 0;
}
#ifdef MTSEND
void * OOPMPICommManager::SendTaskMT(void * Data)
{
  OOPTask * pTask = static_cast<OOPTask *>(Data);
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Sending Task Id:" << pTask->Id() << " ClassId:" << pTask->ClassId() <<
      " to proc " << pTask->GetProcID ();
    LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
  }
  int process_id = pTask->GetProcID ();
  if (process_id >= CM->NumProcessors() || process_id < 0) {
    stringstream sout;
    sout << "Sending Task to a processor which doesn't exist!\nFinishing MPICommManager !\nFarewell !";
#ifdef LOGPZ
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    //Finish("Sending Task to a processor which doesn't exist!\nFinishing MPICommManager !\nFarewell !");
    delete pTask;
    return NULL;
  }
  // Se estiver tentando enviar para mim mesmo.
  if (process_id == CM->GetProcID()) {
    stringstream sout;
    sout << "Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !";
#ifdef LOGPZ
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    ((OOPMPICommManager *)CM)->Finish("Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !");
    delete pTask;
    return NULL;
  }
  OOPMPIStorageBuffer lLocalBuffer;
  pthread_mutex_lock(&fCommunicate);
  pTask->Write(lLocalBuffer, 1);
  lLocalBuffer.Send(process_id);
  pthread_mutex_unlock(&fCommunicate);
  delete pTask;
  return NULL;

}
#endif
int OOPMPICommManager::SendTask (OOPTask * pTask)
{
  //OOPMPIStorageBuffer Buffer;
#ifdef MTSEND
  pthread_t lTId;
  pthread_create(&lTId, NULL, SendTaskMT, pTask);
  return 1;
#else
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Sending Task Id:" << pTask->Id() << " ClassId:" << pTask->ClassId() <<
      " to proc " << pTask->GetProcID ();
    LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
  }
  int process_id = pTask->GetProcID ();
  if (process_id >= f_num_proc || process_id < 0) {
    stringstream sout;
    sout << "Sending Task to a processor which doesn't exist!\nFinishing MPICommManager !\nFarewell !";
#ifdef LOGPZ
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    //Finish("Sending Task to a processor which doesn't exist!\nFinishing MPICommManager !\nFarewell !");
    delete pTask;
    return -1;
  }
  // Se estiver tentando enviar para mim mesmo.
  if (process_id == f_myself)
  {
    stringstream sout;
    sout << "Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !";
#ifdef LOGPZ
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    Finish("Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !");
    delete pTask;
    return -1;
  }
  pthread_mutex_lock(&fCommunicate);
  OOPMPIStorageBuffer Buffer;
  pTask->Write (Buffer, 1);
  pthread_mutex_unlock(&fCommunicate);
  Buffer.Send(process_id);
  delete pTask;
  return 1;
#endif
};
int OOPMPICommManager::ReceiveMessagesBlocking()
{
  int res = -1;
  res = pthread_create(&fReceiveThread, NULL, ReceiveMsgBlocking, this);
  if(res)
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Fail to create Blocking Receive thread";
    LOGPZ_DEBUG(logger,sout.str().c_str());
    cout << sout.str().c_str() << endl;
#endif
  }
  return res;
}
void * OOPMPICommManager::ReceiveMsgBlocking (void *t)
{
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Castting a local CM for the Receive Thread";
    LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif

  OOPMPICommManager *LocalCM=NULL;
  LocalCM = static_cast<OOPMPICommManager *>(t);
  if(!LocalCM)
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Local CM is invalid on the receive thread !\nNo can do !\nFarewell";
    LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
  }

#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Entering Infinit Receive loop";
    LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  while (LocalCM->fKeepReceiving){
    int ret = LocalCM->m_RecvBuffer.ReceiveBlocking();
    if (ret <= 0)
    {
#ifdef LOGPZ
      {
        stringstream sout;
        sout << "LocalCM->m_RecvBuffer.ReceiveBlocking() returned <= 0\nThis is Communication problem\nFarewell";
        LOGPZ_ERROR(logger,sout.str().c_str());
      }
#endif
      LocalCM->Finish("ReceiveBlocking <receive error>");
    }
#ifdef LOGPZ
    {
      stringstream sout;
      sout << "Data received on Buffer! Calling ProcessMessage";
      LOGPZ_DEBUG(logger,sout.str().c_str());
    }
#endif
    LocalCM->ProcessMessage(LocalCM->m_RecvBuffer);
#ifdef LOGPZ
    {
      stringstream sout;
      sout << "Messages Processed ! Going to receive blocking again";
      LOGPZ_DEBUG(logger,sout.str().c_str());
    }
#endif
  }
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Leaving ReceiveThread infinit loop " << LocalCM->f_myself;
    sout << " | Posting semaphore fReceiveSemaphore";
    LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  return NULL;
}
int OOPMPICommManager::ProcessMessage(OOPMPIStorageBuffer & msg)
{
  pthread_mutex_lock(&fCommunicate);
  TPZSaveable *obj = msg.Restore ();
  pthread_mutex_unlock(&fCommunicate);
  if (obj == NULL && this->fKeepReceiving) {
    Finish( "ReceiveMessages <Erro em Restore() do objeto>.\n" );
  }
  OOPTask *task = dynamic_cast<OOPTask *> (obj);
  if(task) {
    task->Submit();
  } else {
#ifdef LOGPZ
    std::stringstream sout;
    sout << "OOPMPICommManager::ProcessMessage received an object which is not a task";
    LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
    delete obj;
  }
  return 1;
}

void OOPMPICommManager::Finish(char * msg){
  cout << msg << endl;
  cout.flush();
  m_RecvBuffer.CancelRequest();
  cout << "Processor " << f_myself  << " reached synchronization point !" << endl;
  MPI_Barrier( MPI_COMM_WORLD );
  cout << "Calling Finilize for " << f_myself << endl;
  cout.flush();
  MPI_Finalize();
}

int OOPMPICommManager::IAmTheMaster(){
  return (f_myself == 0);
}

char * OOPMPICommManager::ClassName(){
  return ("OOPMpiCommManager::");
}

int OOPMPICommManager::SendMessages(){
  return 0;
}

void OOPMPICommManager::UnlockReceiveBlocking()
{
#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << "Setting KeepReceiving flag to FALSE on Processor " << CM->GetProcID();
    LOGPZ_DEBUG(logger,sout.str().c_str());
    cout << sout.str().c_str() << endl;
  }
#endif
  fKeepReceiving = false;
  int classid = -1;
  pthread_mutex_lock(&fCommunicate);
  OOPMPIStorageBuffer buff;
  buff.PkInt( &classid, 1);
  buff.Send( CM->GetProcID() );
  pthread_mutex_unlock(&fCommunicate);
#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << "Waiting for ReceiveThread sinalization on Processor " << CM->GetProcID();
    LOGPZ_DEBUG(logger,sout.str().c_str());
    cout << sout.str().c_str() << endl;
  }
#endif
  pthread_join(fReceiveThread, NULL);
#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << "Processor " << CM->GetProcID() << " Received ReceiveThread termination confirmation";
    LOGPZ_DEBUG(logger,sout.str().c_str());
    cout << sout.str().c_str() << endl;
  }
#endif
}
