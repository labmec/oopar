//
// Autor: F�io Amaral de Castro, RA: 991722
//
// E-mail: facastro99@yahoo.com
//
// Arquivo: com_mpi.c
//
// Classe: TMpiComManager
//
// Descr.: Classe para comunicacao utilizando MPI
//
// Versao: 12 / 2002
//
#include "mpi.h"
//#include "communic.h"
#include <stdio.h>
//#include <stdlib.h>
#include <string>
#include <pthread.h>
#include "oopmpicomm.h"
#include "ooptaskmanager.h"

#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMPICommManager"));

class   OOPMPIStorageBuffer;
class   OOPMPICommManager;
using namespace std;
extern OOPTaskManager *TM;
pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;

OOPMPICommManager::OOPMPICommManager(){
  LOG4CXX_WARN(logger, "Empty Constructor should never be called!");
}
OOPMPICommManager::OOPMPICommManager (int &argc, char **argv)
{
	f_myself = -1;
	f_num_proc = 0;
	fReceiveThreadExists=false;
	// f_proc = (int *) NULL; 
  cout << "Before calling MPI_Init\n";
	cout.flush();
	MPI_Init(&argc,&argv); 
	Initialize((char*)argv, argc);
	f_argc = argc;
	f_argv = argv;
  cout << "After calling MPI_Init";
	cout.flush();
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
	
	//pthread_mutex_lock(&fCommunicate);
//#warning "Nao tem necessidade do mutex neste ponto"
#ifdef DEBUGALL
  {
    stringstream sout;
    sout <<  __PRETTY_FUNCTION__ << " Sending task " << pTask->ClassId() << 
           " to proc " << pTask->GetProcID ();
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
	int process_id = pTask->GetProcID ();	// processo onde ptask deve
						// ser executada
	// Se "process_id" nao for valido.
	if (process_id >= f_num_proc) {
		Finish( "SendObject <process ID out of range>");
    LOG4CXX_WARN(logger,"SendObject <process ID out of range>");
		delete pTask;
		return -1;
	}
	// Se estiver tentando enviar para mim mesmo.
	if (process_id == f_myself) {
		Finish( "SendObject <I cannot send to myself>");
    LOG4CXX_WARN(logger,"SendObject <I cannot send to myself>");
		delete pTask;
		return -1;
	}
	//Attention here
#ifdef DEBUGALL
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ <<" Packing the task in a buffer";
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
	pTask->Write (f_buffer, 1);
#ifdef DEBUGALL
  {
    stringstream sout;
    sout <<  __PRETTY_FUNCTION__ << " Sending the buffer";
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
	f_buffer.Send(process_id);
#ifdef DEBUGALL
  {
    stringstream sout;
    sout <<  __PRETTY_FUNCTION__ << " Message Sent";
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
	delete pTask;
	//pthread_mutex_unlock(&fCommunicate);
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
void * OOPMPICommManager::ReceiveMsgBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
#ifdef DEBUG
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << "ReceiveMsgBlocking ";
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
	while (1){
		
		OOPMPIStorageBuffer msg;
		pthread_mutex_lock(&fCommunicate);
		int ret = msg.ReceiveBlocking ();
		pthread_mutex_unlock(&fCommunicate);
		// se houver erro, Kill
		if (ret <= 0) {
	 		LocalCM->Finish("ReceiveBlocking <receive error>");
		}
#ifdef DEBUG
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << "Calling ProcessMessage";
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
		LocalCM->ProcessMessage (msg);
	}
	return NULL;
	
}
void * OOPMPICommManager::ReceiveMsgNonBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
#ifdef DEBUG
  LOG4CXX_DEBUG(logger,"ReceiveMsgBlocking \n");
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
    LOG4CXX_DEBUG(logger,"Calling ProcessMessage\n");
#endif
		LocalCM->ProcessMessage (msg);
}
	return NULL;
	
}

int OOPMPICommManager::ReceiveBlocking ()
{
  if(!CM->GetProcID()){
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << __LINE__ << "before receivemessages " << CM->GetProcID();
    LOG4CXX_DEBUG(logger,sout.str());
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
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << __LINE__ << "after receivemessages " << CM->GetProcID();
    LOG4CXX_DEBUG(logger,sout.str());
  }

	return 1;
};
int OOPMPICommManager::ProcessMessage (OOPMPIStorageBuffer & msg)
{
	TPZSaveable *obj = msg.Restore ();
	if (obj == NULL) {
		Finish( "ReceiveMessages <Erro em Restore() do objeto>.\n" );
	}
	// Trace( " ClassId do objeto recebido: " );
	// Trace( obj->GetClassId() << ".\n" );
	OOPTask *task = dynamic_cast<OOPTask *> (obj);
	if(task) {
		task->Submit();
	} else {
    std::stringstream sout;
		sout << "OOPMPICommManager::ProcessMessage received an object which is not a task";
    LOG4CXX_DEBUG(logger,sout.str());
		delete obj;
	}
	return 1;
}
void OOPMPICommManager::Finish(char * msg){
	cout << msg << endl;
	cout.flush();
	f_buffer.FreeRequest();
	MPI_Finalize();
}
