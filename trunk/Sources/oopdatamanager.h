// -*- c++ -*-

#ifndef TDATAMANAGERH
#define TDATAMANAGERH


//#include "savlist.h"
#include "ooppardefs.h"
#include "ooptask.h"
#include "oopmetadata.h"
#include <vector>
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "ooppardefs.h"
#include "ooptask.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
class OOPSendStorage;
class OOPReceiveStorage;


class OOPSaveable;


using namespace std;

class OOPMetaData;
class OOPDMOwnerTask;
class OOPDMRequestTask;
class OOPCurrentLocation;

/**
 * Implements all the data management in the OOPar environment.
 * Acts as daemon in all processors which are part of the parallel environment.
 * Any data involved in the parallelization must be submitted to the environment through the Data Manager.
 */
class OOPDataManager {

public:
	/**
	 * Used only for testing purposes
	 */
	static void main();
	
	~OOPDataManager();
	
	/**
	 * Releases the access request from TaskId on dataId and on the specifieds version and accees state
	 * @param TaskId Identifies the task from which the access shoul be released.
	 * @param dataId Identifies the data to which access is no longer required
	 * @param version Version access request must match release access request.
	 * @param access Access request must match release access request.
	 */
	void ReleaseAccessRequest(const OOPObjectId & TaskId,const OOPMDataDepend &depend);
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
	int SubmitAccessRequest(const OOPObjectId & TaskId,const  OOPMDataDepend &depend,const long ProcId);
	/**
	* Add TaskId to the list of tasks willing to access the dataId object. Along with the taskId, type of 
	* access and data version must also be specified. The processor is the current processor
	* @param TaskId Id of the Task willing to access the data.
	* @param depend structure which defines the type of access, version and object id
	* @param ProcId Id of the processor where the access should occur.
	* @return 1 if the access request is compatible, 0 if not compatible
	*/
	int SubmitAccessRequest(const OOPObjectId & TaskId,const  OOPMDataDepend &depend) {
	  return SubmitAccessRequest(TaskId,depend,fProcessor);
	}
	/**
	* Initialization of the DataManager on the indicated processor
	* @param Procid : Processor where the data manager should be initialized
	*/
	OOPDataManager(int Procid);
	
	/**
	* Return the processor id which owns the current object
	*/
	int GetProcID() { return fProcessor;}
	/**
	* Register the object on the data manager
	* Submits the pointer to the TSaveable object to the DataManager
	* @param *obj : Pointer to TSaveable object which is to be submited
	* @param trace : Indicates if submited object is traceable or not
	*/
	OOPObjectId SubmitObject(OOPSaveable *obj, int trace = 0);
	/**
	* Initiates the process do delete the object
	* follows up on the process to delete it
	* @param ObjId : Id of object to be deleted
	*/
	int DeleteObject(OOPObjectId & ObjId);
	/**
	* Transfers the object identified by ObjId to the processor identified by
	* ProcessorId.
	* and takes action to satisfy the request but does not follow up on the request
	* @param ObjId : Identifier of the object to be transfered.
	* @param ProcessorId : Identifier of processor destination.
	*/
	
	void TransferObject(OOPObjectId & ObjId, int ProcessorId);
	
	/**
	* Processes the updated access information from the other processors
	* @param *task Pointer to the task which (Undocumented)
	*/
	void GetUpdate(OOPDMOwnerTask *task);
	/**
	* Processes the requests coming from the other processors
	* @param *task Pointer to task which (Undocumented)
	*/
	void GetUpdate(OOPDMRequestTask *task);
	
	
	/**
	 * Returns true if object is found on the DM list
	 */
	bool HasObject(OOPObjectId id);

private:
	/**
	* Processor where the processor is located.
	*/
	long fProcessor;
	/**
	* Id of the object
	*/
	OOPObjectId fObjId;
	/**
	* Id of the last created object
	*/
	long fLastCreated;
	/**
	* Maximum Id number that can be created
	*/
	long fMaxId;
	
	/**
	* Collections of objects
	*/
	deque <OOPMetaData * > fObjects;
	
	/**
	* Generates a new object ID
	*/
	OOPObjectId GenerateId();
	/**
	* Returns a pointer to the data structure
	* @param ObjId : Id of object which must have its data structure pointer returned
	*/
	OOPMetaData *Data(OOPObjectId ObjId);
	
public:

    /**
     * Returns the Data Version object of the Meta data identified by Id.
     * Necessary for inquiring the current version of the MetaData object.
     * @param Id Identifies the object to be inquired 
     */
    OOPDataVersion GetDataVersion(OOPObjectId & Id);

private:

};

/**
 * Implements a task which owns (Undocumented)
 */
class OOPDMOwnerTask : public OOPDaemonTask {

public:

	OOPMDMOwnerMessageType fType;
	/**
	* Access state associated with this message
	*/
	OOPMDataState	fState;
	/**
	* Version of the data
	*/
	OOPDataVersion fVersion;

	/**
	* Holds a pointer to the object when the object data is transferred
	*/
	OOPSaveable *fObjPtr;
	/**
	* Id of task which originated this message
	*/
	OOPObjectId fTaskId;
	/**
	* Id of processor for which this message is intended
	* If Id==-1 all the processor must be notified (Multi-Cast)
	*/
	int fProcDestination;
	/**
	* Id of processor which originated the message
	*/
	int fProcOrigin;
	/**
	* Id of object to which this object refers
	*/
	OOPObjectId fObjId;
	/**
	* List of processors with access state for updating
	*/
	vector<long> fAccessProcessors;
	/**
	* Used for debugging purposes, keeps track of all data by means of log files.
	*/
	int fTrace;
	/**
	* Constructor
	* @param t : type of ower task
	* @param proc : processor which owns the message
	*/
	OOPDMOwnerTask(OOPMDMOwnerMessageType t, int proc);
	
	virtual OOPMReturnType Execute();
	
	virtual long GetClassID() { return TDMOWNERTASK_ID; }
	
	virtual int Unpack( OOPReceiveStorage *buf );
	
	static OOPSaveable *Restore(OOPReceiveStorage *buf);
	
	virtual int Pack( OOPSendStorage *buf );
	
	// Apenas para DEBUG.
	//  virtual void Work()  { Debug( "\nTSaveable::Work." ); }
	//  virtual void Print() { Debug( "  TSaveable::Print." ); }
	
	virtual char *ClassName()    { return( "TDMOwnerTask" ); }
	/**
	* Returns  true if the object
	* belongs to a class which is derived from a class
	* with id classid
	*/
	virtual int DerivedFrom(long Classid);
	/**
	* Returns  true if the object
	* belongs to a class which is derived from a class
	* with name classname
	*/
	virtual int DerivedFrom(char *classname);
	
	
};

/**
 * Implements a request task (Undocumented)
 */
class OOPDMRequestTask : public OOPDaemonTask {

public:
	/**
	 * Returns the type of message request.
	 */
	OOPDMRequestMessageType fType;
	/**
	 * Access state associated with this message.
	 */
	OOPMDataState	fAccessState;	
	
	/**
	 * Version associated with this message.
	 */
	OOPDataVersion fVersion;
	/**
	 * Id of the task which originated this message.
	 */
	OOPObjectId fTaskId;
	/**
	 * Id of the processor for which this message is intended.
	 */
	 
	int fProcDestination;
	/**
	 * Id of the processor to which the request applies.
	 */
	int fProcOrigin;	
	/**
	 * Id of the object to which this object refers.
	 */
	OOPObjectId fObjId;		
	/**
	 * Constructor
	 */
	OOPDMRequestTask(OOPDMRequestMessageType t, int proc);
	
	virtual OOPMReturnType Execute();
	
	virtual long GetClassID() { return TDMREQUESTTASK_ID; }
	
	virtual int Unpack( OOPReceiveStorage *buf );
	
	static OOPSaveable *Restore(OOPReceiveStorage *buf);
	
	virtual int Pack( OOPSendStorage *buf );
	
	// Apenas para DEBUG.
	//  virtual void Work()  { Debug( "\nTSaveable::Work." ); }
	//  virtual void Print() { Debug( "  TSaveable::Print." ); }
	
	virtual char *ClassName()    { return( "TDMRequestTask" ); }
	
	virtual int DerivedFrom(long Classid); // returns  true if the object
	/**
	 * Belongs to a class which is derived from a class with id classid.
	 */
	virtual int DerivedFrom(char *classname); // a class with name classname

};

class OOPCurrentLocation{
public:
	OOPObjectId fObjectId;
	long fProcessor;

	OOPCurrentLocation(long Processor, OOPObjectId & Id){
		fProcessor = Processor;
		fObjectId = Id;
	}

	OOPCurrentLocation & operator = (const OOPCurrentLocation & loc){
		fObjectId = loc.fObjectId;
		fProcessor = loc.fProcessor;
		return *this;
	}
	OOPCurrentLocation(const OOPCurrentLocation & copy){
		fObjectId = copy.fObjectId;
		fProcessor = copy.fProcessor;
	}
};


extern OOPDataManager *DM;
#endif
