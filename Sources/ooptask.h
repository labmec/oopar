// -*- c++ -*-
#ifndef TTASKH
#define TTASKH
#include <deque>
#include "oopsaveable.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "oopmdatadepend.h"
class   OOPSendStorage;
class   OOPReceiveStorage;
using namespace std;
class   OOPMetaData;
class   OOPMDataDepend;
class   OOPMDataDependList;
/**
 * Implements a abstract Task on the environment.
 * Any task which are going to parallelized on OOPar must be derived from TTask
 * Since tasks are transmitted through the network TTask is also derived from TSaveable
 * and implements Pack() and Unpack() methods.
 */
class   OOPTask:public OOPSaveable
{
private:
	bool fIsExecuting;
public:
	void SetExecuting(bool value = true){
		fIsExecuting = value;
	}
	/**
	 * Sets the label property
	 */
	void SetLabel(char * label){
		fLabel = label;
	}
	void SetLabel(string label){
		fLabel = label;
	}
	string GetLabel(){
		return fLabel;
	}

	/**
	 * Prints log information on a log file
	 * @since 16/08/2003
	 * @author Gustavo C Longhin
	 */
	void PrintLog(ostream & out = cout, char * message = "empty log message");
	/**
	 * Returns the reference to the OOPMDataDependList object.
	 * @since 04/07/2003
	 * @author Gustavo C. Longhin
	 */
	OOPMDataDependList & Depend ()
	{
		return fDataDepend;
	}
	void    Print (ostream & out = cout);
	/**
	 * This method is called when a task terminates its execution
	 * Update information such as data access, ownership etc pending on that task.
	 */
	void    TaskFinished ();
	/**
	 * Constructor based on a processor-id
	 * @param Procid Id of processor where the object is being created
	 */
	OOPTask (int Procid);
	OOPTask (const OOPTask & task);
	virtual ~ OOPTask ()
	{
	}
	/**
	 * Returns the Id of current object
	 */
	OOPObjectId Id ();
  /**
   * returns the dependency list of the task
   */
	OOPMDataDependList & GetDependencyList ()
	{
		return fDataDepend;
	}
  /**
   * Sets the dependency list of the task
   */
	void    SetDependencyList (const OOPMDataDependList & deplist)
	{
		fDataDepend = deplist;
	}
  /**
   * Sets the id of current object
   * @param id Id to be set
   */
	void    SetTaskId (OOPObjectId id)
	{
		fTaskId = id;
	}
	/**
	 * Processor where the task should execute
	 */
	int     GetProcID ();
	/**
	 * Sets the processor Id where the task should be executed
	 * @param proc Processor Id
	 */
	void    SetProcID (int proc)
	{
		fProc = proc;
	}
	/**
	 * Changes the task priority
	 * @param newpriority New priority to be set
	 */
	void    ChangePriority (int newpriority);	// execute this task
	/**
	 * Returns the priority of the current task
	 */
	int     Priority ();
	/**
	* Adds a data dependency to the task. It is assumed that
	* write-access implies that this procedure update the object
	* @param depend Dependency which will be appended
	*/
	void    AddDependentData (const OOPMDataDepend & depend);
	/**
	* Returns the estimated execution time.
	* returns 0 if the task is instantaneous
	* returns > 0 if estimate is known
	* return < 0 if no estimate is known
	*/
	virtual long ExecTime ();
	/**
	* Execute the task, verifying that all needed data acesses are satisfied.
	*/
	virtual OOPMReturnType Execute ();
	/**
	* Submits the task to the TM, TaskId is returned.
	*/
	OOPObjectId Submit ();
	/**
	* Returns last created Id.
	*/
	virtual long GetClassID ()
	{
		return TTASK_ID;
	}
	/**
	* Virtual Unpack function.
	* Defines the necessary interface for task communication along the network.
	* @param * buf Buffer for data manipulation.
	*/
	virtual int Unpack (OOPReceiveStorage * buf);
	/**
	* static function defined for the Restore functionality once TTask objects arrives
	* on destination processor
	*/
	static OOPSaveable *Restore (OOPReceiveStorage * buf);
	virtual int Pack (OOPSendStorage * buf);
// Apenas para DEBUG.
//  virtual void Work()  { Debug( "\nTSaveable::Work." ); }
//  virtual void Print() { Debug( "  TSaveable::Print." ); }
    /**
     * Returns the recurrence information 
     */
	int     IsRecurrent ();
    /**
     * Sets the recurrence parameter
     * @param recurrence Holds the recurrence value
     * @since 12/06/2003 
     */
	void    SetRecurrence (bool recurrence = true);
      protected:
	/**
	* Processor where the task should be executed
	*/
	int     fProc;
	/**
	* Id of current task.
	* The Id is assigned to the task after having been submitted.
	*/
	OOPObjectId fTaskId;
	/**
	* List of objects of type MDepend.
	* The current task depends on all entries of the list with an specifi access/version state
	*/
	OOPMDataDependList fDataDepend;
	/**
	* Priority of current task
	*/
	int     fPriority;
      private:
    /**
     * Indicates when a task is recurrent.
     * @since 12/06/2003 
     */
	int     fIsRecurrent;
	/**
	 * Holds a brief description of the task purpose
	 */
	string fLabel;

      protected:
};
/**
 * Prototype for an instantaneous task
 */
class   OOPDaemonTask:public OOPTask
{
      public:
	OOPDaemonTask (int ProcId);
	        OOPDaemonTask (const OOPDaemonTask & task):OOPTask (task)
	{
	}
	long    ExecTime ();	// always returns 0
	int     CanExecute ();	// always returns 1
	virtual long GetClassID ()
	{
		return TDAEMONTASK_ID;
	}
	static OOPSaveable *Restore (OOPReceiveStorage * buf);
};
extern int GLogMsgCounter;
#endif
