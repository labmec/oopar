#ifndef TTASKH
#define TTASKH

// -*- c++ -*-


#include "oopsaveable.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
#include <deque>
#include "oopsaveable.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
class OOPSendStorage;
class OOPReceiveStorage;

using namespace std;

class OOPMetaData;

class OOPMDataDepend;

/**
 * Implements a abstract Task on the environment.
 * Any task which are going to parallelized on OOPar must be derived from TTask
 * Since tasks are transmitted through the network TTask is also derived from TSaveable
 * and implements Pack() and Unpack() methods.
 */
class OOPTask : public OOPSaveable
{
public:
	bool AmICompatible();
	
	void Print(ostream & out=cout);
	/**
	 * This method is called when a task terminates its execution
	 * Update information such as data access, ownership etc pending on that task.
	 */
	void TaskFinished();

	/**
	 * Submits to the data manager a access requirement based on information found in fDataDepend.
	 */
	void SubmitDependentData();

	/**
	 * Constructor based on a processor-id
	 * @param Procid Id of processor where the object is being created
	 */
	OOPTask (int Procid);	

	/**
	 * Returns the Id of current object
	 */
	OOPObjectId Id ();	
	
	/**
*	 * Returns the quantity of data to which the current task is dependent on
	 */
	int GetNDependentData();

	/**
	 * Sets the id of current object
	 * @param id Id to be set
	 */
	void SetTaskId (OOPObjectId id)
	{
		fTaskId = id;
	}

	/**
	 * Processor where the task should execute
	 */
	int GetProcID ();	

	/**
	 * Sets the processor Id where the task should be executed
	 * @param proc Processor Id
	 */
	void SetProcID (int proc)
	{
		fProc = proc;
	}


	/**
	 * Changes the task priority
	 * @param newpriority New priority to be set
	 */
	void ChangePriority (int newpriority);	// execute this task

	/**
	 * Returns the priority of the current task
	 */
	int Priority ();
	/**
	* Adds a data dependency to the task. It is assumed that
	* write-access implies that this procedure update the object
	* @param DataId Id of the data to be dependent on
	* @param access Access state on the identified data
	* @param version Version on the identified data
	*/
	void AddDependentData(OOPObjectId &DataId,
			    OOPMDataState access ,
			    OOPDataVersion &version);

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
	* Returns 1 if data accesses are satisfied.
	* Returns 0 if can't execute.
	*/
	int CanExecute ();

	/** 
	* Submits requests to the DataManager to transfer
	* the required objects.
	* Will not check if there are task dependencies.
	*/
	void RequestAccess ();

	/**
	* Release the access requests of the current task
	* maybe the task will be deleted, shipped to another processor, etc;
	*/
	void ReleaseAccessRequests();
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
	* Returns class name
	*/
	virtual char *ClassName ()
	{
		return ("TTask");
	}

	/**
	* Returns true if the object belongs to a class derived from the specified classid
	* @param ClassId Id of inquired super class
	*/
	virtual int DerivedFrom (long Classid);
	/**
	* Returns true if the object belongs to a class derived from the specified class name
	* @param classname Name of inquired super class
	*/
	virtual int DerivedFrom (char *classname);	// a class with name
							// classname
	/**
	* Grant access to an object
	*/
    void GrantAccess(OOPObjectId &id, OOPMDataState st, OOPSaveable *objptr);

	/**
	* Revoke access to an object
	*/
	void RevokeAccess(OOPObjectId &id, OOPMDataState st);	

    /**
     * Returns the recurrence information 
     */
    int IsRecurrent();

    /**
     * Sets the recurrence parameter
     * @param recurrence Holds the recurrence value
     * @since 12/06/2003 
     */
    void SetRecurrence(bool recurrence = true);

protected:

	/**
	* Processor where the task should be executed
	*/
	int fProc;
	/**
	* Id of current task.
	* The Id is assigned to the task after having been submitted.
	*/
	OOPObjectId fTaskId;
	/**
	* List of objects of type MDepend.
	* The current task depends on all entries of the list with an specifi access/version state
	*/
	deque<OOPMDataDepend> fDataDepend;

	/**
	* Priority of current task
	*/
	int fPriority;
private:

    /**
     * Indicates when a task is recurrent.
     * @since 12/06/2003 
     */
    int fIsRecurrent;
protected:

};


/**
 * Implements the dependency relation between a Task a Data
 */
class OOPMDataDepend
{
public:
	void Print(ostream &out = cout);
	/**
	 * Operator overloaded
	 */
	bool operator == (const OOPMDataDepend & dd);
	/**
	 * Copy constructor
	 */
	OOPMDataDepend(const ::OOPMDataDepend & dd);
	/**
	 * Id of the data needed to execute the task
	 */
	OOPObjectId fDataId;	
	/**
	 * Type of access required
	 */
	OOPMDataState fNeed;	
	/**
	 * Version required for the task
	 */
	OOPDataVersion fVersion;	
private:
	/**
	 * Pointer to the required object itself
	 */
	OOPSaveable *fObjPtr;
public:	
	OOPSaveable * ObjPtr();
	void SetObjPtr(OOPSaveable * objptr);
	/**
	 * Empty constructor
	 */
	OOPMDataDepend() {fObjPtr=0;}
	//if fVersion == -1 , there is no version dependency
	/**
	 * Constructor with initial parameters
	 */
	OOPMDataDepend (OOPObjectId &id, 
		   OOPMDataState st,
		   OOPDataVersion &ver);
	/**
	 * Returns 1 if the data is accessible
	 */
	int CanExecute (OOPObjectId TaskId);	
	/**
	 * Request the access to the DataManager
	 * @param TaskId Id of the task requiring the access
	 */
	void RequestAccess (OOPObjectId TaskId);
	/**
	 * Returns access status.
	 * returns 0 if no access
	 * returns the pointer to the task which is accessing the data
	 */
	bool Status() { 
		cout << "Pointer " << fObjPtr << endl;
		return fObjPtr != 0;
		}

};

/**
 * Prototype for an instantaneous task
 */
class OOPDaemonTask : public OOPTask
{

public:

	OOPDaemonTask (int ProcId);

	long ExecTime ();	// always returns 0

	int CanExecute ();	// always returns 1

	virtual long GetClassID ()
	{
		return TDAEMONTASK_ID;
	}

	static OOPSaveable *Restore (OOPReceiveStorage * buf);

//  Apenas para DEBUG.
//  virtual void Work()  { Debug( "\nTSaveable::Work." ); }
//  virtual void Print() { Debug( "  TSaveable::Print." ); }

	virtual char *ClassName ()
	{
		return ("TDaemonTask");
	}

	virtual int DerivedFrom (long Classid);	// returns true if the object
//  belongs to a class which is derived from a class
//  with id classid
	virtual int DerivedFrom (char *classname);	// a class with name
							// classname
};

#endif
