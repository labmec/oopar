#ifndef TDATAH
#define TDATAH

#include "oopdataversion.h"
#include "oopsaveable.h"
//#include "longvec.h"
#include "oopdatamanager.h"
#include "ooppardefs.h"

#include "oopdataversion.h"
#include <deque>
#include <set>
#include "oopsaveable.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
using namespace std;
// NOT IMPLEMENTED :

// If various tasks on a same processor request blocking read access,
// only one task number is registered.
// this implies that the other tasks may have their processing interrupted
// because of the termination of the original task
class OOPDataManager;
extern OOPDataManager *DM;
class OOPDMOwnerTask;

/**
 * Implements functionalities concerning access information on a data.
 * It is sufficient to describe all the possible access requirements from a TTask to any data
 */
class OOPAccessInfo{
public:

    /**
     * Describes the type of access state 
     */
	OOPMDataState fState;

    /**
     * Identifies the TaskId willing to access the data object 
     */
	OOPObjectId fTaskId;

    /**
     * Identifies in which version the task needs the data to be 
     */
	OOPDataVersion fVersion;

    /**
     * Identifies in what processor the access is required 
     */
	long fProc;

    /**
     * Indicates if data is being accessed 
     */
  	int fIsAccessing;

    /**
     * Constructor with initial parameters
	 * @param TaskId Id of task requiring access on the data.
	 * @param st Type of access on the data
	 * @param version Version required on the data
	 * @param proc Processor where the access should occur
     */
	OOPAccessInfo(OOPObjectId &TaskId, OOPMDataState &st, OOPDataVersion &version, long proc){
		fTaskId= TaskId;
		fState = st;
		fVersion = version;
		fProc = proc;
		fIsAccessing = 0;
	}		
	
	/**
	 * Operator equal overloaded
	 */
	OOPAccessInfo & operator = (const OOPAccessInfo & aci){
		fTaskId= aci.fTaskId;
		fState = aci.fState;
		fVersion = aci.fVersion;
		fProc = aci.fProc;
		fIsAccessing = aci.fIsAccessing;
		return *this;
	}
	/**
	 * Copy constructor
	 * @param aci AccessInfo object to be copied
	 */
	OOPAccessInfo(const ::OOPAccessInfo & aci){
		fTaskId= aci.fTaskId;
		fState = aci.fState;
		fVersion = aci.fVersion;
		fProc = aci.fProc;
		fIsAccessing = aci.fIsAccessing;
	}
	void Print(ostream & out = cout){
		out << "Is Accessing ? " << (bool)fIsAccessing << endl;
		out << "Proc " << fProc << endl;
		out << "Data State " << fState << endl;
		out << "TaskId " << endl;
		fTaskId.Print(out);
		out << "Version" << endl;
		fVersion.Print(out);
	}
	
};

/**
 * Implements some sort of tag which must be attatched to any obejct subjected
 * to parallelization on the environment.
 * Such tag contains information concerning data ownership, data version, access
 * state, location and so on
 * The Data Manager is responsible for issuing those tags.
 * @since Jan/2003
 * @author Gustavo Longhin
 */

class OOPMetaData : public OOPSaveable
{
private:
	
	/**
	 * Identifies currente class
	 */
	 long int fClassId;

	
	/**
	 * List of tasks which requires specific access to this data.
	 */
	deque <OOPAccessInfo> fTaskList;
	
// dados:
    /**
    * pointer to the object
    */
	OOPSaveable *fObjPtr;
     /**
      * Id of the object
      */
	OOPObjectId fObjId;
     /**
      * Holds version of data
      */
	OOPDataVersion fVersion;


     /**
      * Processor where the object is located
      */
	int fProc;
	/**
	 * Indicates if the current data is write access mode
	 */
	OOPObjectId fTaskWrite;
	
	/**
	 * Indicates the id of the task with current version access
	 */
	OOPObjectId fTaskVersionAccess;
	
     /**
      * Indicates in which transition state the object is
      */
	OOPMTransitionState fTrans;
     /**
      * Indicates whether the data is under delete request
      */
	int fToDelete;
     /**
      * Number of confirmations pending to get out of the transition state
      */
	int fNumConfirm;
     /**
      * Processors accessing current data for read access.
	  * Whenever the data is under write access, the vector contains only the id of that processor
      */
	vector <long> fAccessProcessors;	  
	
	 
     /**
      * Indicates trace of the data.
      * It is mostly used for debugging purposes
      */

	int fTrace;		


public:
	void Print(ostream & out = cout);
	/**
	 * Accesses the TDataVersion data of the current object actuating on it to
	 * set the required data version.
	 * @param level Level where to acuate
	 * @param cardinality The desired cardinality to be set
	 */
	void SetCardinality(int level, int cardinality);
	/**
	 * Increments the version level seting its cardinality.
	 * @param cardinality Cardinality to be set on new level.
	 */
	void IncrementVersionLevel(int cardinality);
	/**
	 * Decreases the level of the data version subtracting its last level.
	 */
	void DecreaseVersionLevel();

	/**
	 * SetVersion is allowed if the task has read and/or version access
	 */
	void SetVersion(const OOPDataVersion &ver, const OOPObjectId &taskid);
	/**
	 * Sets the Id of current data
	 * @param id Id to be set
	 */
	void SetId(OOPObjectId & id);
	  
	/**
	 * Empty constructor
	 */
	OOPMetaData (){}

     /**
      * Constructor
      * @param *ObPtr Pointer to object TSaveable
      * @param ObjId Id of object
      * @param proc Processor number which owns TData.
      * @param st Indicates status of data.
      */
	  OOPMetaData (OOPSaveable * ObPtr, OOPObjectId & ObjId, int proc, OOPMDataState st =
		 ENoAccess);
		 
	/**
	 * Checks if some task on the task access list is satisfied by the current data state
	 */
	void VerifyAccessRequests();
	/**
	 * Submits a task which requires access on current data.
	 * @param taskId Identifier of the task willing to access current data object.
	 * @param version Version for the required data.
	 * @param access Access state for current data.
	 * @param proc Processor where the access shoud occur
	 */
	void SubmitAccessRequest(OOPObjectId &taskId, OOPDataVersion &version, OOPMDataState access, long proc);
	
	/**
	 * Returns true if current data can be accessed with the desired state
	 * This method is virtually not implemented
	 * it ONLY CHECKS whether the version is compatible
	 * it does NOT CHECK whether there is already a task accessing the data
	 * @param version Desired version for the data
	 * @param access Desired access state for the data
	 */
	bool CanExecute(OOPDataVersion & version, OOPMDataState access);
     /**
      * Returns pointer to the TSaveable object
      */
	OOPSaveable *Ptr ();
     /**
      * Overloading of operator
      */
	OOPSaveable *operator-> ();
     /**
      * Transfer an object based on the parameters.
      * @param ProcId Identifies processor Id
      * @param TaskId Identifies task Id
      * @param AccessRequest Represents actual data state
      * @param version Version of the object.
      */
	int TransferObject (int ProcId, OOPObjectId & TaskId, OOPMDataState AccessRequest,
			    OOPDataVersion & version);
	 /**
      * Takes the appropriate action to satisfy the request. This method is used when the data belongs to this processor.
      * Takes action on an incoming message
      * @param &ms Identifies owner of the task.
      */
	void HandleMessage (OOPDMOwnerTask & ms);
	 /**
      * Issues a request message to enable the requested access. This method is used when the data belongs to a different processor.
      * @param ProcId Identifies processor which owns requested object
      * @param TaskId Identifies task which is to be transfered
      * @param AccessRequest Status of the data being requested
      * @param version Identifies version of the object
      */
	void RequestTransferObject (int ProcId, OOPObjectId & TaskId,
				    OOPMDataState AccessRequest, OOPDataVersion &version);
     /**
      * Takes action to transfer the ownership of this object
      * @param ProcId Identifies processor which owns requested object
      * @param TaskId Identifies task which is to be transfered
      * @param AccessRequest Status of the data being requested
      * @param version Identifies version of the object
      */
	void TransferOwnerShip (OOPObjectId & TaskId, int ProcId,
				OOPMDataState AccessRequest, OOPDataVersion &version);
     /**
      * Returns the access state of this data
      */
	OOPMDataState State ();
     /**
      * Returns the processor to which the object belongs
      */
	int Proc ();
     /**
      * returns 1 if the processor has read access
	  * on the given data
	  * if(Procid == -1) returns the number of processors different than the owner
	  * that have read access
      * @param Procid Identifies processor id
      */

	int HasReadAccess (long Procid );
     /**
      * Returns 1 if the processor has write access
	  * on the given data
      * @param Procid Identifies processor id
      */

	int HasWriteAccess (OOPObjectId &Procid);
     /**
      * Return true if it TaskId has the access no the data
	  * @param TaskId Id of inquired task
	  * @param AccessType Type of access the task should have on the data
	  * @param version Data version the data should be for the taskId and AccessType
      */
	int HasAccess(OOPObjectId & TaskId, OOPMDataState AccessType, OOPDataVersion & version);
    /**
     * Attempts to delete the object pointed to by the object
     * Issues deletion request message
     * @param ObjId Id of object to be deleted
     */
	int DeleteObject (OOPObjectId & ObjId);
	/**
     * Issues a request message for deletion to the owner of the data
	 * waits for delete confirmation and finishes the deletion procedure
	 * returns 0 if the deletion was not concluded
     * @param ObjId Id of object ti be deleted
     */
	void RequestDelete (OOPObjectId & ObjId);
	/**
     * Cancels read access to the object
     * Issues the appropriate messages to the other processors and waits for confirmation
     */
	void CancelReadAccess ();
	/**
     * Changes the access state of the data and notifies the task manager to try the indicated task
     * Grants write access
     * @param TaskId Identifies task which is to be transfered
     * @param st Status of the data being requested
     * @param version Identifies version of the object
     */
	void GrantWriteAccess (OOPObjectId & TaskId, OOPMDataState st, OOPDataVersion & version);
    /**
     * Changes the access state of the data and notifies the task manager to try the indicated task
     * Grants read access
     * @param TaskId Identifies task which is to be transfered
     * @param ProcId Identifies processsor which owns data
     * @param AccessRequest Status of the data being requested
     * @param version Identifies version of the data
     */
	void GrantReadAccess (OOPObjectId TaskId, int ProcId, OOPMDataState st,
			      OOPDataVersion version);
    /**
     * Changes the access state of the data and notifies the task manager to try the indicated task
     * Grants version access
     * @param TaskId Identifies task which is to be transfered
     * @param ProcId Identifies processsor which owns data
     * @param AccessRequest Status of the data being requested
     * @param version Identifies version of the data
     */
	void GrantVersionAccess (OOPObjectId TaskId, int ProcId, OOPMDataState st,
			      OOPDataVersion version);
    /**
     * Communicates, when necessary, the state of access to tasks willing to access the data
     
	void NotifyAccessStates ();*/
    /**
     * Returns the version of the data
     */
	OOPDataVersion Version () const ;
    /**
     * Increments the version by one
	 * calls NotifyAccessStates to update the version
     */
	void IncrementVersion ();
    /**
     * Sets trace on data
     * @param trace Indicates trace to be followed*/
	void SetTrace (int trace);
    /**
     * Traces a message
     * @param ms Owner of the message to be traced.
     */
	void TraceMessage (OOPDMOwnerTask & ms);	//Erico

protected:
    /**
     * Traces a message based on is name.
     * @param message identifies message to be traced
     */
	void TraceMessage (char *message);
      public:
    /**
     * Returns the thread which owns the data
     */
	int Thread ();
	
	/**
	 * Returns current object Id
	 */
	OOPObjectId Id() const;
	/**
	 * It releases the access posted by the Task identified by id.
	 * It erases the id entry from the task list.
	 * If any of the conditions mismatches an error message is issued.
	 * @param id Identifies the task from which the access request should be dropped
	 * @param st The required access state
	 * @param ver The required data version.
	 */
	void ReleaseAccess(OOPObjectId &id, OOPMDataState st, OOPDataVersion & ver);

    /**
     * Requests access on current data according to data version restrictions
     * @param taskId : Id of the task willing to access current data
     * @param version : current data version must be compliant with version 
     */
	 long int GetClassID() { return fClassId;}

};

/*
inline TData::TData(TSaveable *ObPtr, long ObjId, int proc, MDataState st) {
	fObjPtr = ObPtr;
	fObjId = ObjId;
	fProc = proc;
	fState = st;
}
*/

inline OOPSaveable *
OOPMetaData::Ptr ()
{
	return fObjPtr;
}

inline OOPSaveable *
OOPMetaData::operator-> ()
{
	return fObjPtr;
}

/*inline MDataState
TData::State ()
{
	return (MDataState) fAccessProcessors[DM->GetProcID ()];
}
*/


inline int
OOPMetaData::Proc ()
{
	return fProc;
}


inline void
OOPMetaData::SetTrace (int trace)
{				//Erico
	fTrace = trace;
}
#endif
