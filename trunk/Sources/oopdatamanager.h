// -*- c++ -*-
#ifndef TDATAMANAGERH
#define TDATAMANAGERH
//#include "savlist.h"
#include "ooppardefs.h"
#include "ooptask.h"
#include "oopmetadata.h"
#include <vector>
#include <map>
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "ooppardefs.h"
#include "ooptask.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
#include <pthread.h>
class TPZSaveable;
class   OOPStorageBuffer;
class   OOPStorageBuffer;
class   OOPSaveable;
using namespace std;
class   OOPMetaData;
class   OOPDMOwnerTask;
class   OOPDMRequestTask;
class   OOPCurrentLocation;
/**
 * Implements all the data management in the OOPar environment.
 * Acts as daemon in all processors which are part of the parallel environment.
 * Any data involved in the parallelization must be submitted to the environment
 * through the Data Manager.
 */
class   OOPDataManager
{
	friend class OOPMetaData;
public:
	void PrintDataQueues(char * msg, ostream & out);
	/**
	 * Used only for testing purposes
	 */
	static void main ();
	/**
	 * Simple destructor
	 */
    ~OOPDataManager ();
	/**
	 * Releases the access request from TaskId on dataId and on the specifieds version and accees state
	 * @param TaskId Identifies the task from which the access shoul be released.
	 * @param dataId Identifies the data to which access is no longer required
	 * @param version Version access request must match release access request.
	 * @param access Access request must match release access request.
	 */
	void    ReleaseAccessRequest (const OOPObjectId & TaskId,
				      const OOPMDataDepend & depend);
	/**
	* Add TaskId to the list of tasks willing to access the dataId object. Along with the taskId, type of 
	* access and data version must also be specified, as well as the processor where the access should occur.
	* @param TaskId Id of the Task willing to access the data.
	* @param dataId Id of the data the task is willing to access.
	* @param version Version the task wants the data to be.
	* @param access Type of access.
	* @param ProcId Id of the processor where the access should occur.
	* @return 1 if the access request is compatible, 0 if not compatible
	*/
	int     SubmitAccessRequest (const OOPObjectId & TaskId,
				     const OOPMDataDepend & depend,
				     const long ProcId);
	/**
	* Add TaskId to the list of tasks willing to access the dataId object. Along with the taskId, type of 
	* access and data version must also be specified. The processor is the current processor
	* @param TaskId Id of the Task willing to access the data.
	* @param depend structure which defines the type of access, version and object id
	* @param ProcId Id of the processor where the access should occur.
	* @return 1 if the access request is compatible, 0 if not compatible
	*/
	int     SubmitAccessRequest (const OOPObjectId & TaskId,
				     const OOPMDataDepend & depend)
	{
		return SubmitAccessRequest (TaskId, depend, fProcessor);
	}
	/**
	* Initialization of the DataManager on the indicated processor
	* @param Procid : Processor where the data manager should be initialized
	*/
	OOPDataManager (int Procid);
	/**
	* Return the processor id which owns the current object
	*/
	int     GetProcID ()
	{
		return fProcessor;
	}
	void SubmitAllObjects();
	/**
	* Register the object on the data manager
	* Submits the pointer to the TSaveable object to the DataManager
	* @param *obj : Pointer to TSaveable object which is to be submited
	* @param trace : Indicates if submited object is traceable or not
	*/
	OOPObjectId SubmitObject (TPZSaveable * obj, int trace = 0);
      private:
	/**
	* Deletes the object from the datastructure
	* @param ObjId : Id of object to be deleted
	*/
	void    DeleteObject (OOPObjectId & ObjId);
      public:
	/**
	* Initiates the process do delete the object
	* follows up on the process to delete it
	* @param ObjId : Id of object to be deleted
	*/
	void    RequestDeleteObject (OOPObjectId & ObjId);
	/**
	* Transfers the object identified by ObjId to the processor identified by
	* ProcessorId.
	* and takes action to satisfy the request but does not follow up on the request
	* @param ObjId : Identifier of the object to be transfered.
	* @param ProcessorId : Identifier of processor destination.
	*/
	void    TransferObject (OOPObjectId & ObjId, int ProcessorId);
	/**
	* Processes the updated access information from the other processors
	* @param *task Pointer to the task which (Undocumented)
	*/
	void    GetUpdate (OOPDMOwnerTask * task);
	/**
	* Processes the requests coming from the other processors
	* @param *task Pointer to task which (Undocumented)
	*/
	void    GetUpdate (OOPDMRequestTask * task);
	/**
	 * Returns true if object is found on the DM list
	 */
	bool    HasObject (OOPObjectId & id);
      private:
	/**
	* Processor where the processor is located.
	*/
	long    fProcessor;
	/**
	* Id of the object
	*/
	OOPObjectId fObjId;
	/**
	* Id of the last created object
	*/
	long    fLastCreated;
	/**
	* Maximum Id number that can be created
	*/
	long    fMaxId;
	/**
	* Collections of objects
	*/
    map < OOPObjectId, OOPMetaData *> fObjects;
	/**
	 * Recently submtted objects
	 * Still not available for the TM.
	 */
	list <OOPMetaData * > fSubmittedObjects;
	/**
	* Generates a new object ID
	*/
	OOPObjectId GenerateId ();
public:
	/**
	* Returns a pointer to the data structure
	* @param ObjId : Id of object which must have its data structure pointer returned
	*/
	OOPMetaData *Data (OOPObjectId ObjId);
    /**
     * Returns the Data Version object of the Meta data identified by Id.
     * Necessary for inquiring the current version of the MetaData object.
     * @param Id Identifies the object to be inquired 
     */
//    OOPDataVersion GetDataVersion( const OOPObjectId & Id);
private:
	/**
	* Mutex for accessing the fSubmittedObjects queue.
	* Prevents simultaneos access to such queue.
	*/
	pthread_mutex_t fDataMutex;

};
/**
 * Implements a task which owns (Undocumented)
 */
class   OOPDMOwnerTask:public OOPDaemonTask
{
      public:
	void LogMeReceived(ostream & out);
	/**
	 * Generates logging information
	 */
	void LogMe(ostream & out);
	OOPMDMOwnerMessageType fType;
	/**
	* Access state associated with this message
	*/
	OOPMDataState fState;
	/**
	* Version of the data
	*/
	OOPDataVersion fVersion;
	/**
	* Holds a pointer to the object when the object data is transferred
	*/
	TPZSaveable *fObjPtr;
	/**
	* Id of processor which originated the message
	*/
	int     fProcOrigin;
	/**
	* Id of object to which this object refers
	*/
	OOPObjectId fObjId;
	/**
	* Used for debugging purposes, keeps track of all data by means of log files.
	*/
	int     fTrace;
	/**
	* Constructor
	* @param t : type of ower task
	* @param proc : processor which owns the message
	*/
	OOPDMOwnerTask (OOPMDMOwnerMessageType t, int proc);
	virtual ~OOPDMOwnerTask();
	virtual OOPMReturnType Execute ();
	virtual long GetClassID ()
	{
		return TDMOWNERTASK_ID;
	}
	virtual int Read (TPZStream * buf);
	static TPZSaveable *Restore (TPZStream * buf);
	virtual int Write (TPZStream * buf);
	// Apenas para DEBUG.
	// virtual void Work() { Debug( "\nTSaveable::Work." ); }
	// virtual void Print() { Debug( " TSaveable::Print." ); }
};
/**
 * Implements a request task (Undocumented)
 */
class   OOPDMRequestTask:public OOPDaemonTask
{
      public:
	void LogMe(ostream & out);
	/**
	 * Id of the processor to which the request applies.
	 */
	int     fProcOrigin;
	/**
	 * Dependency data of the request
	 */
	OOPMDataDepend fDepend;
	/**
	 * Constructor
	 */
	        OOPDMRequestTask (int proc, const OOPMDataDepend & depend);
	        OOPDMRequestTask (const OOPDMRequestTask & task);
	        OOPDMRequestTask ();
	virtual OOPMReturnType Execute ();
	virtual long GetClassID ()
	{
		return TDMREQUESTTASK_ID;
	}
	virtual int Read (TPZStream * buf);
	static TPZSaveable *Restore (TPZStream * buf);
	virtual int Write (TPZStream  * buf);
};
class   OOPCurrentLocation
{
      public:
	OOPObjectId fObjectId;
	long    fProcessor;
	        OOPCurrentLocation (long Processor, OOPObjectId & Id)
	{
		fProcessor = Processor;
		fObjectId = Id;
	}
	OOPCurrentLocation & operator = (const OOPCurrentLocation & loc)
	{
		fObjectId = loc.fObjectId;
		fProcessor = loc.fProcessor;
		return *this;
	}
	OOPCurrentLocation (const OOPCurrentLocation & copy)
	{
		fObjectId = copy.fObjectId;
		fProcessor = copy.fProcessor;
	}
};
extern OOPDataManager *DM;
extern ofstream DataManLog;
extern int GLogMsgCounter;
#endif
