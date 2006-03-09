// -*- c++ -*-
#ifndef OOPTASKCONTROLH
#define OOPTASKCONTROLH
class   OOPTask;
#include "oopmdatadepend.h"
/**
 * class which encapsulates a task object and data dependency structure.
 * The main idea is to separate the data dependency structure from the task.
 * As such the task is free to manipulate its internal dependency structure.
 */
class   OOPTaskControl
{
	/**
	 * Pointer to the task object the taskcontrol reffers to.
	 */
	OOPTask *fTask;
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
};
#endif
