/*
 * oopinternalcommanager.h
 *
 *  Created on: Jul 21, 2009
 *      Author: phil
 */

#ifndef OOPINTERNALCOMMANAGER_H_
#define OOPINTERNALCOMMANAGER_H_

//#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include "oopcommmanager.h"
#include "tpzautopointer.h"
#include "pzvec.h"
#include "oopsocketstorage.h"
#include <list>

/**
 * @brief Implements the communication between processes.
 * @author Philippe Devloo
 * @since 21/07/2009
 * @ingroup managercomm
 */
class OOPInternalCommunicationManager : public OOPCommunicationManager
{
	
public:
	
	/**
	 * @brief Constructur identifying the number of the actual processor and the number of processes in the pool
	 */
	OOPInternalCommunicationManager(int procid, int numproc);
	
	virtual ~OOPInternalCommunicationManager();
	
	/**
	 * @brief Identify a communication manager to exchange messages with
	 */
	void SetCommunicationManager(int procid, TPZAutoPointer<OOPCommunicationManager> partner);
	
    /**
     * @brief get the vector of communication managers
     */
    TPZVec<TPZAutoPointer<OOPCommunicationManager> > GetCommunicationManagers()
    {
        return fNeighbours;
    }
	/**
	 * @brief Initialize the processing threads. \n
	 * This method must be called before any communication activity
	 */
	virtual int Initialize(char *process_name, int num_of_process);
	
	/**
	 * @brief Send a task object to a processor identified by the task
	 * @param *pTask : A pointer to the object TTask which will be sent
	 */
	int SendTask(OOPTask* pTask);
	
 	/**
	 * @brief Sends all messages in all buffers
	 * will return after all messages have been processed
	 */
 	virtual int SendMessages();
	
 	/**
	 * @brief Returns 1 if I am the master !
	 * The master corresponds to \f$ procid == 0 \f$ \n
	 * Must be implemented in derived classes otherwise I am NOT the master
	 */
    virtual int IAmTheMaster();
	
    /**
     * @brief Receive an object serialized in a buffer
     */
    void ReceiveObject(TPZAutoPointer<OOPSocketStorageBuffer> buf);
	
    void WaitWakeUpCall()
    {
  		fServiceSemaphore->Wait();
    }
    void WakeUpCall();
	
    void SetKeepGoing(bool go);
	
    bool KeepGoing();
	
    /**
     * @brief Clear the pointer so the object can be deleted
     */
    virtual void ClearPointer();
	
	
    /**
     * @brief Terminate the execution thread
     */
    virtual void JoinThread();
	
    /**
	 * @brief return the mutex which will synchronize the locking operations
	 */
	pthread_mutex_t *Mutex()
	{
		return &fMutex;
	}
	
private:
	
	/**
	 * @brief Pointer to the service thread which handles processing of the communication manager
	 */
	pthread_t fServiceThread;
	/**
	 * @brief Service thread execution method for the CM
	 */
	static void * ServiceThread(void * data);
	/**
	 * @brief Semaphore for the CM service thread
	 * We switched to boost because of Macintosh issues
	 */
    TPZSemaphore *fServiceSemaphore;
//	boost::interprocess::interprocess_semaphore * fServiceSemaphore;
	
	/**
	 * @brief The mutex object around which we will be locking
	 */
	pthread_mutex_t fMutex;
	
	/**
	 * @brief Indicates wether ServiceThread should keep running
	 */
	bool fKeepGoing;
	
	/**
	 * @brief List of neighbours of the communication manager
	 */
	TPZVec<TPZAutoPointer<OOPCommunicationManager> > fNeighbours;
	
	/**
	 * @brief List of submitted objects
	 */
	std::list<TPZAutoPointer<OOPSocketStorageBuffer> > fReceived;
};

#endif /* OOPINTERNALCOMMANAGER_H_ */
