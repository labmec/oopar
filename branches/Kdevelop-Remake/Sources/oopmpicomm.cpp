#ifdef OOP_MPI

#include "mpi.h"
#include <stdio.h>

#include <string>
#include <pthread.h>
#include <signal.h>
#include "oopmpicomm.h"
#include "ooptaskmanager.h"

#include <sys/types.h>
#include <unistd.h>

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

class   OOPMPIStorageBuffer;
class   OOPMPICommManager;
using namespace std;
extern OOPTaskManager *TM;
pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;

OOPMPICommManager::OOPMPICommManager(){
#ifdef LOGPZ
  LOGPZ_WARN(logger, "Empty Constructor should never be called!");
#endif
}
OOPMPICommManager::OOPMPICommManager (int &argc, char **argv)
{
  f_myself = -1;
  f_num_proc = 0;
  fReceiveThreadExists=false;
  cout << "Initializing MPI !\n Calling MPI_Init\n";
  cout.flush();
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Initializing MPICommManager";
    cout << sout.str();//LOGPZ_INFO(logger, sout.str());
  }
#endif
  MPI_Init(&argc,&argv); 
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
    cout << sout.str();//LOGPZ_INFO(logger, sout.str());
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
    LOGPZ_INFO(logger, sout.str());
  }
#endif
  if (f_myself == 0)
    return f_num_proc;
  else
    return 0;
}
int OOPMPICommManager::SendTask (OOPTask * pTask)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Sending Task Id:" << pTask->Id() << " ClassId:" << pTask->ClassId() << 
      " to proc " << pTask->GetProcID ();
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  int process_id = pTask->GetProcID ();
  if (process_id >= f_num_proc) {
    stringstream sout;
    sout << "Sending Task to a processor which doesn't exist!\nFinishing MPICommManager !\nFarewell !";
#ifdef LOGPZ  
    LOGPZ_ERROR(logger,sout.str());
#endif
    Finish("Sending Task to a processor which doesn't exist!\nFinishing MPICommManager !\nFarewell !");
    delete pTask;
    return -1;
  }
  // Se estiver tentando enviar para mim mesmo.
  if (process_id == f_myself) {
    stringstream sout;
    sout << "Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !";
#ifdef LOGPZ  
    LOGPZ_ERROR(logger,sout.str());
#endif
    Finish("Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !");
    delete pTask;
    return -1;
  }
  pTask->Write (f_buffer, 1);
  f_buffer.Send(process_id);
  delete pTask;
  return 1;
};
int OOPMPICommManager::ReceiveMessages ()
{

  f_buffer.Receive();
  while(f_buffer.TestReceive()) {
    ProcessMessage(f_buffer);
    f_buffer.Receive();
  }
  return 1;
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
    LOGPZ_DEBUG(logger,sout.str());
    cout << sout.str() << endl;
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
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  
  OOPMPICommManager *LocalCM=NULL;
  LocalCM = static_cast<OOPMPICommManager *>(t);
  if(!LocalCM)
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Local CM is invalid on the receive thread !\nNo can do !\nFarewell";
    LOGPZ_DEBUG(logger,sout.str());
#endif
  } 

#ifdef LOGPZ
  {    
    stringstream sout;
    sout << "Entering Infinit Receive loop";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  while (LocalCM->fKeepReceiving){
    int ret = LocalCM->f_buffer.ReceiveBlocking();
    if (ret <= 0)
    {
#ifdef LOGPZ    
      {
        stringstream sout;
        sout << "LocalCM->f_buffer.ReceiveBlocking() returned <= 0\nThis is Communication problem\nFarewell";
        LOGPZ_ERROR(logger,sout.str());
      }
#endif
      LocalCM->Finish("ReceiveBlocking <receive error>");
    }
#ifdef LOGPZ    
    {
      stringstream sout;
      sout << "Data received on Buffer! Calling ProcessMessage";
      LOGPZ_DEBUG(logger,sout.str());
    }
#endif    
    LocalCM->ProcessMessage(LocalCM->f_buffer);
  }
#ifdef LOGPZ    
  {
    stringstream sout;
    sout << "Leaving ReceiveThread infinit loop " << LocalCM->f_myself;
    sout << " | Posting semaphore fReceiveSemaphore";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif    
  return NULL;
}
void * OOPMPICommManager::ReceiveMsgNonBlocking (void *t){
  OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
#ifdef DEBUG
  LOGPZ_DEBUG(logger,"ReceiveMsgBlocking \n");
#endif
  while (1){
		
    OOPMPIStorageBuffer msg;
    pthread_mutex_lock(&fCommunicate);
    int ret = msg.ReceiveBlocking();
    pthread_mutex_unlock(&fCommunicate);
    // se houver erro, Kill
    if (ret <= 0) {
      LocalCM->Finish("ReceiveBlocking <receive error>");
    }
#ifdef DEBUG
    LOGPZ_DEBUG(logger,"Calling ProcessMessage\n");
#endif
    LocalCM->ProcessMessage (msg);
  }
  return NULL;
	
}

int OOPMPICommManager::ReceiveBlocking ()
{
  if(!CM->GetProcID()){
#ifdef LOGPZ    
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << __LINE__ << "before receivemessages " << CM->GetProcID();
    LOGPZ_DEBUG(logger,sout.str());
#endif
  }
	
  f_buffer.ReceiveBlocking();
  if(f_buffer.TestReceive()) {
    ProcessMessage (f_buffer);
    f_buffer.Receive();
  } else {
    //		cout << "OOPMPICommManager::ReceiveBlocking I dont understand\n";
  }
#ifdef DEBUG
  //  sleep(1);
#endif
  if(!CM->GetProcID()){
#ifdef LOGPZ    
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << __LINE__ << "after receivemessages " << CM->GetProcID();
    LOGPZ_DEBUG(logger,sout.str());
#endif
  }

  return 1;
};
int OOPMPICommManager::ProcessMessage(OOPMPIStorageBuffer & msg)
{
  
  TPZSaveable *obj = msg.Restore ();
  if (obj == NULL) {
    Finish( "ReceiveMessages <Erro em Restore() do objeto>.\n" );
    //return 1;              
  }
  // Trace( " ClassId do objeto recebido: " );
  // Trace( obj->GetClassId() << ".\n" );
  OOPTask *task = dynamic_cast<OOPTask *> (obj);
  if(task) {
    task->Submit();
  } else {
#ifdef LOGPZ    
    std::stringstream sout;
    sout << "OOPMPICommManager::ProcessMessage received an object which is not a task";
    LOGPZ_DEBUG(logger,sout.str());
#endif
    delete obj;
  }
  return 1;
}

void OOPMPICommManager::Finish(char * msg){
  cout << msg << endl;
  cout.flush();
  f_buffer.CancelRequest();
  cout << "Processor " << f_myself  << " reached synchronization point !" << endl;
//  MPI_Barrier( MPI_COMM_WORLD );
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
    LOGPZ_DEBUG(logger,sout.str());
    cout << sout.str() << endl;
  }
#endif
  fKeepReceiving = false;
  int ret;
  int tag = 0;
  char * buff = new char[1];
  ret = MPI_Send (&buff[0], 0, MPI_PACKED, CM->GetProcID(), tag, MPI_COMM_WORLD);
#ifdef LOGPZ
  {    
    std::stringstream sout;
    sout << "Waiting for ReceiveThread sinalization on Processor " << CM->GetProcID();
    LOGPZ_DEBUG(logger,sout.str());
    cout << sout.str() << endl;
  }
  delete [] buff;
#endif
  pthread_join(fReceiveThread, NULL);
#ifdef LOGPZ
  {    
    std::stringstream sout;
    sout << "Processor " << CM->GetProcID() << " Received ReceiveThread termination confirmation";
    LOGPZ_DEBUG(logger,sout.str());
    cout << sout.str() << endl;
  }
#endif
}

#endif
