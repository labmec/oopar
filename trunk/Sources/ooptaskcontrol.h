/**
 * @file
 */
#ifndef OOPTASKCONTROLH
#define OOPTASKCONTROLH
class   OOPTask;



#include "oopaccesstag.h"

/**
 * @brief Class which encapsulates a task object and data dependency structure.
 * @ingroup managertask
 */
/**
 * The main idea is to separate the data dependency structure from the task.
 * As such the task is free to manipulate its internal dependency structure.
 * A task object depends on a TaskControl object for its execution.
 */
class   OOPTaskControl
{
	/**
	 * @brief Pointer to the task object the taskcontrol reffers to.
	 */
	OOPTask *fTask;
	
	/////////////////////////////////////Task Data////////////////////////////////////////
	/**
	 * @brief Store the task identifier since that task is now delete by it own thread
	 */
	OOPObjectId fTaskId;
	
	/**
	 * @brief Store the task classid identifier since that task is now delete by it own thread
	 */
	int fClassId;
	/////////////////////////////////////Task Data////////////////////////////////////////
	
	
	/**
	 * @brief Flag indicating whether the thread was started. \n
	 * Only if this flag is true the thread will have meaningful data
	 */
	int fExecStarted;
	/**
	 * @brief Flag indicating whether the thread finished execution
	 */
	int fExecFinished;
	
	/**
	 * @brief Thread created to execute the task
	 */
	pthread_t fExecutor;
	
public:
	void Print(std::ostream & out);
	/**
	 * @brief Constructor, will initiate the data dependency list with the dependency list of the task
	 */
	OOPTaskControl (OOPTask * task);
	/**
	 * @brief Destructor, will delete the task object is the pointer is not null
	 */
	~OOPTaskControl ();
	/**
	 * @brief Data access method
	 */
	OOPTask *Task ()
	{
		return fTask;
	}
	/**
	 * @brief Zeroes the task pointer
	 */
	void ZeroTask ()
	{
		fTask = 0;
	}
	/* @brief Execute the task in a separate thread */
	void Execute();
	
	/**
	 * @brief Entry point for the execution thread. \n
	 * TaskControl objects triggers the Execute() method of the associated OOPTask * object.
	 * @param tcobject Pointer of the task control to execute.
	 */
	/**
	 * At the end of execution, the TaskControl either submit a new version to the MetaData object,
	 * or, if the PointerIsBeingModified is true, updates the current version to the new version.
	 * All infrastructure for this Version arithmetics is responsibility of the MetaData.
	 */
	static void *ThreadExec(void *tcobject);
	
    /**
	 * @brief returns true if the task started
     */
    int TaskStarted() const
    {
        return fExecStarted;
    }
	
    /**
     * @brief Returns true if the task finished
     */
    int TaskFinished() 
    {
		int res;
		res = fExecFinished;
		return res;
    }
    /**
     * @brief Synchronizes the execution thread with the termination of the TaskControl object
     */
    void Join();
	
    /**
     * @brief Updates versions of dependent MetaData objects.
	 */
	/**
     * If for a given MetaData, its access is WriteAccess type, then version will be incremented, obeying the following rule:
     * - If PointerIsBeingModified, then the current version is itself incremented and this indicates that only this object.
     * has access to that MetaData.
     * - If PointerIsBeingModified is false, then more than one task is accessing the MetaData with WriteAccess, which indicates
     * that a new version submission will be required
     */
    void UpdateVersions();
	
    /**
     * @brief Acess to task data since that task is deleted itself after execute
     */
    OOPObjectId & Id() {return fTaskId;}
	
    int ClassId() {return fClassId;}
	
};

#endif
