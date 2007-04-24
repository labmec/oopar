#ifdef FILE_COMM
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// E-mail:  mauroesm@dcc.unicamp.br
//
// Arquivo: com_file.cc
//
// Classe:  TFileComManager
//
// Descr.:  Classe para comunicacao atraves de arquivos.
//
// Versao:  09 / 08 / 95.
//
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ooptaskmanager.h"
#include "oopsaveable.h"
#include "oopfilecomm.h"
#include "ooperror.h"
#include "ooppartask.h"
#include <errno.h>
using namespace std;
class   OOPTask;
OOPError Err;

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPFileComManager"));
#endif

/************************** Public *************************/
/*******************/
/*** Constructor ***/
OOPFileComManager::OOPFileComManager ()
{
}
OOPFileComManager::OOPFileComManager (char *prefix, int num_proc, int myID)
{
	// Inicializa variaveis.
	Err.GSetErrorFile("error.dat");
	Err.SetErrorFile("errorlocal.dat");
	// ATENCAO!!!! f_num_proc pode ser zero e causar excecao aritmetica!!!
	// Thiago - 2003.09.25
	f_num_proc = num_proc % 100;
	f_myself = myID % f_num_proc;
	strcpy (f_prefix, prefix);
	sprintf (f_my_prefix, "%s.%1d", f_prefix, f_myself);
	// Cria novos buffers.
	f_buffer = new (PTFileStorageBuffer[f_num_proc]);
	if (f_buffer == NULL) {
    LOGPZ_ERROR(logger, "Constructor <can't alloc buffers>\n");
		Err.Error (1, "Constructor <can't alloc buffers>\n");
	}
	// Inicializa os novos buffers.
	for (int i = 0; i < f_num_proc; i++) {
		if (i != f_myself)
			f_buffer[i] =
				new OOPFileStorageBuffer (f_my_prefix,
							f_myself);
		else
			f_buffer[i] = NULL;
	}
}
/******************/
/*** Destructor ***/
OOPFileComManager::~OOPFileComManager ()
{
	if (f_buffer != NULL) {
		for (int i = 0; i < f_num_proc; i++)
			if (f_buffer[i] != NULL)
				delete (f_buffer[i]);
		delete[]f_buffer;
	}
}
/*******************/
/*** Send Object ***/
int OOPFileComManager::SendTaskVrt (OOPTask * pObject)
{
	int process_id;
	process_id = pObject->GetProcID ();
	// Se "process_id" nao for valido.
	if ((process_id < -1) || (process_id >= f_num_proc)) {
		Err.Error (1, "SendObject <process ID out of range>\n");
    LOGPZ_ERROR(logger, "SendObject <process ID out of range>\n");
		return 0;
	}
	// Se estiver tentando enviar para mim mesmo.
	if (process_id == f_myself) {
		Err.Error (1, "SendObject <I canot send to myself>\n");
    LOGPZ_ERROR(logger, "SendObject <I canot send to myself>\n");
		return 0;
	}
	int iprmin = process_id;
	int iprmax = process_id + 1;
	if (process_id == -1) {
		iprmin = 0;
		iprmax = f_num_proc;
	}
	int ipr;
	for (ipr = iprmin; ipr < iprmax; ipr++) {
		if (ipr == f_myself)
			continue;
		process_id = ipr;
		pObject->SetProcID (process_id);
		// Empacota o objeto no buffer destino.
		OOPFileStorageBuffer *buf = f_buffer[process_id];
		char name[256];
		buf->FileName (name);
		buf->Open ();
		char has_an_object = 1;
		buf->PkByte (&has_an_object);
		pObject->Pack (buf);
		buf->Close ();
	}
	return (1);
}
int OOPFileComManager::SendTask (OOPTask * pObject)
{
	int process_id;
	process_id = pObject->GetProcID ();
	// Se "process_id" nao for valido.
	if ((process_id < -1) || (process_id >= f_num_proc)) {
		Err.Error (1, "SendObject <process ID out of range>\n");
    LOGPZ_ERROR(logger, "SendObject <process ID out of range>\n");
		return 0;
	}
	// Se estiver tentando enviar para mim mesmo.
	if (process_id == f_myself) {
		Err.Error (1, "SendObject <I canot send to myself>\n");
    LOGPZ_ERROR(logger, "SendObject <I canot send to myself>\n");
		return 0;
	}
	int iprmin = process_id;
	int iprmax = process_id + 1;
	if (process_id == -1) {
		iprmin = 0;
		iprmax = f_num_proc;
	}
	int ipr;
	//Alguma coisa estranha aqui !!!!
	for (ipr = iprmin; ipr < iprmax; ipr++) {
		if (ipr == f_myself)
			continue;
		process_id = ipr;
		pObject->SetProcID (process_id);
		// Empacota o objeto no buffer destino.
		OOPFileStorageBuffer *buf = f_buffer[process_id];
		char name[256];
		buf->FileName (name);
		buf->Open ();
		char has_an_object = 1;
		buf->PkByte (&has_an_object);
		pObject->Pack (buf);
		buf->Close ();
	}
	return (1);
}
/************************/
/*** Receive Messages ***/
int OOPFileComManager::ReceiveMessages ()
{
	// Monta o nome do arquivo de recepcao dos dados.
	TM->SetKeepGoing(false);
	char rcv_file[FILE_NAME_SIZE];
	sprintf (rcv_file, "%s00", f_my_prefix);
	// Verifica se o arquivo de recepcao existe.
	FILE   *recv;
	if ((recv = fopen (rcv_file, "r")) == NULL) {
		// Como o arquivo de recepcao nao existe, Cria-o.
		if ((recv = fopen (rcv_file, "w")) == NULL) {
			Err.Error (1,
				   "ReceiveMessages <error open receive file>\n");
     LOGPZ_ERROR(logger, "ReceiveMessages <error open receive file>\n");
		}
		else {
			fclose (recv);
			return (0);
		}
	}
	// Enquanto houver arquivos de mensagens...
	char msg_file_name[FILE_NAME_SIZE];
	int leu_msg;
	leu_msg = 0;
	while (fgets (msg_file_name, FILE_NAME_SIZE, recv) != NULL) {
		// Enquanto houver arquivos de mensagens...
		// Ajusta nome lido.
		int last = strlen (msg_file_name) - 1;
		if (msg_file_name[last] == '\n')
			msg_file_name[last] = '\0';
		// Le os dados de cada arquivo indicado no arquivo
		// de recepcao de dados.
		int check;
		check = 1;
		OOPFileStorageBuffer msg (msg_file_name);
		char has_more_objects;
		has_more_objects = 0;
		msg.UpkByte (&has_more_objects);
		while (has_more_objects) {
			// Restore para o corrente objeto n� est�definido
			// Era alguma coisa global ?
			// N� sei.
			OOPSaveable *new_object = 0;
			new_object = msg.Restore ();
			if (new_object == NULL) {
				Err.Error (1,
					   "ReceiveMessages <Erro em Restore() do objeto>.\n");
        LOGPZ_ERROR(logger, "ReceiveMessages <Erro em Restore() do objeto>.\n");
			}
			//TM->ReSubmit ((OOPTask *) new_object);
			TM->Submit((OOPTask *) new_object);
			msg.UpkByte (&has_more_objects);
		}
		leu_msg = 1;
	}
	// Fecha e ZERA o arquivo de recepcao de dados.
	fclose (recv);
	if ((recv = fopen (rcv_file, "w")) == NULL) {
		Err.Error (1,
			   "ReceiveMessages <error truncating receive file>\n");
    LOGPZ_ERROR(logger, "ReceiveMessages <error truncating receive file>\n");
	}
	fclose (recv);
	return (leu_msg);
}
/*********************/
/*** Send Messages ***/
int OOPFileComManager::SendMessages ()
{
	for (int i = 0; i < f_num_proc; i++) {
		char file_to_send[FILE_NAME_SIZE];
		// Nesta chamada o buffer finaliza o arquivo a ser enviado,
		// coloca seu nome em 'file_to_send' e procura um outro
		// arquivo para usar como buffer.
		// 
		if ((f_buffer[i] != NULL) &&
		    f_buffer[i]->HasSomeThingToSend (file_to_send)) {
			// Escreve o nome do arquivo que contem os dados a
			// serem enviados, no arquivo de recepcao de dados
			// do destinatario.
			// 
			FILE   *dest;
			char dst_file[FILE_NAME_SIZE];
			sprintf (dst_file, "%s.%1d00", f_prefix, i);
			if ((dest = fopen (dst_file, "a")) == NULL) {
				Err.Error (1,
					   "SendMessages <error open dest file>\n");
        LOGPZ_ERROR(logger, "SendMessages <error open dest file>\n");
			}
			fprintf (dest, "%s\n", file_to_send);
			fclose (dest);
		}
	}
	return (1);
}
/************************** Private *************************/
#endif //FILE_COMM