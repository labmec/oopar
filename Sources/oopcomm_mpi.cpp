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
#include "oopcommmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oopcomm_mpi.h"

OOPMPICommManager::OOPMPICommManager ()
{
	f_buffer = (OOPMPISendStorage *) NULL;
	f_myself = -1;
	int     f_num_proc = 0;
	// f_proc = (int *) NULL; 
	// MPI_Init(&argc,&argv);
}

OOPMPICommManager::~OOPMPICommManager ()
{
	// delete( f_proc );
	delete (f_buffer[f_num_proc]);
	MPI_Finalize ();
}

int OOPMPICommManager::Initialize (
	char *process_name,
	int num_of_process)
{
	// cout << "process_name:" << argv <<endl;;
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &f_num_proc);
	MPI_Comm_rank (MPI_COMM_WORLD, &f_myself);

	f_buffer = new (OOPMPISendStorage[n_num_proc]);
	if (f_buffer == NULL)
		Error (1, "Initialize <Error allocating sending buffers>");
	for (int i = 0; i < f_num_proc; i++)
	{
		if (i == f_myself)
		{
			f_buffer[i] = (OOPMPISendStorage *) NULL;
		}
		else
		{
			f_buffer[i] = new OOPMPISendStorage (i);	// meio 
									// esquisito....
			if (f_buffer[i] == NULL)
				Error (1,
				       "Initialize <Error making sendings buffers>");
		}
	}

	// Nomeia o arquivo de saida para a geracao de erros.
	char er_file[64];
	sprintf (er_file, "error.%d", f_myself);
	GSetErrorFile (er_file);

	if (f_myself == 0)
		return &f_num_proc;
	else
		return 0;
}

int OOPMPICommManager::SendTask (
	TTask * pTask)
{
	int process_id = pTask->GetProcID ();	// processo onde ptask deve
						// ser executada
	// Se "process_id" nao for valido.
	if (process_id >= f_num_proc)
		Error (1, "SendObject <process ID out of range>");
	// Se estiver tentando enviar para mim mesmo.
	if (f_proc[process_id] == f_myself)
		Error (1, "SendObject <I cannot send to myself>");

	// Se 'process_id' < 0, faz um multi cast.
	int first = (process_id < 0 ? 0 : process_id);
	int last = (process_id < 0 ? f_num_proc : process_id + 1);

	char has_an_object = 1;
	for (int i = first; i < last; i++)
		if (i != f_myself)
		{
			// Empacota o objeto no buffer destino.
			pTask->SetProcID (i);	// muda destino para
						// processador i 
			OOPMPISendStorage *buf = f_buffer[i];	// onde q se
								// definiu
								// f_buffer?
			buf->Activate ();	// ****de onde q saiu isso?
			buf->PkByte (&has_an_object);	// char has_an_objct
							// =1. e dai?
			pTask->Pack (buf);
		}

	// Restaura o ID do processador destino na 'task'.
	pTask->SetProcID (process_id);

	return 1;

};

int OOPMPICommManager::ReceiveMessages (
	)
{
	OOPMPIReceiveStorage msg;
	int ret = msg.Receive ();	// ***Receive, 2 argumentos
	// Se nao tiver mensagem, retorna.
	if (ret <= 0)
		return (ret);
	int count;

	char has_more_objects = 0;
	msg.UpkByte (&has_more_objects);	// UpkByte eh do tipo int,
						// argumento deve ser um
						// *char
	for (count = 0; has_more_objects; count++)
	{
		OOPSaveable *new_object = msg.Restore ();
		if (new_object == NULL)
			// Error( 1, "ReceiveMessages <Erro em Restore() do
			// objeto>.\n" );
			TM->Submit ((OOPTask *) new_object);
		msg.UpkByte (&has_more_objects);
	}
	return (1);
};

int OOPMPICommManager::SendMessages (
	)
{
	OOPMPISendStorage *p;
	char end_of_message = 0;
	int length = 0;
	for (int i = 0; i < f_num_proc; i++)
	{
		p = f_buffer[i];
		if (p != NULL && p->Length () > 0)
		{
			// p->Activate(); ****De onde q saiu isso?
			p->PkByte (&end_of_message);	// end_of_message =
							// 0, e dai?
			if (p->Send (1) != 0)	// *****Send usa 2 argumentos
				cerr <<  "SendMessages <send error>";
		}
	}
	return (1);
};

//Retorna 0 se o processo não tiver sido disparado através de Initialize()
int OOPMPICommManager::IAmTheMaster (
	)
{
return (f_num_proc > 0)};	// ****parece errado na versao pvm

char OOPMPICommManager::*ClassName (
	)
{
	return ("OOPMPICommManager::");
}
