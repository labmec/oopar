// -*- c++ -*-
#ifndef TTASKMANAGER
#define TTASKMANAGER
#ifndef WIN32
#include <pthread.h>
#endif
#include "ooptask.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif

#include <list>
#include <set>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
//#include <semaphore.h>

class OOPStorageBuffer;
class OOPDataVersion;
class OOPSaveable;
class OOPTaskControl;
using namespace std;
class OOPObjectId;
class TMLock;
class OOPDataManager;
class OOPCommunicationManager;


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
 * Implements the manager of tasks on the environment.
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
   * Dumps on disk the current state of the Manager object
   */
  void SnapShotMe(std::ostream & out);
  /**
   * Set max number of simultaneous threads.
   */
  void SetNumberOfThreads (const int n);

  /**
   * Associate the Datamanager with the task manager
   */
  void SetDataManager(TPZAutoPointer<OOPDataManager> DM);

  /**
   * Associate the Comunication Mangager with the task manager
   */
  void SetCommunicationManager(TPZAutoPointer<OOPCommunicationManager> CM);

  /**
   * Access method for the communication Manager
   */
  TPZAutoPointer<OOPCommunicationManager> CM();

  /**
   * Access method for the data manager
   */
  TPZAutoPointer<OOPDataManager> DM();

  /**
   * Access method for the autopointer reference to itself
   */
  TPZAutoPointer<OOPTaskManager> TM();

  /**
   * Clear the pointer so the object can be deleted
   */
  void ClearPointer();

  /**
   * Get max number of simultaneous threads.
   */
  int NumberOfThreads ();

  void Wait ();

  /**
   * Indicates that the task manager should not send tasks anymore
   */
  void StopSending();
  /**
   * receiving a stop sending confirmation
   */
  void StopSendingConfirmation(int procorigin);
  /**
   * Sets the KeepGoing flag which will control the TM Execute method
   */
  void SetKeepGoing (bool go);

  /**
   * Print TM task queues
   */
  void PrintTaskQueues (char *msg, std::ostream & out);
  /**
   * Print TaskManager data structure
   */
  void Print (std::ostream & out);
  /**
   * Checks for the existence for obselete tasks on the task manager list
   */
  void CleanUpTasks ();
  /**
   * Constructor passing processor id as parameter.
   * @param proc Processor where the TM is created.
   */
  OOPTaskManager (int proc);
  /**
   * Simple destructor
   */
  ~OOPTaskManager ();
  /**
   * Submits a task to the TaskManager.
   * Assigns to that task a unique Id on the environment.
   * @param task Pointer to the submitted task.
   */
public:

    OOPObjectId Submit (OOPTask * task);
public:
  /**
   * Executes a daemon task as soon as it is submitted to the current task manager
   * @param task Pointer to the submitted task.
   */
  void ExecuteDaemon(OOPTask * dmt);
  /**
   * Returns the number tasks currently being managed by this data manager
   */
  int NumberOfTasks();
  /**
   * Returns the total number of task on the environment.
   */
  int GlobalNumberOfTasks();
  /**
   * Inserts in the fMessages list an instruction for the cancelation of a Task.
   * @param tag Tag contains the Id of the task which will be canceled.
   */
  void CancelTask (OOPAccessTag & tag);

  /**
   * Transfer Tasks from the Executing tasks list to the Finished tasks list
   */
  void TransferExecutingTasks ();
  /**
   * Transfer the tasks which are in the fSubmittedList to the
   * fTaskList, registering their dependencies
   */
  void TransferSubmittedTasks ();
  /**
   * Indicate to the TaskManager that a given task can execute
   */
  void TransfertoExecutable (const OOPObjectId & taskid);
  /**
   * Triggers the Service Thread method for the OOPar environment.
   */
  void Execute ();
  /**
   * ServiceThread execution method for the TM
   * This method also triggers the services threads for the remaining Managers
   */
  static void * ExecuteMTBlocking (void *data);

  /**
   * Grants access to task identified in the Tag Object
   * The tag represents a Task requiring access to a Data.
   * With this method the Requirement represented in the tag is satisfied.
   */
  void GrantAccess(OOPAccessTag & tag);
  /**
   * The TM ServiceThread goes to sleep when it had completed all its tasks.
   * New tasks submissions should wake the ServiceThread  Up. This is performed calling WakeUpCall()
   * It Posts a sem_post on the ServiceThread semaphore which went to sleep by calling a sem_wait
   * Service thread now sleeps based on a semaphore type
   * Semaphore are used instead of mutex and conditional variables combined.
   * Semaphores avoid deadlocking in the cond_signal, cond_wait, mutex_lock and unlocking
   */
  void WakeUpCall(){
    //sem_post(&fServiceSemaphore);
		fServiceSemaphore->post();
  }
  /**
   * Returns true if the service thread has work to do
   */
  bool KeepRunning();
  /**
   * Puts the TM ServiceThread to sleep since there is nothing else to do.
   * The ServiceThread then needs to be awaken by a call to WakeUpCall()
   */
  void WaitWakeUpCall();
  /**
   * Whenever possible, tasks which can be executed by this method.
   * Complies with the data protection of the TM data structure
   */
  void TriggerTasks();
  /**
   * return the mutex which will synchronize the locking operations
   */
  pthread_mutex_t *Mutex()
  {
	  return &fMutex;
  }


  /**
   * Handles the messages contained on the fMessages list
   */
  void HandleMessages();

private:

	/**
	 * The associated communication manager
	 */
	TPZAutoPointer<OOPCommunicationManager> fCM;

	/**
	 * The associated data manager
	 */
	TPZAutoPointer<OOPDataManager> fDM;

	/**
	 * The autopointer associated with this
	 */
	TPZAutoPointer<OOPTaskManager> fTM;

  /**
   * Max number of threads
   */
  int fNumberOfThreads;
  /**
   * Thread which is the main execution loop of the task manager
   */
  pthread_t fExecuteThread;
  /**
   * Thread which owns the lock
   */
  pthread_t fLockThread;

  /**
   * the mutex object around which we will be locking
   */
  pthread_mutex_t fMutex;

  /**
   * Indicates if TM must continue its processing
   */
  bool fKeepGoing;

  /**
   * Indicates the the TM should not send tasks or not
   */
  bool fStopSending;

  /**
   * The processors which confirmed the stop sending command
   */
  std::set<int> fConfirmedProcessors;

  /**
   * Semaphore for the ServiceThread
   */
  //sem_t fServiceSemaphore;
	boost::interprocess::interprocess_semaphore * fServiceSemaphore;

  /**
   * Generate a unique id number
   */
  OOPObjectId GenerateId ();
  /**
   * Find the task with the given id.
   */
  OOPTask *FindTask (OOPObjectId taskid);	//
  /**
   * reorder the tasks according to their priority
   */
  int fProc;
  /**
   * Counter for number of objects assigned to this manager.
   * Whenever a new task is assigned, fLastCreated is incremented.
   */
  long fLastCreated;
  /**
   * Maximum number of generated Id.
   */
  int fReceiveThreadCreated;
  /**
   * List of tasks which can't be executed yet
   */
  list < OOPTaskControl * >fTaskList;
  /**
   * List of tasks which can be readily executed
   */
  list < OOPTaskControl * >fExecutable;
  /**
   * List of executing tasks
   */
  list < OOPTaskControl * >fExecuting;

  /**
   * List of tasks recently submitted
   */
  list < OOPTask * >fSubmittedList;
  /**
   * Holds a list of messages to the TM
   * The messages are translated in actions to the Tasks on TM
   * The list is composed by pairs of Type and AccessTags.
   * Types can be TMAccessGranted or TMCancelTask
   */
  std::list <std::pair< int, OOPAccessTag> > fMessages;
  /**
   * Translates a GrantAccess message to its necessary action on the TM context
   */
  void ExtractGrantAccessFromTag(const OOPAccessTag & tag);
  /**
   * Translates a CancelTask message to its necessary action on the TM context
   */
  void ExtractCancelTaskFromTag(const OOPAccessTag & tag);
};


//extern OOPTaskManager *TM;
#endif
