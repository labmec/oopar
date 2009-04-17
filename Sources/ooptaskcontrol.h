// -*- c++ -*-
#ifndef OOPTASKCONTROLH
#define OOPTASKCONTROLH
class   OOPTask;



#include "oopaccesstag.h"
//#include "oopevtmanager.h"
/**
 * Class which encapsulates a task object and data dependency structure.
 * The main idea is to separate the data dependency structure from the task.
 * As such the task is free to manipulate its internal dependency structure.
 * A task object depends on a TaskControl object for its execution.
 */
class   OOPTaskControl
{
#ifdef OOP_MPE
  /**
   * The static attribute for EventID management
   */
//  static OOPEvtManager s_EvtMan;
#endif
  /**
   * Pointer to the task object the taskcontrol reffers to.
   */
  OOPTask *fTask;

 /////////////////////////////////////Task Data////////////////////////////////////////
 /**
  * Store the task identifier since that task is now delete by it own thread
  */
 OOPObjectId fTaskId;

 /**
  * Store the task dependencies since that task is now deleted by its own thread
  */
 //OOPMDataDependList fDataDepend;

 /**
  * Store the task classid identifier since that task is now delete by it own thread
  */
 int fClassId;
 /////////////////////////////////////Task Data////////////////////////////////////////
  
 
  /**
   * Flag indicating whether the thread was started
   * Only if this flag is true the thread will have meaningful data
   */
  int fExecStarted;
  /**
   * Flag indicating whether the thread finished execution
   */
  int fExecFinished;
 
  /**
   *Thread created to execute the task
   */
  pthread_t fExecutor;
  
public:
  void Print(std::ostream & out);
  /**
   * constructor, will initiate the data dependency list with the dependency list of the task
   */
  OOPTaskControl (OOPTask * task);
  /**
   * destructor, will delete the task object is the pointer is not null
   */
  ~OOPTaskControl ();
  /**
   * data access method
   */
  OOPTask *Task ()
  {
    return fTask;
  }
  /**
   * zeroes the task pointer
   */
  void ZeroTask ()
  {
    fTask = 0;
  }
   /*
   Execute the task in a separate thread
   */
   void Execute();
   
  /**
   * Entry point for the execution thread
   * TaskControl objects triggers the Execute() method of the associated OOPTask * object.
   * At the end of execution, the TaskControl either submit a new version to the MetaData object,
   * or, if the PointerIsBeingModified is true, updates the current version to the new version.
   * All infrastructure for this Version arithmetics is responsibility of the MetaData.
   */
  static void *ThreadExec(void *tcobject);

    /*!
        \fn OOPTaskControl::TaskStarted() const
        returns true if the task started
     */
    int TaskStarted() const
    {
        return fExecStarted;
    }

    /**
     * Returns true if the task finished
     */
    int TaskFinished() 
    {
      int res;
      res = fExecFinished;
      return res;
    }
    /**
     * Synchronizes the execution thread with the termination of the TaskControl object
     */
    void Join();

    /**
     * Updates versions of dependent MetaData objects.
     * If for a given MetaData, its access is WriteAccess type, then version will be incremented, obeying the following rule:
     * - If PointerIsBeingModified, then the current version is itself incremented and this indicates that only this object.
     * has access to that MetaData.
     * - If PointerIsBeingModified is false, then more than one task is accessing the MetaData with WriteAccess, which indicates
     * that a new version submission will be required
     */
    void UpdateVersions();

    /**
     * Acess to task data since that task is deleted itself after execute
     */
    OOPObjectId & Id() {return fTaskId;}

    int ClassId() {return fClassId;}

};
#endif
