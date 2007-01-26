// -*- c++ -*-
#ifndef TTASKMANAGER
#define TTASKMANAGER
#ifndef WIN32
#include <pthread.h>
#endif
#include "ooptask.h"
#include <list>
#include <set>

#include <semaphore.h>

class OOPStorageBuffer;
class OOPDataVersion;
class OOPSaveable;
class OOPTaskControl;
using namespace std;
class OOPObjectId;
class TMLock;

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
  
  void WakeServiceThread(){
    sem_post(&fServiceSemaphore);
  }
  /**
   * Dumps on disk the current state of the Manager object
   */
  void SnapShotMe();
  /**
   * Set max number of simultaneous threads.
   */
  void SetNumberOfThreads (const int n);

  /**
   * Get max number of simultaneous threads.
   */
  int NumberOfThreads ();

  void Wait ();
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
   * Used for testing purposes.
   * Checks all the public interfaces of the class
   */
  static void main ();
  /**
   * Notifies the task that a required access was granted on the data.
   * @param TaskId Id of the task to which the access was granted.
   * @param depend dependency structure including objectid, state and version.
   * @param objptr Pointer to data object.
   */
  void NotifyAccessGranted (const OOPAccessTag & depend);
  /**
   * Notifies the task that a required access was revoked on the data.
   * @param TaskId Id of the task to which the access was granted.
   * @param depend dependency structure including objectid, state and version.
   */
  void RevokeAccess (const OOPObjectId & TaskId,
		     const OOPAccessTag & depend);
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
protected:
    OOPObjectId SubmitOriginal (OOPTask * task);

    OOPObjectId Submit (OOPTask * task);
public:
  /**
   * Submits a daemon task to the TaskManager.
   * @param task Pointer to the submitted task.
   */
  void SubmitDaemon(OOPDaemonTask * task);
  /**
   * Returns the number tasks currently being managed by this data manager
   */
  int NumberOfTasks();
  /**
   * Returns true if there is a chance of finding an executable task
   */
  bool HasWorkTodo();
  /**
   * Returns the total number of task on the environment.
   */
  int GlobalNumberOfTasks();
  /**
   * Changes the priority of the task identified by Id.
   * @param Id Id of the task having its priority changed.
   * @param newpriority New priority assigned to the task.
   */
  int ChangePriority (OOPObjectId & Id, int newpriority);
  /**
   * Cancels the task.
   * @param taskid Id of task which will be canceled.
   */
  int CancelTask (OOPObjectId taskid);
  /**
   * Returns 1 if task does exist on the current TM
   * @param taskid Id of the searched task
   */
  int ExistsTask (OOPObjectId taskid);	// returns 1 if the

  void TransferExecutingTasks ();
  /**
   * Transfer the tasks which are in the fSubmittedList to the
   * fTaskList, registering their dependencies
   */
  void TransferSubmittedTasks ();
  /**
   * Transfer the finished tasks to the tasklist if they are recurrent
   * else delete the finished tasks
   */
  void TransferFinishedTasks ();
  /**
   * Indicate to the TaskManager that a given task can execute
   */
  void TransfertoExecutable (const OOPObjectId & taskid);
  /**
   * Execute all daemons which are in the list
   */
  void ExecuteDaemons ();
  /**
   * Very important method for the whole OOPar environment.
   * Starts all task which has their data access requests granted from the DM.
   * At least one call to one of the task managers should performed for the OOPar to start.
   */
  void Execute ();
  static void *ReceiveMessages (void *data);
  static void * ExecuteMTBlocking (void *data);

  static void *ExecuteMT (void *data);

  /**
   * This method will grab the fSubmittedMutex
   */
  void Lock (TMLock & obj);
  /**
   * This method will signal the condition variable
   */
  void Signal (TMLock & obj);
  /**
   * This method will release the fSubmittedMutex
   */
  void Unlock (TMLock & obj);
  void GrantAccess(OOPAccessTag & tag);
private:

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
   * Indicates if TM must continue its processing
   */
  bool fKeepGoing;
  /**
   * Mutual exclusion lock for the service thread
   */
  pthread_mutex_t fServiceMutex;
  /**
   * Mutual exclusion lock for the fSubmittedList queue
   */
  pthread_mutex_t fSubmittedMutex;

  /**
  * Condition variable to put the taskmanager thread to sleep
  */
  //pthread_cond_t fExecuteCondition;
  
  sem_t fServiceSemaphore;
  /**
   * The lock object currently holding the mutex
   */
  TMLock *fLock;

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
  void Reschedule ();
  /**
   * Processor where the current object is located
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
    list < OOPTaskControl * >fExecuting;

  /**
   * List of daemon tasks which can be readily executed
   */
    list < OOPDaemonTask * >fDaemon;
  /**
   * List of tasks recently submitted
   */
    list < OOPTask * >fSubmittedList;
  /**
   * List of finished tasks
   */
    list < OOPTaskControl * >fFinished;
};

/**
 * Class which implements a lock on the task manager data structure
 */
class TMLock
{
public:
  /**
   * This method will grab the mutex of the task manager
   */
  TMLock ();

  /**
   * The destructor will release the mutex
   */
  ~TMLock ();

  /**
   * This method will signal the condition of the task manager
   */
  void Signal ();
};

class OOPTerminationTask:public OOPTask
{
public:
  ~OOPTerminationTask ();
  /**
   * Simple constructor
   */
  OOPTerminationTask ()
  {
  }
  OOPTerminationTask (int ProcId);
  OOPTerminationTask (const OOPTerminationTask & term);
  /**
   * Returns execution type
   */
  OOPMReturnType Execute ();
  virtual int ClassId () const
  {
    return TTERMINATIONTASK_ID;
  }
   ;
  void Write (TPZStream & buf, int withclassid);
  void Read (TPZStream & buf, void *context = 0);
  long int ExecTime ();
  static TPZSaveable *Restore (TPZStream & buf, void *context = 0);
};
template class TPZRestoreClass < OOPTerminationTask, TTERMINATIONTASK_ID >;

/*
  enum TTMMessageType {
  ENoTaskMessage, // indicates that the task message was no initialized
  ETaskFinished,	// indicate that the task finished successfully
  ETaskAborted,	// indicate that the task was aborted
  ETaskMoved,		// indicate that the task was moved to a different processor
  ENewTask,		// signal the criation of a new task
  ETaskSubmit,	// indicating a task being submitted
  EAddDependence, // adding a dependence to a task
  EChangePriority	// changing the priority of the task
  };
  class TTMMessageTask : public TDaemonTask {
  // task which will be sent to the other processors when
  //      a task terminated successfully, aborted or was deleted
  public:
  TObjectId fTaskId;				// task id of the task which changed
  int fMessageOrigin;			// processor from which the message originated
  int fMessageDestination;	// processor to which the data is destinated
  int fProcDestination;		// data with reference to the message
  int fProcOrigin;			// data with reference to the message
  int fPriority;				// data with reference to the message
  long fTaskDependence;		// data with reference to EAddDependence
  TTMMessageType fMessage;
  TTask *fTask;				// data with reference to the message
  TTMMessageTask(int ProcId);
  virtual MReturnType Execute();
  virtual long GetClassID() { return TTMMESSAGETASK_ID; }
  virtual int Unpack( TReceiveStorage *buf );
  static TSaveable *Restore(TReceiveStorage *buf);
  
  virtual int Pack( TSendStorage *buf );
  // Apenas para DEBUG.
  //  virtual void Work()  { Debug( "\nTSaveable::Work." ); }
  //  virtual void Print() { Debug( "  TSaveable::Print." ); }
  virtual char *ClassName()    { return( "TTMMessageTask" ); }
  virtual int DerivedFrom(long Classid); // returns  true if the object
  //  belongs to a class which is derived from a class
  //  with id classid
  virtual int DerivedFrom(char *classname); // a class with name classname
  };*/

extern OOPTaskManager *TM;
#endif
