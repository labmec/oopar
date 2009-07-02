#ifndef OOPDUMBCOMMMAN_H
#define OOPDUMBCOMMMAN_H

#include "OOPCommManager.h"

/*
 *  OOPDumbCommMan.h
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 5/25/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

class OOPDumbCommManager : public OOPCommunicationManager
{
public:
	OOPDumbCommManager() 
	{
		f_num_proc = 1;
		f_myself = 0;
	}
private:
	virtual int Initialize (char *process_name, int num_of_process) 
	{
		f_num_proc = 1;
		f_myself = 0;
	}
	
	/**
	 * Returns the processor number in which this object is running
	 */
	int     GetProcID () {return 0;}
	
	/**
	 * Returns the total number of processors
	 */
	int     NumProcessors () {return 1;}
	
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
	virtual int SendTask (OOPTask * pTask) {return 0;}
	// Recebe uma mensagem qualquer que tenha chegado. Se nao huver
	// nenhuma mensagem, retorna 0.
	// 
	/**
	 * Receives any arrived messages.
	 * Returns zero if none has arrived
	 */
	// 	virtual int ReceiveMessages () = 0;
	// Envia todas as mensagens de todos os buffers.
	// 
	/**
	 * Sends all messages in all buffers
	 */
	virtual int SendMessages (){return 0;}
	// Retorna 0 se este processo nao tiver sido disparado pelo
	// console.
	// 
	/**
	 * Returns 1 if I am the master !
	 * Must be implemented in derived classes otherwize I am NOT the master
	 */
	virtual int IAmTheMaster (){return 1;}
	
	/**
	 * Returns the name of the class
	 */
	char   *ClassName () {return "DumbComm";}
	
protected:
	
};

#endif
