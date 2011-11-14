/**
 * @file
 */
#ifndef TTASKMANAGER
#define TTASKMANAGER

#ifndef WIN32
#include <pthread.h>
#endif

#include "ooptask.h"
#include "oopcommmanager.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif

#include <list>
#include <set>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

class OOPStorageBuffer;
class OOPDataVersion;
class OOPSaveable;
class OOPTaskControl;
using namespace std;
class OOPObjectId;
class TMLock;
class OOPDataManager;

/**
 * @addtogroup managertask
 * @{
 */

enum TMMessageType {
	ETMAccessGranted,
	ETMCancelTask,
};


struct SszQueues
{
	int fWaiting_sz;
	int fExecabl_sz;
	int fExecing_sz;
	void set (int wait, int execab, int execing)
	{
		fWaiting_sz = wait;
		fExecabl_sz = execab;
		fExecing_sz = execing;
	}
	bool IsEqual (int wait, int execab, int execing)
	{
		if (execing != fExecing_sz || execab != fExecabl_sz
			|| wait != fWaiting_sz)
			return false;
		return true;
	}
};

/**
 * @brief Implements the manager of tasks on the environment.
 * All parallelized task are submitted to environment through the TaskManager.
 * Along with Communication Manager and Data Manager, Task Manager acts as daemon on all nodes
 * present on the environment.
 */
class OOPTaskManager
{
	friend class OOPTask;
public:
	ofstream * fMainLog;
	/**
	 * @brief Dumps on disk the current state of the Manager object
	 */
	void SnapShotMe(std::ostream & out);
	/**
	 * @brief Set max number of simultaneous threads.
	 */
	void SetNumberOfThreads (const int n);
	
	/**
	 * @brief Associate the Datamanager with the task manager
	 */
	void SetDataManager(TPZAutoPointer<OOPDataManager> DM);
	
	/**
	 * @brief Associate the Comunication Mangager with the task manager
	 */
	void SetCommunicationManager(TPZAutoPointer<OOPCommunicationManager> CM);
	
	/**
	 * @brief Access method for the communication Manager
	 */
	TPZAutoPointer<OOPCommunicationManager> CM();
	
	/**
	 * @brief Access method for the data manager
	 */
	TPZAutoPointer<OOPDataManager> DM();
	
	/**
	 * @brief Access method for the autopointer reference to itself
	 */
	TPZAutoPointer<OOPTaskManager> TM();
	
	/**
	 * Clear the pointer so the object can be deleted
	 */
	void ClearPointer();
	
	/**
	 * @brief Get max number of simultaneous threads.
	 */
	int NumberOfThreads ();
	
	void Wait ();
	
	/**
	 * @brief Indicates that the task manager should not send tasks anymore
	 */
	void StopSending();
	/**
	 * @brief Receiving a stop sending confirmation
	 */
	void StopSendingConfirmation(int procorigin);
	/**
	 * @brief Sets the KeepGoing flag which will control the TM Execute method
	 */
	void SetKeepGoing (bool go);
	
	/**
	 * @brief Print TM task queues
	 */
	void PrintTaskQueues (char *msg, std::ostream & out);
	/**
	 * @brief Print TaskManager data structure
	 */
	void Print (std::ostream & out);
	/**
	 * @brief Checks for the existence for obselete tasks on the task manager list
	 */
	void CleanUpTasks ();
	/**
	 * @brief Constructor passing processor id as parameter.
	 * @param proc Processor where the TM is created.
	 */
	OOPTaskManager (int proc);
	/**
	 * @brief Simple destructor
	 */
	~OOPTaskManager ();
	/**
	 * @brief Submits a task to the TaskManager. \n
	 * Assigns to that task a unique Id on the environment.
	 * @param task Pointer to the submitted task.
	 */
public:
	
    OOPObjectId Submit (OOPTask * task);
public:
	/**
	 * @brief Executes a daemon task as soon as it is submitted to the current task manager
	 * @param dmt Pointer to the submitted task.
	 */
	void ExecuteDaemon(OOPTask * dmt);
	/**
	 * @brief Returns the number tasks currently being managed by this data manager
	 */
	int NumberOfTasks();
	/**
	 * @brief Returns the total number of task on the environment.
	 */
	int GlobalNumberOfTasks();
	/**
	 * @brief Inserts in the fMessages list an instruction for the cancelation of a Task.
	 * @param tag Tag contains the Id of the task which will be canceled.
	 */
	void CancelTask (OOPAccessTag & tag);
	
	/**
	 * @brief Transfer Tasks from the Executing tasks list to the Finished tasks list
	 */
	void TransferExecutingTasks ();
	/**
	 * @brief Transfer the tasks which are in the fSubmittedList to the fTaskList, registering their dependencies
	 */
	void TransferSubmittedTasks ();
	/**
	 * @brief Indicate to the TaskManager that a given task can execute
	 */
	void TransfertoExecutable (const OOPObjectId & taskid);
	/**
	 * @brief Triggers the Service Thread method for the OOPar environment.
	 */
	void Execute ();
	/**
	 * @brief ServiceThread execution method for the TM. \n
	 * This method also triggers the services threads for the remaining Managers
	 */
	static void * ExecuteMTBlocking (void *data);
	
	/**
	 * @brief Grants access to task identified in the Tag Object
	 * @param tag The tag represents a Task requiring access to a Data.
	 * @note With this method the Requirement represented in the tag is satisfied.
	 */
	void GrantAccess(OOPAccessTag & tag);
	/**
	 * @brief The TM ServiceThread goes to sleep when it had completed all its tasks. \n
	 * New tasks submissions should wake the ServiceThread  Up. This is performed calling WakeUpCall()
	 * @note It Posts a sem_post on the ServiceThread semaphore which went to sleep by calling a sem_wait. \n
	 * Service thread now sleeps based on a semaphore type. 
	 * Semaphore are used instead of mutex and conditional variables combined.
	 * Semaphores avoid deadlocking in the cond_signal, cond_wait, mutex_lock and unlocking
	 */
	void WakeUpCall(){
		fServiceSemaphore->post();
	}
	/**
	 * @brief Returns true if the service thread has work to do
	 */
	bool KeepRunning();
	/**
	 * @brief Puts the TM ServiceThread to sleep since there is nothing else to do. \n
	 * The ServiceThread then needs to be awaken by a call to WakeUpCall()
	 */
	void WaitWakeUpCall();
	/**
	 * @brief Whenever possible, tasks which can be executed by this method. \n
	 * Complies with the data protection of the TM data structure
	 */
	void TriggerTasks();
	/**
	 * @brief Returns the mutex which will synchronize the locking operations
	 */
	pthread_mutex_t *Mutex()
	{
		return &fMutex;
	}
	
	
	/**
	 * @brief Handles the messages contained on the fMessages list
	 */
	void HandleMessages();
	
private:
	
	/**
	 * @brief The associated communication manager
	 */
	TPZAutoPointer<OOPCommunicationManager> fCM;
	
	/**
	 * @brief The associated data manager
	 */
	TPZAutoPointer<OOPDataManager> fDM;
	
	/**
	 * @brief The autopointer associated with this
	 */
	TPZAutoPointer<OOPTaskManager> fTM;
	
	/**
	 * @brief Max number of threads
	 */
	int fNumberOfThreads;
	/**
	 * @brief Thread which is the main execution loop of the task manager
	 */
	pthread_t fExecuteThread;
	/**
	 * @brief Thread which owns the lock
	 */
	pthread_t fLockThread;
	
	/**
	 * @brief The mutex object around which we will be locking
	 */
	pthread_mutex_t fMutex;
	
	/**
	 * @brief Indicates if TM must continue its processing
	 */
	bool fKeepGoing;
	
	/**
	 * @brief Indicates the the TM should not send tasks or not
	 */
	bool fStopSending;
	
	/**
	 * @brief The processors which confirmed the stop sending command
	 */
	std::set<int> fConfirmedProcessors;
	
	/**
	 * @brief Semaphore for the ServiceThread
	 */
	boost::interprocess::interprocess_semaphore * fServiceSemaphore;
	
	/**
	 * @brief Generate a unique id number
	 */
	OOPObjectId GenerateId ();
	/**
	 * @brief Find the task with the given id.
	 */
	OOPTask *FindTask (OOPObjectId taskid);	//
	/**
	 * @brief Reorder the tasks according to their priority
	 */
	int fProc;
	/**
	 * @brief Counter for number of objects assigned to this manager. \n
	 * Whenever a new task is assigned, fLastCreated is incremented.
	 */
	long fLastCreated;
	/**
	 * @brief Maximum number of generated Id.
	 */
	int fReceiveThreadCreated;
	/**
	 * @brief List of tasks which can't be executed yet
	 */
	list < OOPTaskControl * >fTaskList;
	/**
	 * @brief List of tasks which can be readily executed
	 */
	list < OOPTaskControl * >fExecutable;
	/**
	 * @brief List of executing tasks
	 */
	list < OOPTaskControl * >fExecuting;
	
	/**
	 * @brief List of tasks recently submitted
	 */
	list < OOPTask * >fSubmittedList;
	/**
	 * @brief Holds a list of messages to the TM. 
	 * The messages are translated in actions to the Tasks on TM. \n
	 * The list is composed by pairs of Type and AccessTags. \n
	 * Types can be TMAccessGranted or TMCancelTask
	 */
	std::list <std::pair< int, OOPAccessTag> > fMessages;
	/**
	 * @brief Translates a GrantAccess message to its necessary action on the TM context
	 */
	void ExtractGrantAccessFromTag(const OOPAccessTag & tag);
	/**
	 * @brief Translates a CancelTask message to its necessary action on the TM context
	 */
	void ExtractCancelTaskFromTag(const OOPAccessTag & tag);
};

/**
 * @}
 */

#endif
