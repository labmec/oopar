// -*- c++ -*-
#ifndef OOPTASKCONTROLH
#define OOPTASKCONTROLH
class   OOPTask;



#include "oopmdatadepend.h"

#ifdef OOP_MPE
#include <map>
#include <set>
class OOPEvtId{
public:
  OOPEvtId();
  int f_EvtStart;
  int f_EvtEnd;
  int f_StateExecuting;
  void Print(ostream & out = cout);  
};
class OOPEvtManager{
public:
  OOPEvtManager();
  void GetEvtIdAndIndex(int & index, OOPEvtId & Evt);
  
  void ReleaseEvtIdIndex(int index);
private:
  set<int> m_Avail;
  set<int> m_Used;
  map<int, OOPEvtId> m_Evts;
};
#endif

/**
 * class which encapsulates a task object and data dependency structure.
 * The main idea is to separate the data dependency structure from the task.
 * As such the task is free to manipulate its internal dependency structure.
 */
class   OOPTaskControl
{
  static OOPEvtManager s_EvtMan;
  OOPEvtId m_EvtId;
  int m_EvtIdIndex;
    /**
     * Identifies an event for the associated OOPTask execution start 
     */
    int m_MPEEvtStart;

    /**
     * Identifies an event for the associated OOPTask execution finish 
     */
    int m_MPEEvtEnd;
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
  * Store the task dependencies since that task is now delete by it own thread
  */
 OOPMDataDependList fDataDepend;

 /**
  * Store the task classid identifier since that task is now delete by it own thread
  */
 int fClassId;
 /////////////////////////////////////Task Data////////////////////////////////////////
  
	/**
	 * List of dependency for the current object.
	 */
	OOPMDataDependList fDepend;
 
   /**
   Flag indicating whether the thread was started
   Only if this flag is true the thread will have meaningful data
   */
   int fExecStarted;
   /**
   Flag indicating whether the thread finished execution
   */
   int fExecFinished;
   
   /**
   Thread created to execute the task
   */
   pthread_t fExecutor;
  
      public:
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
	        OOPMDataDependList & Depend ()
	{
		return fDepend;
	}
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
	void    ZeroTask ()
	{
		fTask = 0;
	}
   /*
   Execute the task in a separate thread
   */
   void Execute();
   
   /**
   Entry point for the execution thread
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

    /*!
        \fn OOPTaskControl::TaskFinished() const
      returns true if the task finished
     */
    int TaskFinished() 
    {
      int res;
      res = fExecFinished;
      return res;
    }
    
    void Join();


    /**
     * Acess to task data since that task is deleted itself after execute
     */
    OOPObjectId & Id() {return fTaskId;}

    int ClassId() {return fClassId;}

    OOPMDataDependList & TaskDepend (){return fDataDepend;}
};
#endif
