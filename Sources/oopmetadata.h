// -*- c++ -*-
#ifndef TMETADATAH
#define TMETADATAH
#include "oopdataversion.h"

//#include "longvec.h"
//#include "oopdatamanager.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include <deque>
#include <set>
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "oopaccessinfo.h"
#include "OOPDataLogger.h"
#include "pzsave.h"
using namespace std;
// NOT IMPLEMENTED :
class   OOPDataManager;
extern OOPDataManager *DM;
class   OOPDataLogger;
extern OOPDataLogger *LogDM;
class   OOPDMOwnerTask;
/**
 * Implements some sort of tag which must be attatched to any obejct subjected
 * to parallelization on the environment.
 * Such tag contains information concerning data ownership, data version, access
 * state, location and so on
 * The Data Manager is responsible for issuing those tags.
 * @since Jan/2003
 * @author Gustavo Longhin
 */
class   OOPMetaData:public TPZSaveable
{
public:
	virtual ~OOPMetaData(){}		 
      private:
	/**
	 * List of tasks which requires specific access to this data.
	 */
	OOPAccessInfoList fAccessList;
// dados:
    /**
    * pointer to the object
    */
	TPZSaveable *fObjPtr;
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
	int     fProc;
	/**
	 * Indicates if the current data is write access mode
	 */
	OOPObjectId fTaskWrite;
	/**
	 * Indicates the processor with current version access
	 * If no processor accesses the object, its value == -1
	 * If the value != -1
	 * If fProcVersionAccess == this processor, the processor can grant version access
	 * If fProcVersionAccess != processor request, access to that processor has been granted????
	 */
	int     fProcVersionAccess;
	/**
	 * Indicates the id of the task which has version access
	 */
	OOPObjectId fTaskVersion;
     /**
      * Indicates in which transition state the object is
      */
	OOPMTransitionState fTrans;
     /**
      * Indicates whether the data is under delete request
      */
	int     fToDelete;
     /**
      * Processors accessing current data for read access.
	  * 
	  * Whenever the data has read access, the vector contains only the id of that processor
      */
	        list < int >fReadAccessProcessors;
     /**
      * Processors accessing current data with access.
	  * 
	  * Whenever the data has suspended read access, the vector contains only the id of that processor
      */
	        list < int >fSuspendAccessProcessors;
     /**
      * Indicates trace of the data.
      * It is mostly used for debugging purposes
      */
	int     fTrace;
      public:
		  
	void PrintLog (ostream & out = cout);
	void    Print (ostream & out = cout);
	/**
	 * SetVersion is allowed if the task has read and/or version access
	 */
	void    SetVersion (const OOPDataVersion & ver,
			    const OOPObjectId & taskid);
	/**
	 * Sets the Id of current data
	 * @param id Id to be set
	 */
	void    SetId (OOPObjectId & id);
	/**
	 * Empty constructor
	 */
	        OOPMetaData ();
     /**
      * Constructor
      * @param *ObPtr Pointer to object TSaveable
      * @param ObjId Id of object
      * @param proc Processor number which owns TData.
      */
	        OOPMetaData (TPZSaveable * ObPtr, const OOPObjectId & ObjId,
			     const int proc);
	virtual long GetClassID ();
	/**
	 * Checks if some task on the task access list is satisfied by the current data state
	 */
	void    VerifyAccessRequests ();
      private:
	/**
	 * Verifies whether the transition state of the object can
	 * not be adjusted
	 */
	void    CheckTransitionState ();
      public:
	/**
	 * Submits a task which requires access on current data.
	 * @param taskId Identifier of the task willing to access current data object.
	 * @param depend dependency type requested.
	 * @param processor processor for which access is requested
	 */
	void    SubmitAccessRequest (const OOPObjectId & taskId,
				     const OOPMDataDepend & depend,
				     int processor);
      private:
	/**
	 * The access request is sent to the owning processor if it cannot
	 * be honoured on the local processor
	 */
	void    SendAccessRequest (const OOPMDataDepend & depend);
      public:
	/**
	 * Signals the object that the task is going into execution or not
	 */
	void    SetExecute (const OOPObjectId & taskId,
			    const OOPMDataDepend & depend, bool condition);
     /**
      * Returns pointer to the TSaveable object
      */
	TPZSaveable *Ptr ();
     /**
      * Overloading of operator
      */
	TPZSaveable *operator-> ();
  /**
   * Transfer an object based on the parameters.
   * @param ProcId Identifies processor Id
   */
	void    TransferObject (int ProcId);
	 /**
      * Takes the appropriate action to satisfy the request. This method is used when the data belongs to this processor.
      * Takes action on an incoming message
      * @param &ms Identifies owner of the task.
      */
	void    HandleMessage (OOPDMOwnerTask & ms);
     /**
      * Returns the access state of this data
      */
	OOPMDataState State () const;
     /**
      * Returns the processor to which the object belongs
      */
	int     Proc () const;
	/**
	 * returns true if the current processor is owner of the object
	 */
	bool    IamOwner () const;
     /**
      * returns 1 if any processor has read access
	  * on the given data
      */
	bool    HasReadAccess () const;
     /**
      * returns 1 if the processor has read access
	  * on the given data
      * @param Procid Identifies processor id
      */
	bool    HasReadAccess (const int Procid) const;
	/**
	 * Indicates whether the object can be accessed for version
	 */
	bool    HasVersionAccess () const;
	/**
	 * Indicates whether the version access has been granted to a task
	 */
	bool    HasVersionAccessTask () const
	{
		return !fTaskVersion.IsZeroOOP();
	}
	/**
	 * Indicates whether a task has write access to the object
	 */
	bool    HasWriteAccessTask () const
	{
		return !fTaskWrite.IsZeroOOP();
	}
	/**
	 * Indicates whether the task has write access to the data
	 */
	bool    HasWriteAccess (const OOPObjectId & taskid) const;
    /**
     * Attempts to delete the object pointed to by the object
     * Issues deletion request message
     * @param ObjId Id of object to be deleted
     */
	void    DeleteObject ();
	/**
     * Issues a request message for deletion to the owner of the data
	 * waits for delete confirmation and finishes the deletion procedure
     */
	void    RequestDelete ();
	/**
     * Cancels read access to the object
     * Issues the appropriate messages to the other processors and waits for confirmation
     */
	void    CancelReadAccess ();
	/**
	 * Suspends the read access to the objects
	 * Issues the appropriate messages to the other processors
	 */
	void    SuspendReadAccess ();
	/**
	 * Sends a TDMOwnerTask granting the access state to the processor
	 */
	void    GrantAccess (OOPMDataState state, int processor);
	/**
     * Changes the access state of the data and notifies the task manager to try the indicated task
     * Grants write access
     * @param TaskId Identifies task which is to be transfered
     * @param st Status of the data being requested
     * @param version Identifies version of the object
     */
	void    GrantWriteAccess (OOPObjectId & TaskId, OOPMDataState st,
				  OOPDataVersion & version);
    /**
     * Changes the access state of the data and notifies the task manager to try the indicated task
     * Grants read access
     * @param TaskId Identifies task which is to be transfered
     * @param ProcId Identifies processsor which owns data
     * @param AccessRequest Status of the data being requested
     * @param version Identifies version of the data
     */
	void    GrantReadAccess (OOPObjectId TaskId, int ProcId,
				 OOPMDataState st, OOPDataVersion version);
    /**
     * Changes the access state of the data and notifies the task manager to try the indicated task
     * Grants version access
     * @param TaskId Identifies task which is to be transfered
     * @param ProcId Identifies processsor which owns data
     * @param AccessRequest Status of the data being requested
     * @param version Identifies version of the data
     */
	void    GrantVersionAccess (OOPObjectId TaskId, int ProcId,
				    OOPMDataState st, OOPDataVersion version);
    /**
     * Communicates, when necessary, the state of access to tasks willing to access the data
     
	void NotifyAccessStates ();*/
    /**
     * Returns the version of the data
     */
	OOPDataVersion Version () const;
    /**
     * Sets trace on data
     * @param trace Indicates trace to be followed*/
	void    SetTrace (int trace);
    /**
     * Traces a message
     * @param ms Owner of the message to be traced.
     */
	void    TraceMessage (OOPDMOwnerTask & ms);	// Erico
      protected:
    /**
     * Traces a message based on is name.
     * @param message identifies message to be traced
     */
	void    TraceMessage (char *message);
      public:
	/**
	 * Returns current object Id
	 */
	        OOPObjectId Id () const;
	/**
	 * It releases the access posted by the Task identified by id.
	 * It erases the id entry from the task list.
	 * If any of the conditions mismatches an error message is issued.
	 * @param taskid Identifies the task from which the access request should be dropped
	 * @param depend Identifies the type of dependency
	 */
	void    ReleaseAccess (const OOPObjectId & taskid,
			       const OOPMDataDepend & depend);
	 /**
	  * Verifies whether the object can grant any type of access
	  * @return true if there are types of access which can be granted
	  */
	bool    CanGrantAccess () const;
};
/**
 *log file for MetaData objects
 */
extern ofstream DataLog;
extern ofstream TransferDataLog;
extern int GLogMsgCounter;
/*
inline TData::TData(TSaveable *ObPtr, long ObjId, int proc, MDataState st) {
	fObjPtr = ObPtr;
	fObjId = ObjId;
	fProc = proc;
	fState = st;
}
*/
inline  TPZSaveable *OOPMetaData::Ptr ()
{
	return fObjPtr;
}
inline TPZSaveable *OOPMetaData::operator-> ()
{
	return fObjPtr;
}
/*inline MDataState
TData::State ()
{
	return (MDataState) fAccessProcessors[DM->GetProcID ()];
}
*/
     inline int OOPMetaData::Proc () const
     {
	     return fProc;
     }
     inline void OOPMetaData::SetTrace (int trace)
{      // Erico
	fTrace = trace;
}
#endif
