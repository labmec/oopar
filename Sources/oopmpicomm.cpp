//
// Autor: Fábio Amaral de Castro, RA: 991722
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

class   OOPMPIStorageBuffer;
class   OOPMPICommManager;
using namespace std;
extern OOPTaskManager *TM;
pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;


OOPMPICommManager::OOPMPICommManager (int argc, char **argv)
{
	f_myself = -1;
	f_num_proc = 0;
	f_argc = argc;
	f_argv = argv;
	fReceiveThreadExists=false;
	// f_proc = (int *) NULL; 
	cout << "Before calling MPI_Init\n";
	cout.flush();
	MPI_Init(&f_argc,&f_argv); 
	cout << "After calling MPI_Init\n";
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
  // que é o processo que inicializa os demais.
  // Para inicializar, necessita de variaveis int argc e char **argv.
int OOPMPICommManager::Initialize (char * argv, int argc)//(int arg_c, char **arg_v)
{
	MPI_Comm_size (MPI_COMM_WORLD, &f_num_proc);
	MPI_Comm_rank (MPI_COMM_WORLD, &f_myself);
	cout << "MPIComm Initialize f_myself " << f_myself << " f_num_proc " << f_num_proc << endl;
        cout.flush();
	if (f_myself == 0)
		return f_num_proc;
	else
		return 0;
}
int OOPMPICommManager::SendTask (OOPTask * pTask)
{
	
	//pthread_mutex_lock(&fCommunicate);
#warning "Nao tem necessidade do mutex neste ponto"
#ifdef VERBOSE
	cout << "Sending task " << pTask->GetClassID() << endl;
	cout.flush();
#endif
	int process_id = pTask->GetProcID ();	// processo onde ptask deve
						// ser executada
	// Se "process_id" nao for valido.
	if (process_id >= f_num_proc) {
		Finish( "SendObject <process ID out of range>");
		delete pTask;
		return -1;
	}
	// Se estiver tentando enviar para mim mesmo.
	if (process_id == f_myself) {
		Finish( "SendObject <I cannot send to myself>");
		delete pTask;
		return -1;
	}
	pTask->Pack (&f_buffer);
	f_buffer.Send(process_id);
#ifdef VERBOSE
	cout << "Message Sent\n";
	cout.flush();
#endif
	delete pTask;
	//pthread_mutex_unlock(&fCommunicate);
	return 1;
};
int OOPMPICommManager::ReceiveMessages ()
{
	
    f_buffer.Receive();
	if(f_buffer.TestReceive()) {
		ProcessMessage(f_buffer);
		f_buffer.Receive();
	}		
	return 1;
};
void * OOPMPICommManager::ReceiveMsgBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
#ifdef VERBOSE
	cout << "ReceiveMsgBlocking \n";
	cout.flush();
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
#ifdef VERBOSE
		cout << "Calling ProcessMessage\n";
		cout.flush();
#endif
		LocalCM->ProcessMessage (msg);
	}
	return NULL;
	
}
void * OOPMPICommManager::ReceiveMsgNonBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
#ifdef VERBOSE
	cout << "ReceiveMsgBlocking \n";
	cout.flush();
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
#ifdef VERBOSE
		cout << "Calling ProcessMessage\n";
		cout.flush();
#endif
		LocalCM->ProcessMessage (msg);
}
	return NULL;
	
}

int OOPMPICommManager::ReceiveBlocking ()
{
	
	f_buffer.ReceiveBlocking();
	if(f_buffer.TestReceive()) {
		ProcessMessage (f_buffer);
		f_buffer.Receive();
	} else {
//		cout << "OOPMPICommManager::ReceiveBlocking I dont understand\n";
	}
	return 1;
};
int OOPMPICommManager::ProcessMessage (OOPMPIStorageBuffer & msg)
{
	OOPSaveable *obj = msg.Restore ();
#warning "Restore( &msg ); not implemented on OOPMPICommManager"
	if (obj == NULL) {
		Finish( "ReceiveMessages <Erro em Restore() do objeto>.\n" );
	}
	// Trace( " ClassID do objeto recebido: " );
	// Trace( obj->GetClassID() << ".\n" );
	OOPTask *task = dynamic_cast<OOPTask *> (obj);
	if(task) {
		TM->Submit (task);
	} else {
		cout << "OOPMPICommManager::ProcessMessage received an object which is not a task\n";
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
