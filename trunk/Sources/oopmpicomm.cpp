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
	// cout << "process_name:" << argv <<endl;;
	//f_num_proc = num_of_process;
	//int local_num_procs=num_of_process;
	/*int i;
	f_argc=5;
	char ** largv = new char*[5];
	//char ** largv;
	
	vector<string> srgv;
	srgv[0]
	largv[0]=(char *)calloc(sizeof(f_argv[0]),1);
	strcpy(largv[0],f_argv[0]);
	largv[1]=(char*)calloc(10,1);
	sprintf(largv[1],"-p4pg\0");
	pid_t pid = getpid();
	largv[2] = (char*)calloc(10,1);
	sprintf(largv[2], "PI%d\0", pid);
	
	largv[3]=(char *)calloc(10,1);
	sprintf(largv[3],"-p4wd\0");
	largv[4]=(char *)calloc(sizeof("/home/pos/longhin/Projetos/OOPar/Projects/Flux\0"),1);
	strcpy(largv[4], "/home/pos/longhin/Projetos/OOPar/Projects/Flux\0");
	
	
	for(i=0;i<f_argc;i++){
		cout << largv[i] << endl;
	}
	
&Argv 0 = /home/ic/facastro/exemplo_comm
&Argv 1 = -p4pg
&Argv 2 = /home/pos/longhin/PI3180
&Argv 3 = -p4wd
&Argv 4 = /home/ic/facastro*/
	
	int i;
	MPI_Comm_size (MPI_COMM_WORLD, &f_num_proc);
	//MPI_Comm_size (MPI_COMM_WORLD, &local_num_procs);
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
	int process_id = pTask->GetProcID ();	// processo onde ptask deve
						// ser executada
	// Se "process_id" nao for valido.
	if (process_id >= f_num_proc) {
#warning "//Finish( "Initialize <Error allocating sending buffers>" );"	// Finish( 
									// "SendObject 
									// <process 
									// ID 
									// out 
									// of 
									// range>" 
									// );
		delete pTask;
		return -1;
	}
	// Se estiver tentando enviar para mim mesmo.
	if (process_id == f_myself) {
#warning "//Finish( "Initialize <Error allocating sending buffers>" );"	// Finish( 
									// "SendObject 
									// <I 
									// cannot 
									// send 
									// to 
									// myself>" 
									// );
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
	
	OOPMPICommManager * LocalCM = dynamic_cast<OOPMPICommManager * > (CM);
	if(!fReceiveThreadExists) {
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&fReceiveThread, NULL, LocalCM->ReceiveMsgBlocking, NULL);
		fReceiveThreadExists = true;
	}
	return 1;
	
	/*OOPMPIReceiveStorage msg;
	int ret = msg.Receive ();
	// Se nao tiver mensagem, retorna.
	if (ret <= 0)
		return (ret);
	ProcessMessage (msg);
	return 1;*/
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
	#warning "Finish("ReceiveBlocking <receive error>");\n";
			cout << "ReceiveBlocking <receive error\n";
			cout.flush();
			exit (-1);
		}
		cout << "Calling ProcessMessage\n";
		cout.flush();
		LocalCM->ProcessMessage (msg);
	}
	return NULL;
}
int OOPMPICommManager::ReceiveBlocking ()
{
	OOPMPIReceiveStorage msg;
	
	int ret = msg.ReceiveBlocking ();
	
	// se houver erro, Kill
	if (ret <= 0) {
#warning "Finish("ReceiveBlocking <receive error>");\n";
	}
	ProcessMessage (msg);
	return 1;
};
int OOPMPICommManager::ProcessMessage (OOPMPIReceiveStorage & msg)
{
	// Trace("Recebendo uma mensagem do processador ");
	// Trace( FindID(msg.GetSender())<<"\n");
	OOPSaveable *obj = msg.Restore ();
#warning "Restore( &msg ); not implemented on OOPMPICommManager"
	if (obj == NULL) {
#warning "Finish( "ReceiveMessages <Erro em Restore() do objeto>.\n" );"
			cout << "ReceiveMessages <Erro em Restore() do objeto>.\n"; 
			cout.flush();
			exit (-1);
	}
	// Trace( " ClassID do objeto recebido: " );
	// Trace( obj->GetClassID() << ".\n" );
	TM->Submit ((OOPTask *) obj);
	return 1;
}
