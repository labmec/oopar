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
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMPICommManager"));
#endif

class   OOPMPIStorageBuffer;
class   OOPMPICommManager;
using namespace std;
extern OOPTaskManager *TM;
pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;

OOPMPICommManager::OOPMPICommManager(){
 	LOGPZ_WARN(logger, "Empty Constructor should never be called!");
}
OOPMPICommManager::OOPMPICommManager (int &argc, char **argv)
{
	f_myself = -1;
	f_num_proc = 0;
	fReceiveThreadExists=false;
	// f_proc = (int *) NULL; 
  	cout << "Initializing MPI !\n Calling MPI_Init\n";
	cout.flush();
	MPI_Init(&argc,&argv); 
	Initialize((char*)argv, argc);
#	ifdef OOP_MPE
	//MPE_Init_log();
	//MPE_Describe_state( 1, 2, "Running", "yellow" );
	//MPE_Describe_state( 3, 4, "Idle", "yellow" );
#	endif	
	
	f_argc = argc;
	f_argv = argv;
  	cout << "MPI_Init Called\n";
	cout.flush();
	fReceiveThread = 0;
        pthread_mutex_init(&fReceiveMutex, NULL);       
        pthread_cond_init(&fReceiveCond, NULL);       
        sem_init(&fReceiveSemaphore, 0, 0);
        fKeepReceiving = true;
        
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
	cout << "MPIComm Initialize f_myself " << f_myself << " f_num_proc " << f_num_proc << std::endl;
	if (f_myself == 0)
		return f_num_proc;
	else
		return 0;
}
int OOPMPICommManager::SendTask (OOPTask * pTask)
{
	

#	ifdef DEBUGALL
	{
#		ifdef LOGPZ    
		stringstream sout;
		sout <<  __PRETTY_FUNCTION__ << " Sending task " << pTask->ClassId() << 
			" to proc " << pTask->GetProcID ();
		LOGPZ_DEBUG(logger,sout.str());
#		endif    
	}
#	endif
	int process_id = pTask->GetProcID ();	// processo onde ptask deve
						// ser executada
	// Se "process_id" nao for valido.
	if (process_id >= f_num_proc) {
		Finish( "SendObject <process ID out of range>");
		LOGPZ_WARN(logger,"SendObject <process ID out of range>");
		delete pTask;
		return -1;
	}
	// Se estiver tentando enviar para mim mesmo.
	if (process_id == f_myself) {
		Finish( "SendObject <I cannot send to myself>");
    		LOGPZ_WARN(logger,"SendObject <I cannot send to myself>");
		delete pTask;
		return -1;
	}
	//Attention here
#	ifdef DEBUGALL
	{
#		ifdef LOGPZ
		stringstream sout;
		sout << __PRETTY_FUNCTION__ <<" Packing the task in a buffer";
		LOGPZ_DEBUG(logger,sout.str());
#		endif
  	}
#	endif
	pTask->Write (f_buffer, 1);
#	ifdef DEBUGALL
 	{
#		ifdef LOGPZ    
		stringstream sout;
		sout <<  __PRETTY_FUNCTION__ << " Sending the buffer";
		LOGPZ_DEBUG(logger,sout.str());
#		endif    
  	}
#	endif
	f_buffer.Send(process_id);
#	ifdef DEBUGALL
  	{
#		ifdef LOGPZ
		stringstream sout;
		sout <<  __PRETTY_FUNCTION__ << " Message Sent";
		LOGPZ_DEBUG(logger,sout.str());
#		endif
  	}
#	endif
	delete pTask;
	return 1;
};
int OOPMPICommManager::ReceiveMessages ()
{
//    if(!CM->GetProcID()) cout << __PRETTY_FUNCTION__ << __LINE__ << "before receivemessages " << CM->GetProcID() <<   "\n";

	f_buffer.Receive();
	while(f_buffer.TestReceive()) {
		  ProcessMessage(f_buffer);
		  f_buffer.Receive();
    	}
//  if(!CM->GetProcID()) cout << __PRETTY_FUNCTION__ << __LINE__ << "after receivemessages " << CM->GetProcID() <<   "\n";
	return 1;
};
int OOPMPICommManager::ReceiveMessagesBlocking()
{
  pthread_create(&fReceiveThread, NULL, ReceiveMsgBlocking, this);
}
void * OOPMPICommManager::ReceiveMsgBlocking (void *t){
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
        //pthread_mutex_lock(&LocalCM->fReceiveMutex);
#	ifdef DEBUG
  	{
#		ifdef LOGPZ    
    		stringstream sout;
    		sout << __PRETTY_FUNCTION__ << "ReceiveMsgBlocking ";
    		LOGPZ_DEBUG(logger,sout.str());
#		endif
	} 
#	endif

        while (LocalCM->fKeepReceiving){
		int ret = LocalCM->f_buffer.ReceiveBlocking();
		// se houver erro, Kill
		if (ret <= 0) {
/*                  cout << "--------------" << __PRETTY_FUNCTION__ << " " << __LINE__ << endl;
                  cout.flush();*/
	 		LocalCM->Finish("ReceiveBlocking <receive error>");
		}
#		ifdef DEBUG
		{
#			ifdef LOGPZ    
			stringstream sout;
			sout << __PRETTY_FUNCTION__ << "Calling ProcessMessage";
			LOGPZ_DEBUG(logger,sout.str());
#			endif    
  		}
#		endif
		LocalCM->ProcessMessage (LocalCM->f_buffer);
                
	}
        cout << "Leaving ReceiveThread infinit loop " << LocalCM->f_myself << endl;

        sem_post(&LocalCM->fReceiveSemaphore);
        //sleep(3);
        //pthread_cond_signal(&LocalCM->fReceiveCond);
        //pthread_mutex_unlock(&LocalCM->fReceiveMutex);
        cout.flush();
	return NULL;
}
void * OOPMPICommManager::ReceiveMsgNonBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
#	ifdef DEBUG
  	LOGPZ_DEBUG(logger,"ReceiveMsgBlocking \n");
#	endif
	while (1){
		
		OOPMPIStorageBuffer msg;
		pthread_mutex_lock(&fCommunicate);
		int ret = msg.ReceiveBlocking();
		pthread_mutex_unlock(&fCommunicate);
		// se houver erro, Kill
		if (ret <= 0) {
			LocalCM->Finish("ReceiveBlocking <receive error>");
		}
#		ifdef DEBUG
    		LOGPZ_DEBUG(logger,"Calling ProcessMessage\n");
#		endif
		LocalCM->ProcessMessage (msg);
	}
	return NULL;
	
}

int OOPMPICommManager::ReceiveBlocking ()
{
	if(!CM->GetProcID()){
#		ifdef LOGPZ    
		stringstream sout;
		sout << __PRETTY_FUNCTION__ << __LINE__ << "before receivemessages " << CM->GetProcID();
		LOGPZ_DEBUG(logger,sout.str());
#		endif
  	}
	
	f_buffer.ReceiveBlocking();
	if(f_buffer.TestReceive()) {
		ProcessMessage (f_buffer);
		f_buffer.Receive();
	} else {
//		cout << "OOPMPICommManager::ReceiveBlocking I dont understand\n";
	}
#	ifdef DEBUG
//  sleep(1);
#	endif
	if(!CM->GetProcID()){
#		ifdef LOGPZ    
		stringstream sout;
		sout << __PRETTY_FUNCTION__ << __LINE__ << "after receivemessages " << CM->GetProcID();
		LOGPZ_DEBUG(logger,sout.str());
#		endif
	}

	return 1;
};
int OOPMPICommManager::ProcessMessage (OOPMPIStorageBuffer & msg)
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
#		ifdef LOGPZ    
		std::stringstream sout;
		sout << "OOPMPICommManager::ProcessMessage received an object which is not a task";
		LOGPZ_DEBUG(logger,sout.str());
#		endif
		delete obj;
	}
	return 1;
}

void OOPMPICommManager::Finish(char * msg){
	cout << msg << endl;
	cout.flush();
	f_buffer.CancelRequest();
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

void OOPMPICommManager::UnlockReceiveBlocking(){
  fKeepReceiving = false;
  MPI_Barrier( MPI_COMM_WORLD );
  cout << " ProcID " << CM->GetProcID() << " Synchronizing Termination Requests !" << endl;
  if (CM->GetProcID()==0){
    OOPTerminationTask * tt = new OOPTerminationTask(f_num_proc - 1);
    CM->SendTask(tt);
  }else{
  
    OOPTerminationTask * tt = new OOPTerminationTask(CM->GetProcID()-1);
    CM->SendTask(tt);
  }
  sem_wait(&((OOPMPICommManager *)CM)->fReceiveSemaphore);
  //pthread_mutex_lock(&((OOPMPICommManager *)CM)->fReceiveMutex);
  cout << " ProcID " << CM->GetProcID() << " Waiting for ReceiveThread sinalization ..." << endl;
  //pthread_cond_wait(&((OOPMPICommManager *)CM)->fReceiveCond,&((OOPMPICommManager *)CM)->fReceiveMutex);
  cout << " ProcID " << CM->GetProcID() << " Got it !" << endl;
  
}

#endif