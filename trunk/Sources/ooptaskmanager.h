// -*- c++ -*-
#ifndef TTASKMANAGER
#define TTASKMANAGER

#ifndef WIN32
#include <pthread.h>
#endif

#include "ooptask.h"
#include <deque>
#include <vector>
//#include "ooptask.h"
class OOPDataVersion;
//class OOPMReturnType;
//class OOPMDataState;
class OOPSaveable;
class OOPTaskControl;
using namespace std;

class   OOPObjectId;





//class   OOPTMMessageTask;


/**
 * Implements the manager of tasks on the environment.
 * All parallelized task are submitted to environment through the TaskManager.
 * Along with Communication Manager and Data Manager, Task Manager acts as daemon on all nodes
 * present on the environment.
 */
class   OOPTaskManager
{
public:
  void Print(ostream & out);
  /**
   * Checks for the existence for obselete tasks on the task manager list
   */
  void CleanUpTasks();
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
  void    NotifyAccessGranted (const OOPObjectId & TaskId,
			       const OOPMDataDepend & depend,
				   OOPMetaData * objptr);
  /**
     * Notifies the task that a required access was revoked on the data.
   * @param TaskId Id of the task to which the access was granted.
   * @param depend dependency structure including objectid, state and version.
   */
  void    RevokeAccess(const OOPObjectId & TaskId,
			       const OOPMDataDepend & depend);

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
  OOPObjectId Submit (OOPTask * task);
  
  /**
   * Re submits a task to the task manager. It won't call the GenerateId() method
   * Assumes that the task was already given an Id.
   * @param *task Pointer to the task which will be submitted
   * @return Returns the Id of the current task.
   * @since 25/07/2003
   * @author Gustavo C Longhin
   */
  OOPObjectId ReSubmit(OOPTask *task);

  /**
   * Returns the number tasks currently being managed by this data manager
   */
  int     NumberOfTasks ();

  /**
   * Returns the total number of task on the environment.
   */
  int     GlobalNumberOfTasks ();

  /**
   * Changes the priority of the task identified by Id.
   * @param Id Id of the task having its priority changed.
   * @param newpriority New priority assigned to the task.
   */
  int     ChangePriority (OOPObjectId &Id,
			  int newpriority);

  /**
   * Cancels the task.
   * @param taskid Id of task which will be canceled.
   */
  int     CancelTask (OOPObjectId taskid);

  /**
   * Returns 1 if task does exist on the current TM
   * @param taskid Id of the searched task
   */
  int     ExistsTask (OOPObjectId taskid);	// returns 1 if the task
  // exists

  /**
   * Transfer the tasks which are in the fSubmittedList to the
   * fTaskList, registering their dependencies
   */
  void TransferSubmittedTasks();

  /**
   * Transfer the finished tasks to the tasklist if they are recurrent
   * else delete the finished tasks
   */
  void TransferFinishedTasks();

  /**
   * Indicate to the TaskManager that a given task can execute
   */
  void TransfertoExecutable(const OOPObjectId &taskid);
  /**
   * Very important method for the whole OOPar environment.
   * Starts all task which has their data access requests granted from the DM.
   * At least one call to one of the task managers should performed for the OOPar to start.
   */
  void    Execute ();
private:
#ifndef WIN32
  /**
   * Mutual exclusion locks for adding tasks to the submission task list.
   */
  pthread_mutex_t fActOnTaskList;

#endif
  /**
   * Generate a unique id number
   */
  OOPObjectId GenerateId ();
  /**
   * Find the task with the given id.
   */
  OOPTask  *FindTask (OOPObjectId taskid);	// 
  /**
   * reorder the tasks according to their priority
   */
  void    Reschedule ();

  /**
   * Processor where the current object is located
   */
  int     fProc;
  /**
   * Counter for number of objects assigned to this manager.
   * Whenever a new task is assigned, fLastCreated is incremented.
   */
  long    fLastCreated;
  /**
   * Maximum number of generated Id.
   */
  long fMaxId;	
  /**
   * List of tasks which can't be executed yet
   */
  deque <OOPTaskControl *> fTaskList;
	

  /**
   * List of tasks which can be readily executed
   */
  deque< OOPTaskControl *> fExecutable;

  /**
   * List of tasks recently submitted
   */
  deque <OOPTask *> fSubmittedList;

  /**
   * List of finished tasks
   */
  deque <OOPTaskControl *> fFinished;
};

/**
 * Implements a task which will be registered as a daemon
 */
class   OOPTMTask:public OOPDaemonTask
{

  /**
   * Simple constructor
   */
  OOPTMTask (int ProcId);

  /**
   * Returns execution type
   */
  OOPMReturnType Execute ();

};

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
