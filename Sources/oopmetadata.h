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
#include "tpzautopointer.h"
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
class   OOPMetaData//:public TPZSaveable
{
public:

  virtual ~OOPMetaData();

private:
  /**
   * List of tasks which requires specific access to this data.
   */
  OOPAccessInfoList fAccessList;
  /**
   * pointer to the object
   */
//  TPZAutoPointer<TPZSaveable> fObjPtr;
  
  /**
   * Id of the object
   */
  OOPObjectId fObjId;
  /**
   * Processor where the object is located
   */
  int fProc;
  /**
   * Indicates in which transition state the object is
   */
  OOPMTransitionState fTrans;
  /**
   * Indicates whether the data is under delete request
   */
  int fToDelete;
  /**
   * Indicates trace of the data.
   * It is mostly used for debugging purposes
   */
  int     fTrace;
  bool f_PtrBeingModified;
  std::map<OOPDataVersion, TPZAutoPointer<TPZSaveable> > fAvailableVersions;
public:
  void ClearAllVersions();
  /**
   * MetaData objects holds more than a single version for each Saveable object.
   * Each different version is therefore stored in a map of OOPVersion to Saveable pointer.
   * This Method provides the necessary interface for submitting new versions for a given object
   */
  void SubmitVersion(const OOPDataVersion  & nextversion, TPZAutoPointer <TPZSaveable> NewPtr);
  /**
   * Indicates if the associated Saveable object is being accessed with WriteAccess.
   * Provides information for granting access policies. If necessary a copy of the pointer
   * is provided. Allowing for a more parallel execution.
   */
  bool PointerBeingModified() const;		  
  /**
   * Different levels of printing.
   * Amount of information and layout are modified in each method
   */
  void PrintLog (std::ostream & out = std::cout);
  void Print (std::ostream & out = std::cout);
  void ShortPrint(std::ostream & out = std::cout);
  /**
   * Sets the Id of current data
   * @param id Id to be set
   */
  void    SetId (OOPObjectId & id);
  /**
   * Empty constructor
   */
  OOPMetaData (){}
  /**
   * Constructor
   * @param *ObPtr Pointer to object TSaveable
   * @param ObjId Id of object
   * @param proc Processor number which owns TData.
   */
  OOPMetaData (const OOPObjectId & ObjId,
			     const int proc);
  OOPMetaData (TPZAutoPointer<TPZSaveable> ObPtr, const OOPObjectId & ObjId,
                    const int ProcId, const OOPDataVersion & ver);				 
  virtual int ClassId () const
  {
    return OOPMETADATA_ID;
  }
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
  TPZAutoPointer<TPZSaveable> Ptr (OOPDataVersion & version);
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
  void ReleaseAccess (const OOPObjectId & taskid,
			       const OOPMDataDepend & depend);
  /**
   * Verifies whether the object can grant any type of access
   * @return true if there are types of access which can be granted
   */
  bool CanGrantAccess () const;
};
//template class TPZRestoreClass<OOPMetaData,OOPMETADATA_ID>;
/*
inline TData::TData(TSaveable *ObPtr, long ObjId, int proc, MDataState st) {
	fObjPtr = ObPtr;
	fObjId = ObjId;
	fProc = proc;
	fState = st;
}
*/
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
