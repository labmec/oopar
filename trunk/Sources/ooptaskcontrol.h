// -*- c++ -*-

#ifndef OOPTASKCONTROLH
#define OOPTASKCONTROLH

class   OOPTask;
#include "oopmdatadepend.h"

/**
 * class which encapsulates a task object and data dependency structure
 * The main idea is to separate the data dependency structure from the task
 * As such the task is free to manipulate its internal dependency structure
 */
class   OOPTaskControl
{

	OOPTask *fTask;
	OOPMDataDependList fDepend;

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

};
#endif
