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

class   OOPMPIReceiveStorage;
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
	MPI_Init(&f_argc,&f_argv); 
}
OOPMPICommManager::~OOPMPICommManager ()
{
	MPI_Finalize ();
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
	
	if (f_myself == 0)
		return f_num_proc;
	else
		return 0;
}
int OOPMPICommManager::SendTask (OOPTask * pTask)
{
	pthread_mutex_lock(&fCommunicate);
#warning "Nao tem necessidade do mutex neste ponto"
	cout << "Sending task " << pTask->GetClassID() << endl;
	cout.flush();
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
	pTask->Pack (&f_sendbuffer);
	f_sendbuffer.Send(process_id);
	cout << "Message Sent\n";
	cout.flush();
	delete pTask;
	pthread_mutex_unlock(&fCommunicate);
	return 1;
};
int OOPMPICommManager::ReceiveMessages ()
{
/*	
	OOPMPICommManager * LocalCM = dynamic_cast<OOPMPICommManager * > (CM);
	if(!fReceiveThreadExists) {
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&fReceiveThread, NULL, LocalCM->ReceiveMsgBlocking, NULL);
		fReceiveThreadExists = true;
	}
	return 1;
*/
    f_receivebuffer.Receive();
	if(f_receivebuffer.TestReceive()) {
		ProcessMessage(f_receivebuffer);
		f_receivebuffer.Receive();
	}		
	/*OOPMPIReceiveStorage msg;
	int ret = msg.Receive ();
	// Se nao tiver mensagem, retorna.
	if (ret <= 0)
		return (ret);
	ProcessMessage (msg);
	return 1;*/
	return 1;
};
void * OOPMPICommManager::ReceiveMsgBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
	cout << "ReceiveMsgBlocking \n";
	cout.flush();
	while (1){
		
		OOPMPIReceiveStorage msg;
		pthread_mutex_lock(&fCommunicate);
		int ret = msg.ReceiveBlocking ();
		pthread_mutex_unlock(&fCommunicate);
		// se houver erro, Kill
		if (ret <= 0) {
	 		LocalCM->Finish("ReceiveBlocking <receive error>");
		}
		cout << "Calling ProcessMessage\n";
		cout.flush();
		LocalCM->ProcessMessage (msg);
	}
	return NULL;
	
}
void * OOPMPICommManager::ReceiveMsgNonBlocking (void *t){
	//OOPMPICommManager *CM=(OOPMPICommManager *)(t);
	OOPMPICommManager *LocalCM=(OOPMPICommManager *)CM;
	cout << "ReceiveMsgBlocking \n";
	cout.flush();
	while (1){
		
		OOPMPIReceiveStorage msg;
		pthread_mutex_lock(&fCommunicate);
		int ret = msg.ReceiveBlocking();
		pthread_mutex_unlock(&fCommunicate);
		// se houver erro, Kill
		if (ret <= 0) {
			LocalCM->Finish("ReceiveBlocking <receive error>");
		}
		cout << "Calling ProcessMessage\n";
		cout.flush();
		LocalCM->ProcessMessage (msg);
	}
	return NULL;
	
}

int OOPMPICommManager::ReceiveBlocking ()
{
	f_receivebuffer.ReceiveBlocking();
	if(f_receivebuffer.TestReceive()) {
		ProcessMessage (f_receivebuffer);
		f_receivebuffer.Receive();
	} else {
		cout << "OOPMPICommManager::ReceiveBlocking I dont understand\n";
	}
	return 1;
};
int OOPMPICommManager::ProcessMessage (OOPMPIReceiveStorage & msg)
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
	cout << msg;
	cout.flush();
	MPI_Finalize();
}
