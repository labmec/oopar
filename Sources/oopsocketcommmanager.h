//
// C++ Interface: oopsocketcommmanager
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPSOCKETCOMMMANAGER_H
#define OOPSOCKETCOMMMANAGER_H

#include "oopcommmanager.h"
#include "oopsocketstorage.h"
#include "oopsocket.h"

#include <semaphore.h>

/**
	@author Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>
*/
class OOPSocketCommManager : public OOPCommunicationManager
{
private:
    OOPSocket SOCKET;

    pthread_t fReceiveThread;

    static void *receiver(void *);

public:
    OOPSocketCommManager();

    ~OOPSocketCommManager();

    // Heranca
    int Initialize (char *process_name=NULL, int num_of_process=-1);

    int SendTask (OOPTask *pTask);

    int SendMessages ();

    int IAmTheMaster ();

    char *ClassName();
		
	void Barrier()
	{
		SOCKET.Barrier();
	}
		
	
};

#endif
