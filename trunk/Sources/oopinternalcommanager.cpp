/*
 * oopinternalcommanager.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: phil
 */

#include "pzlog.h"
#include "oopsocketstorage.h"
#include "ooptaskmanager.h"
#include "ooptask.h"
#include "ooplock.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPar.OOPInternalCommunicationManager"));
#endif

#include "oopinternalcommanager.h"
#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
extern OOPGenericLockService gMutex;
#endif

	/**
	 * Constructur identifying the number of the actual processor and the number of processes in the pool
	 */
	OOPInternalCommunicationManager::OOPInternalCommunicationManager(int procid, int numproc) : OOPCommunicationManager(), fServiceSemaphore(0)
	{
		f_myself = procid;
		f_num_proc = numproc;
		fNeighbours.Resize(numproc);
		pthread_mutex_init (&fMutex, 0);
		fServiceSemaphore = new boost::interprocess::interprocess_semaphore(0);
		fServiceThread = 0;
		fKeepGoing = true;
#ifdef LOG4CXX
		{
//			std::stringstream sout;
			std::cout << __PRETTY_FUNCTION__ << (void *) fServiceSemaphore << " myself " << f_myself << " numproc " << f_num_proc;
//			LOGPZ_DEBUG(logger,sout.str())
		}
#endif
	}

	OOPInternalCommunicationManager::~OOPInternalCommunicationManager()
	{
		pthread_mutex_destroy (&fMutex);
	}

	/**
	 * Identify a communication manager to exchange messages with
	 */
	void OOPInternalCommunicationManager::SetCommunicationManager(int procid, TPZAutoPointer<OOPCommunicationManager> partner)
	{
		if(procid < f_num_proc)
		{
			this->fNeighbours[procid] = partner;
		}
		else
		{
			std::stringstream sout;
			sout << __PRETTY_FUNCTION__ << " called with wrong arguments procid = " << procid << " myself " << f_myself <<
			" number of processors " << f_num_proc;
			std::cout << sout.str() << std::endl;
#ifdef LOG4CXX
			LOGPZ_ERROR(logger,sout.str())
#endif
		}

	}

	/**
	 * Initialize the processing threads
	 * This method must be called before any communication activity
	 */
     int OOPInternalCommunicationManager::Initialize (char *process_name, int num_of_process)
     {
    	 if(!fServiceThread)
    	 {
    		 pthread_create (&fServiceThread, NULL, ServiceThread, this);
#ifdef LOG4CXX
    		 std::stringstream sout;
    		 sout << __PRETTY_FUNCTION__ << " processor " << GetProcID() << "Thread successfully created";
    		 LOGPZ_DEBUG(logger,sout.str());
#endif

    	 }
    	 else
    	 {
    		 std::stringstream sout;
    		 sout << __PRETTY_FUNCTION__ << "Thread was already initialized!!!";
#ifdef LOG4CXX
    		 LOGPZ_ERROR(logger,sout.str());
#else
    		 cout << sout.str() << std::endl;
#endif
    	 }

// initialize the working thread
    	 return 0;
     }

	/**
	 * Send a task object to a processor identified by the task
	 * @param *pTask : A pointer to the object TTask which will be sent
	 */
     int OOPInternalCommunicationManager::SendTask (OOPTask * pTask)
     {
    	 // create a buffer with dynamic memory
    	 OOPSocketStorageBuffer *newbuf  = new OOPSocketStorageBuffer;
    	 TPZAutoPointer<OOPSocketStorageBuffer> buf(newbuf);
    	 // serialize the object
    	 pTask->Write(*newbuf,1);
    	 // identify the destination processor
    	 int procdest = pTask->GetProcID();
#ifdef LOG4CXX
    	 {
    		 std::stringstream sout;
    		 sout << "Serializing task with ClassId " << pTask->ClassId() << " sending to processor " << procdest;
    		 sout << " buffer address " << (void *) newbuf;
    		 LOGPZ_DEBUG(logger,sout.str())
    	 }
#endif
    	 // delete the task object
    	 delete pTask;
    	 // put the buffer in the queue of the corresponding communication manager
    	 OOPInternalCommunicationManager *CM = dynamic_cast<OOPInternalCommunicationManager *>(fNeighbours[procdest].operator->());
    	 if(!CM)
    	 {
    		 std::stringstream sout;
    		 sout << __PRETTY_FUNCTION__ << " Destination processor does not exist " << procdest;
#ifdef LOG4CXX
    		 LOGPZ_ERROR(logger,sout.str());
#elif
    		 cout << sout.str();
#endif
    		 exit(-1);
    	 }
    	 else
    	 {
    		 CM->ReceiveObject(buf);
    	 }

    	 return 1;

     }

 	/**
 	* Sends all messages in all buffers
 	* will return after all messages have been processed
 	*/
 	int OOPInternalCommunicationManager::SendMessages ()
 	{
 		// nothing to do, messages are always instantaneously sent
 		return 1;
 	}

 	/**
 	* Returns 1 if I am the master !
 	* The master corresponds to procid == 0
 	* Must be implemented in derived classes otherwize I am NOT the master
 	*/
    int OOPInternalCommunicationManager::IAmTheMaster ()
    {
    	// i am the master if my procid == 0
    	return f_myself == 0;
    }

    /**
     * Receive an object serialized in a buffer
     */
    void OOPInternalCommunicationManager::ReceiveObject(TPZAutoPointer<OOPSocketStorageBuffer> buf)
    {
    	OOPLock<OOPInternalCommunicationManager> lock(this);
    	fReceived.push_back(buf);
#ifdef LOG4CXX
    	LOGPZ_DEBUG(logger,"Pushed back an object, giving a wakup call")
#endif
    	WakeUpCall();
    }

	/**
 * Service thread execution method for the CM
 */
void * OOPInternalCommunicationManager::ServiceThread(void * data)
{
	OOPInternalCommunicationManager *CMp = (OOPInternalCommunicationManager *) (data);
	TPZAutoPointer<OOPTaskManager> TM = CMp->TM();
	// this garantees that the communication manager will stay alive
	TPZAutoPointer<OOPCommunicationManager> CM = CMp->CM();
	bool keepgoing;
	{
		OOPLock<OOPInternalCommunicationManager> lock(CMp);
		keepgoing = CMp->fKeepGoing;
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Initialized the service thread for proc " << CM->GetProcID() << " keep going " << keepgoing;
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif

	while (keepgoing) {

		CMp->WaitWakeUpCall();
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger,"Passed through WaitWakeUpCall trying to acquire the lock")
#endif
#ifdef STEP_MUTEX
		OOPLock<OOPGenericLockService> lock(&gMutex);
#endif
#ifdef LOG4CXX
		{
			stringstream sout;
			sout << "Proc " << CM->GetProcID()
					<< " Wake Up | One more round ------------------------------------------------------------------"
					<< std::endl;
			LOGPZ_DEBUG(logger, sout.str())
		}
#endif
		std::list<TPZAutoPointer<OOPSocketStorageBuffer> > received;
		{
			OOPLock<OOPInternalCommunicationManager> lock(CMp);
			received = CMp->fReceived;
			CMp->fReceived.clear();
			keepgoing = CMp->fKeepGoing;
		}
		while(!received.empty())
		{
			TPZAutoPointer<OOPSocketStorageBuffer> obj = received.front();
			received.pop_front();
			TPZStream *str = obj.operator->();
#ifdef LOG4CXX
			{
				std::stringstream sout;
				sout << "Receiving object with buffer address " << (void *) str;
				LOGPZ_DEBUG(logger,sout.str())
			}
#endif
//			TPZSaveable *save = TPZSaveable::Restore(*str,0);
			TPZSaveable *save = obj->Restore();
			OOPTask *task = dynamic_cast<OOPTask *>(save);
			if(task)
			{
				CM->TM()->Submit(task);
			}
			else
			{
				delete save;
			}
		}
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Falling through the service thread for processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
	return 0;
}

void OOPInternalCommunicationManager::SetKeepGoing(bool go)
{
	OOPLock<OOPInternalCommunicationManager> lock(this);
	fKeepGoing = go;
	WakeUpCall();
}

bool OOPInternalCommunicationManager::KeepGoing()
{
	OOPLock<OOPInternalCommunicationManager> lock(this);
	bool go = fKeepGoing;
	return go;
}

/**
 * Terminate the execution thread
 */
void OOPInternalCommunicationManager::JoinThread()
{
	  void *execptr;
	  void **executorresultptr = &execptr;
	  LOGPZ_DEBUG(logger,"Trying to join thread")
	  int result = pthread_join(fServiceThread,executorresultptr);
	  if(result)
	  {
	#ifdef LOG4CXX
	    stringstream sout;
	    sout << __FUNCTION__ << __LINE__ << " join operation failed with result " << result;
	    LOGPZ_ERROR(logger,sout.str())
	#endif
	  }
	  LOGPZ_DEBUG(logger,"Join thread succeeded")
	  OOPCommunicationManager::JoinThread();

}

void OOPInternalCommunicationManager::WakeUpCall()
{
  //sem_post(&fServiceSemaphore);
	fServiceSemaphore->post();
#ifdef LOG4CXX
	std::stringstream sout;
	sout << "Post wakeup Call for processor " << GetProcID();
	LOGPZ_DEBUG(logger,sout.str())
#endif

}
/**
 * Clear the pointer so the object can be deleted
 */
void OOPInternalCommunicationManager::ClearPointer()
{
	int nsize = fNeighbours.NElements();
	int i;
	for(i=0; i<nsize; i++)
	{
		fNeighbours[i] = TPZAutoPointer<OOPCommunicationManager>(0);
	}
	LOGPZ_DEBUG(logger,"Resetted the autopointers to communication managers")
	OOPCommunicationManager::ClearPointer();
	LOGPZ_DEBUG(logger,"After ClearPointer of the OOPCommunicationManager")
}
