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

#include <semaphore.h>

class OOPStorageBuffer;
class OOPDataVersion;
class OOPSaveable;
class OOPTaskControl;
using namespace std;
class OOPObjectId;
class TMLock;


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
   * Indicate to the TaskManager that a given task can execute
   */
  void TransfertoExecutable (const OOPObjectId & taskid);
  /**
   * Main method for the whole OOPar environment.
   * OOPar is TM centered. So the TM::Execute starts the kernel of the oopar environment.
   * Inside Execute the CM as well as the DM service threads are triggered.
   * The termination of the OOPar environment is obtained by terminating the TM Service Thread which is triggered by
   * the Execute Method.
   */
  void Execute ();
  static void *ReceiveMessages (void *data);
  static void * ExecuteMTBlocking (void *data);

  void GrantAccess(OOPAccessTag & tag);
  /**
   * Post the ServiceThread semaphore
   * Service thread now sleeps based on a semaphore type
   * Semaphore are used instead of mutex and conditional variables combined.
   * Semaphores avoid deadlocking in the cond_signal, cond_wait, mutex_lock and unlocking
   */
  void WakeUpCall(){
    sem_post(&fServiceSemaphore);
  }
  /**
   * Returns true if the service thread has work to do
   */
  bool KeepRunning();
  void WaitWakeUpCall();
  void TriggerTasks();

  /**
   * Handles the messages contained on the fMessages list
   */
  void HandleMessages();

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
   * Semaphore for the ServiceThread
   */
  sem_t fServiceSemaphore;

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
  list < OOPTaskControl * >fExecuting;

  /**
   * Holds a list of messages to the TM
   * The messages are translated in actions to the Tasks on TM
   * The list is composed by pairs of Type and AccessTags.
   * Types can be TMAccessGranted or TMCancelTask
   */
  std::list <std::pair< int, OOPAccessTag> > fMessages;
  void ExtractGrantAccessFromTag(const OOPAccessTag & tag);
  void ExtractCancelTaskFromTag(const OOPAccessTag & tag); 
  
  void ExecuteDaemon(OOPTask * dmt);
  void InsertTask(OOPTask * task);
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
