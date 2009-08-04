//
// C++ Implementation: oopsocketcommmanager
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopsocketcommmanager.h"
#include "ooptaskmanager.h"

#include <pthread.h>
#include <pzlog.h>

#include <sstream>

#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPar.OOPSocketCommManager"));
#endif

using namespace std;
//extern OOPTaskManager *TM;


pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;

void *OOPSocketCommManager::receiver(void * data)
{
    OOPSocketCommManager *CM = static_cast<OOPSocketCommManager*>(data);

    while(true)
    {
        // Recebendo mensagem (Task)

        SOCKET_Status status;
        int probres=-1;
        try
        {
            probres = CM->SOCKET.Probe(SOCKET_ANY_SOURCE, SOCKET_ANY_TAG, &status);
        }
        catch(const exception & e)
        {
#ifdef LOG4CXX
            {
                stringstream sout;
                sout << "OOPSocketStorage: Exception catched on SOCKET_Probe ! " << e.what();
                LOGPZ_ERROR(logger,sout.str().c_str());
            }
#endif
            std::cout << "OOPSocketStorage: Exception catched on SOCKET_Probe ! " << e.what();
            exit(-1);
        }
        int count = -1;
        try
        {
            CM->SOCKET.Get_count(&status, SOCKET_PACKED , &count);
        }
        catch(const exception & e)
        {
            stringstream sout;
            sout << "OOPSocketStorage: Exception catched on SOCKET_Get_count ! " << e.what();
#ifdef LOG4CXX
            LOGPZ_ERROR(logger,sout.str().c_str());
#endif
            cout << sout.str().c_str();
            exit(-1);
        }
#ifdef LOG4CXX
        {
            stringstream sout;
            sout << "OOPSocketStorage: Receiving " << count << " bytes !!" << " probres = " << probres ;
            LOGPZ_DEBUG(logger,sout.str().c_str());
        }
#endif
        OOPSocketStorageBuffer buffer;
        int res = -1;
        if (count)
        {
            buffer.Resize(count);
        }
        else
        {
            printf("OOPSocketStorage: Resizing buffer to 1\n");fflush(stdout);
            buffer.Resize(1);
        }
        try
        {
            res = CM->SOCKET.Recv(&buffer[0], count, SOCKET_PACKED, SOCKET_ANY_SOURCE, SOCKET_ANY_TAG, &status);
        }
        catch(const exception & e)
        {
            stringstream sout;
            sout << "OOPSocketStorage: Exception catched on SOCKET.Recv ! " << e.what();
#ifdef LOG4CXX
            LOGPZ_ERROR(logger,sout.str().c_str());
#endif
            cout << sout.str().c_str();
            exit(-1);
        }
        if(res == SOCKET_SUCCESS)
        {
            stringstream sout;
            sout << "OOPSocketStorage: SOCKET.Recv called successfully ! ";
#ifdef LOG4CXX
            LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
        }
        else
        {
            stringstream sout;
            sout << "OOPSocketStorage: SOCKET.Recv called FAILED! ";
#ifdef LOG4CXX
            LOGPZ_ERROR(logger,sout.str().c_str());
#endif
            exit(-1);
        }


        // Processando mensagem (Task)
        pthread_mutex_lock(&fCommunicate);
        TPZSaveable *obj = buffer.Restore();
        pthread_mutex_unlock(&fCommunicate);

        if(obj == NULL)
        {
#ifdef LOG4CXX
            std::stringstream sout;
            sout << "OOPSocketCommManager::Restored buffer NULL!";
            LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
        }
        OOPTask *task = dynamic_cast<OOPTask *> (obj);
        if(task) {
            CM->TM()->Submit(task);
        } else {
#ifdef LOG4CXX
            std::stringstream sout;
            sout << "OOPSocketCommManager::ProcessMessage received an object which is not a task";
            LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
            delete obj;
        }
    }

    return 0;
}


OOPSocketCommManager::OOPSocketCommManager() : OOPCommunicationManager()
{
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << "OOPSocketCommManager: Initializing SocketCommManager" << std::endl;
    cout << sout.str().c_str();
    cout.flush();
  }
#endif
  SOCKET.Init_thread();
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << "OOPSocketCommManager: SOCKET.Init_thread called." << std::endl;
    cout << sout.str().c_str();
    cout.flush();
  }
#endif

  Initialize();

  int res = -1;
  res = pthread_create(&fReceiveThread, NULL, receiver, this);
  if(res)
  {
#ifdef LOG4CXX
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << " OOPSocketCommManager: Fail to create thread";
      LOGPZ_DEBUG(logger,sout.str().c_str());
      cout << sout.str().c_str() << endl;
#endif
  }
}


OOPSocketCommManager::~OOPSocketCommManager()
{
	{
		std::stringstream sout;
		sout << "Terminating OOPSocketCommManager" << endl;
		sout << "OOPSocketCommManager: Processor " << f_myself  << " reached synchronization point !" << endl;
		cout << sout.str();
		cout.flush();
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger,sout.str())
#endif
	}
    SOCKET.Barrier();
    {
    	std::stringstream sout;
        sout << "OOPSocketCommManager: Calling Finalize for " << f_myself << endl;
        cout << sout.str();
        cout.flush();
#ifdef LOG4CXX
        LOGPZ_DEBUG(logger,sout.str())
#endif
    }
    SOCKET.Finalize();
    LOGPZ_DEBUG(logger,"Leaving destructor of OOPSocketCommManager")
}


int OOPSocketCommManager::Initialize (char *process_name, int num_of_process)
{
    f_num_proc = SOCKET.Comm_size();
    f_myself = SOCKET.Comm_rank();
    SOCKET.Barrier();

#ifdef LOG4CXX
    {
        stringstream sout;
        sout << "OOPSocketCommManager: SocketComm Initialize f_myself " << f_myself << " f_num_proc " << f_num_proc;
        LOGPZ_DEBUG(logger, sout.str().c_str());
        cout << sout.str().c_str() << endl;
        cout.flush();
    }
#endif
    if (f_myself == 0)
        return f_num_proc;
    else
        return 0;
}


int OOPSocketCommManager::SendTask (OOPTask * pTask)
{
    // Preparando mensagem
    {
#ifdef LOG4CXX
        stringstream sout;
        sout << "OOPSocketCommManager: Sending Task Id:" << pTask->Id() << " ClassId:" << pTask->ClassId() <<
                " to proc " << pTask->GetProcID ();
        LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
    }
  int process_id = pTask->GetProcID ();
  if (process_id >= f_num_proc || process_id < 0) {
      stringstream sout;
      sout << "OOPSocketCommManager: Sending Task to a processor which doesn't exist!\nFinishing SocketCommManager !\nFarewell !";
#ifdef LOG4CXX
      LOGPZ_ERROR(logger,sout.str().c_str());
#endif
      delete pTask;
      return -1;
  }
  // Se estiver tentando enviar para mim mesmo.
  if (process_id == f_myself)
  {
      stringstream sout;
      sout << "OOPSocketCommManager: Trying to send a Task to myself!\nSorry but this is wrong!\nFarewell !";
#ifdef LOG4CXX
      LOGPZ_ERROR(logger,sout.str().c_str());
#endif
      delete pTask;
      return -1;
  }

  OOPSocketStorageBuffer *Buffer = new OOPSocketStorageBuffer;
  pTask->Write (*Buffer, 1);

  // Enviando mensagem
  int tag = 0, ret;

#ifdef LOG4CXX
  {
      stringstream sout;
      sout << "OOPSocketStorage: Calling SOCKET.Send";
      LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  try
  {
      ret = SOCKET.Send(Buffer, SOCKET_PACKED, process_id, tag);
  }
  catch(const exception& e)
  {
#ifdef LOG4CXX
      {
          stringstream sout;
          sout << "OOPSocketStorage: Exception catched ! " << e.what();
          LOGPZ_ERROR(logger,sout.str().c_str());
          exit (-1);
      }
#endif
  }
#ifdef LOG4CXX
  {
      stringstream sout;
      sout << "OOPSocketStorage: Called SOCKET.Send";
      LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  delete pTask;
  return 1;
}


int OOPSocketCommManager::SendMessages ()
{
  return 0;
}


int OOPSocketCommManager::IAmTheMaster ()
{
  return (f_myself == 0);
}


char * OOPSocketCommManager::ClassName()
{
  return ("OOPSocketCommManager::");
}
