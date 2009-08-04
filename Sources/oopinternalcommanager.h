/*
 * oopinternalcommanager.h
 *
 *  Created on: Jul 21, 2009
 *      Author: phil
 */

#ifndef OOPINTERNALCOMMANAGER_H_
#define OOPINTERNALCOMMANAGER_H_

#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include "oopcommmanager.h"
#include "tpzautopointer.h"
#include "pzvec.h"
#include "oopsocketstorage.h"
#include <list>
//OOPSocketStorageBuffer

class OOPInternalCommunicationManager : public OOPCommunicationManager
{

public:

	/**
	 * Constructur identifying the number of the actual processor and the number of processes in the pool
	 */
	OOPInternalCommunicationManager(int procid, int numproc);

	virtual ~OOPInternalCommunicationManager();

	/**
	 * Identify a communication manager to exchange messages with
	 */
	void SetCommunicationManager(int procid, TPZAutoPointer<OOPCommunicationManager> partner);

	/**
	 * Initialize the processing threads
	 * This method must be called before any communication activity
	 */
     virtual int Initialize (char *process_name, int num_of_process);

	/**
	 * Send a task object to a processor identified by the task
	 * @param *pTask : A pointer to the object TTask which will be sent
	 */
     int SendTask (OOPTask * pTask);

 	/**
 	* Sends all messages in all buffers
 	* will return after all messages have been processed
 	*/
 	virtual int SendMessages ();

 	/**
 	* Returns 1 if I am the master !
 	* The master corresponds to procid == 0
 	* Must be implemented in derived classes otherwize I am NOT the master
 	*/
    virtual int IAmTheMaster ();

    /**
     * Receive an object serialized in a buffer
     */
    void ReceiveObject(TPZAutoPointer<OOPSocketStorageBuffer> buf);

    void WaitWakeUpCall()
    {
      //sem_wait(&fServiceSemaphore);
  		fServiceSemaphore->wait();
    }
    void WakeUpCall();

    void SetKeepGoing(bool go);

    bool KeepGoing();

    /**
     * Clear the pointer so the object can be deleted
     */
    virtual void ClearPointer();


    /**
     * Terminate the execution thread
     */
    virtual void JoinThread();

    /**
	 * return the mutex which will synchronize the locking operations
	 */
	pthread_mutex_t *Mutex()
	{
		return &fMutex;
	}




private:

	/**
	 * Pointer to the service thread which handles processing of the communication manager
	 */
	pthread_t fServiceThread;
	/**
	 * Service thread execution method for the CM
	 */
	static void * ServiceThread(void * data);
	/**
	 * Semaphore for the CM service thread
	 * We switched to boost because of Macintosh issues
	 */
	//sem_t fServiceSemaphore;
	boost::interprocess::interprocess_semaphore * fServiceSemaphore;

	/**
	 * the mutex object around which we will be locking
	 */
	pthread_mutex_t fMutex;


	/**
	 * Indicates wether ServiceThread should keep running
	 */
	bool fKeepGoing;

	/**
	 * List of neighbours of the communication manager
	 */
	TPZVec<TPZAutoPointer<OOPCommunicationManager> > fNeighbours;

	/**
	 * List of submitted objects
	 */
	std::list<TPZAutoPointer<OOPSocketStorageBuffer> > fReceived;
};
#endif /* OOPINTERNALCOMMANAGER_H_ */
