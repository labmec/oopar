// -*- c++ -*-
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// E-mail:  mauroesm@dcc.unicamp.br
//
// Arquivo: communic.hh
//
// Classe:  TCommunicationManager
//
// Descr.:  Classe base para a comunicacao entre processos.
//
// Versao:  07 / 08 / 95.
//
#ifndef _COMMUNICHH_
#define _COMMUNICHH_
#include "ooperror.h"

#include "oopobjectid.h"
#include "ooperror.h"
#include "oopobjectid.h"
//class OOPTaskManager;
//class OOPSaveable;
class   OOPTask;
/**
 * Implements the communication manager for the oopar environment
 * Defines interface for any communication library used as communication layer
 * One instance of TCommunicationManager must exist in all nodes of the
 * environment.
 * Acts as daemon in all nodes on the environment.
 */
class   OOPCommunicationManager 
{
      public:
	/**
	* Simple constructor
	*/
	OOPCommunicationManager ()
	{
		f_num_proc = f_myself = 0;
	}
	/**
	* Destructor
	*/
	virtual ~ OOPCommunicationManager ()
	{
	}
	/**
	* If this process was created by the user, all remaining processes are
	* intialized and !=0 is returned. If this process was created by another
	* process, it is initialized (only process_name and num_of_process are
	* ignored) and zero is returned.
	* This method must be called before any communication activity
	*/
	virtual int Initialize (char *process_name, int num_of_process)
	{
		return 0;
	}
	/**
	* Returns the processor number in which this object is running
	*/
	int     GetProcID ()
	{
		return f_myself;
	}
	/**
	* Returns the total number of processors
	*/
	int     NumProcessors ()
	{
		return f_num_proc;
	}
	// Armazena "*pObject" no buffer enderecado para o processo
	// "processID". O buffer so' e' enviado atraves de "SendMessage".
	// Como "TSavable" e' uma classe base para varias outras,
	// "SendObject"
	// pode fazer um tratamento diferenciado de acordo com o objeto a ser
	// enviado. Ex.: Passar todas as TTasks na frente.
	// 
	/**
	* Stores a pointer to the object TTask in the buffer reserved to the process
	* identified by processID. The buffer is sent through SendMessage.
	* @param *pTask : A pointer to the object TTask which will be sent
	*/
	virtual int SendTask (OOPTask * pTask) = 0;
	// Recebe uma mensagem qualquer que tenha chegado. Se nao huver
	// nenhuma mensagem, retorna 0.
	// 
	/**
	* Receives any arrived messages.
	* Returns zero if none has arrived
	*/
	virtual int ReceiveMessages () = 0;
	// Envia todas as mensagens de todos os buffers.
	// 
	/**
	* Sends all messages in all buffers
	*/
	virtual int SendMessages () = 0;
	// Retorna 0 se este processo nao tiver sido disparado pelo
	// console.
	// 
	/**
	* Returns 1 if I am the master !
	* Must be implemented in derived classes otherwize I am NOT the master
	*/
	virtual int IAmTheMaster ()
	{
		return 0;
	}
	/**
	* Returns the name of the class
	*/
	char   *ClassName ()
	{
		return ("");
	}
      protected:
	/**
	* Indicates the number of processors in the oopar environment.
	*/
	int     f_num_proc;
	/**
	* Indicates the objects number
	*/
	int     f_myself;
};
extern OOPCommunicationManager *CM;
#endif // _COMMUNICHH_
