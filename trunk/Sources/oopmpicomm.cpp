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
class   OOPMPIReceiveStorage;
class   OOPMPICommManager;
using namespace std;
extern OOPTaskManager *TM;


OOPMPICommManager::OOPMPICommManager (int argc, char **argv)
{
	f_buffer = (POOPMPISendStorage *) NULL;
	f_myself = -1;
	f_num_proc = 0;
	f_argc = argc;
	f_argv = argv;
	fReceiveThreadExists=false;
	
	// f_proc = (int *) NULL; 
	//MPI_Init(&argc,&argv); 
}
OOPMPICommManager::~OOPMPICommManager ()
{
	for (int i = 0; i < f_num_proc; i++)
		if (f_buffer[i])
			delete (f_buffer[i]);
	delete[]f_buffer;
	MPI_Finalize ();
}
 // Diferente de PVM, o argumento Destino em mpi nao eh o endereco absoluto
 // do 
  // destino, mas o relativo. ou seja, o comando MPI_INIT inicializa os
  // processos e para cada um destina um numero inteiro diferente, a partir
  // de 0,
  // que é o processo que inicializa os demais.
  // Para inicializar, necessita de variaveis int argc e char **argv.
int OOPMPICommManager::Initialize (char *process_name, int num_of_process)
{
	// cout << "process_name:" << argv <<endl;;
	f_num_proc = num_of_process;
	MPI_Init (&f_argc, &f_argv);
	//MPI_Comm_size (MPI_COMM_WORLD, &f_num_proc);
	MPI_Comm_size (MPI_COMM_WORLD, &num_of_process);
	MPI_Comm_rank (MPI_COMM_WORLD, &f_myself);
	f_buffer = new (POOPMPISendStorage[f_num_proc]);
	if (f_buffer == NULL) {
#warning "//Finish( "Initialize <Error allocating sending buffers>" );"
#warning "Finish not defined ??? No idea !"
	}
	for (int i = 0; i < f_num_proc; i++) {
		if (i == f_myself) {
			f_buffer[i] = (OOPMPISendStorage *) NULL;
		}
		else {
			f_buffer[i] = new OOPMPISendStorage (i);	// inicializador 
									// nao-padrao
			// if ( f_buffer[i] == NULL ) Finish( "Initialize
			// <Error making sendings buffers>");
		}
	}
	if (f_myself == 0)
		return f_num_proc;
	else
		return 0;
}
int OOPMPICommManager::SendTask (OOPTask * pTask)
{
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
	}
	// Se 'process_id' < 0, faz um multi cast.
	// se process_id < 0, first = 0. se > 0, first = process_id.
	int first = (process_id < 0 ? 0 : process_id);
	// se process_id < 0, last = numero de processos. se > 0, last =
	// process_id + 1.
	int last = (process_id < 0 ? f_num_proc : process_id + 1);	// 
	char has_an_object = 1;
	for (int i = first; i < last; i++)
		if (i != f_myself) {
			// Empacota o objeto no buffer destino.
			pTask->SetProcID (i);	// muda destino para
						// processador i 
			OOPMPISendStorage *buf = f_buffer[i];
			// buf->Activate(); // soh no pvm
			buf->PkByte (&has_an_object);	// empacota primeiro
							// valor = int = 1
			pTask->Pack (buf);
		}
	// Restaura o ID do processador destino na 'task'.
	pTask->SetProcID (process_id);
	return 1;
};
int OOPMPICommManager::ReceiveMessages ()
{
	
	OOPMPICommManager * LocalCM = dynamic_cast<OOPMPICommManager * > (CM);
	if(!fReceiveThreadExists) {
/*		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);*/
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
	while (1){
		
		OOPMPIReceiveStorage msg;
		int ret = msg.ReceiveBlocking ();
		// se houver erro, Kill
		if (ret <= 0) {
	#warning "Finish("ReceiveBlocking <receive error>");\n";
		}
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
int OOPMPICommManager::SendMessages ()
{
	OOPMPISendStorage *p;
	char end_of_message = 0;
	int length = 0;
	for (int i = 0; i < f_num_proc; i++) {
		p = f_buffer[i];
		if (p != NULL && p->Length () > 0) {
			p->PkByte (&end_of_message);	// coloca int = 0 no
							// final da mensagem
			if (p->Send (1) < 1) {	// msg_id=1,
						// target=TSendStorage::f_target_tid 
						// 
#warning "Finish ("SendMessages <send error>" );"
			}
		}
	}
	return (1);
};
int OOPMPICommManager::ProcessMessage (OOPMPIReceiveStorage & msg)
{
	// Trace("Recebendo uma mensagem do processador ");
	// Trace( FindID(msg.GetSender())<<"\n");
	int count;
	char has_more_objects = 0;
	msg.UpkByte (&has_more_objects);
	for (count = 0; has_more_objects; count++) {
		OOPSaveable *obj = msg.Restore ();;
#warning "Restore( &msg ); not implemented on OOPMPICommManager"
		if (obj == NULL) {
#warning "Finish( "ReceiveMessages <Erro em Restore() do objeto>.\n" );"
		}
		// Trace( " ClassID do objeto recebido: " );
		// Trace( obj->GetClassID() << ".\n" );
		TM->Submit ((OOPTask *) obj);
		msg.UpkByte (&has_more_objects);
	}
	return 1;
}
