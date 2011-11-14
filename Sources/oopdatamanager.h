
#ifndef TDATAMANAGERH
#define TDATAMANAGERH

#include "ooppardefs.h"
#include "ooptask.h"
#include "oopmetadata.h"
#include <vector>
#include <map>
#include <pthread.h>

#include "oopdataversion.h"
#include "oopobjectid.h"
#include "ooppardefs.h"
#include "ooptask.h"
#include "oopdataversion.h"
#include "oopobjectid.h"

#include "tpzautopointer.h"
#include "pzsave.h"
#include "pzstream.h"

#include "boost/interprocess/sync/interprocess_semaphore.hpp"


class TPZSaveable;
class   OOPStorageBuffer;
using namespace std;
class   OOPDMOwnerTask;
class   OOPDMRequestTask;
class   OOPCurrentLocation;
class OOPTaskManager;

/**
 * @brief Identifies possible messages submitted to the DataManager. \n
 * A brief description of each type are also available.
 * @ingroup managerdata
 */
enum DMMessageType {
	/**
	 * @brief Identifies a message which carries a Data object along with the OOPAccessTag.
	 * This message will be translated on the creation of a OOPMetaData object with a AutoPointer valid on the TPZSaveable object.
	 */
	EDMData,
	
	/**
	 * @brief Identifies a message which will be translated in the OOPDMOwnerMessage.
	 */
	EDMOwner,
	
	/**
	 * @brief Identifies a message which will generate a AccessRequest posted on the current processor
	 */
	EDMRequest,
	
	/**
	 * @brief Identifies a message which will be translated on AccessRequest task submitted to a different processor. Foreign access request.
	 */
	EDMForeignRequest
};


/**
 * @brief Implements all the data management in the OOPar environment.
 * @ingroup managerdata
 */
/**
 * Acts as daemon in all processors which are part of the parallel environment.
 * Any data involved in the parallelization must be submitted to the environment
 * through the Data Manager.
 */
class   OOPDataManager
{
public:
	/**
	 * @brief Initialization of the DataManager on the indicated processor
	 * @param Procid : Processor where the data manager should be initialized
	 */
	OOPDataManager (int Procid, TPZAutoPointer<OOPTaskManager> TM);
	/**
	 * @brief Simple destructor
	 */
	~OOPDataManager ();
	
	/**
	 * @brief The autopointer to the task manager with which the data manager relates
	 */
	TPZAutoPointer<OOPTaskManager> TM();
	
	/**
	 * @brief The autopointer to the data manager corresponding to himself
	 */
	TPZAutoPointer<OOPDataManager> DM();
	/**
	 * @brief Clear the pointer so the object can be deleted
	 */
	void ClearPointer();
	/**
	 * @brief Terminate the execution thread
	 */
	void JoinThread();
	
	/**
	 * @brief Encapsulates the call for SubmittAllObjects.
	 * Only for minemonic reasons
	 */
	void HandleMessages();
	/**
	 * @brief Inserts a ObjectChanged entry on the fChangedObject list
	 */
	void ObjectChanged(const OOPObjectId & Id);
	/**
	 * @brief Dumps on disk the current state of the Manager object
	 */
	void SnapShotMe(std::ostream & out);
	
	/**
	 * @brief Checks the list of available objects since a new version had been submitted
	 */
	void ObjectChanged(std::set<OOPObjectId> & set);
	/**
	 * @brief The following three methods appends objects to the fMessages list. \n
	 * Each of them submits messages of a different type.
	 * PostData appends a changed Saveable object,it differs from SubmitObject which returns a ObjectId.
	 * In this case the Object identified in the Tag already has an Id assigned to it.
	 */
	void PostData(OOPAccessTag & tag);
	/**
	 * @brief SubmitOwnerMessage appends a OwnerMessage to the list. OwnerMessages are basically instructions to the DM concerning
	 * some data. Later on the DM perform the required action.
	 */
	void PostOwnerMessage(OOPAccessTag &tag);
	/**
	 * @brief Submits access requests to the objects maintained by the DM. \n
	 * The object to which the AccessRequest is related to is identified on the Tag parameter
	 */
	void PostAccessRequest(OOPAccessTag & depend);
	void PostForeignAccessRequest(OOPAccessTag & depend);
	/**
	 * @brief Returns the processor id which owns the current object.
	 */
	int     GetProcID ()
	{
		return fProcessor;
	}
	/**
	 * @brief Register the object on the data manager. \n
	 * Submits the pointer to the TSaveable object to the DataManager
	 * @param *obj : Pointer to TSaveable object which is to be submited
	 */
	OOPObjectId SubmitObject (TPZSaveable * obj);
	/**
	 * @brief Moves all Posted objects from the fMessages list to the fObjects list. \n
	 * It actually processes messages translating them in actions.
	 */
	void SubmitAllObjects();
	void ExtractObjectFromTag(OOPAccessTag & tag);
	void ExtractOwnerTaskFromTag(OOPAccessTag & tag);
	void ExtractRequestFromTag(OOPAccessTag & tag);
	void ExtractForeignRequestFromTag(OOPAccessTag & tag);
	/**
	 * @brief Processes the updated access information from the other processors
	 * @param *task Pointer to the task which (Undocumented)
	 */
	void    GetUpdate (OOPDMOwnerTask * task);
	/**
	 * @brief Processes the requests coming from the other processors
	 * @param *task Pointer to task which (Undocumented)
	 */
	void    GetUpdate (OOPDMRequestTask * task);
	
	/**
	 * @brief Verifies access requests for OOPMetaData objects stores fObjects
	 */
	void FlushData();
	void WaitWakeUpCall()
	{
		fServiceSemaphore->wait();
	}
	void WakeUpCall()
	{
		fServiceSemaphore->post();
	}
	void SetKeepGoing(bool go);
	int StartService();
	void PostRequestDelete(OOPObjectId & Id);
	void RequestDelete(OOPObjectId & Id);
	
	/**
	 * @brief Joins the DM service thread
	 */
	void Wait();
	
	/**
	 * @brief Returns the mutex which will synchronize the locking operations
	 */
	pthread_mutex_t *Mutex()
	{
		return &fMutex;
	}
	
	
private:
	pthread_t fServiceThread;
	/**
	 * @brief Service thread execution method for the DM
	 */
	static void * ServiceThread(void * data);
	/**
	 * @brief Semaphore for the DM service thread
	 */
	boost::interprocess::interprocess_semaphore * fServiceSemaphore;
	
	/**
	 * @brief The mutex object around which we will be locking
	 */
	pthread_mutex_t fMutex;
	
	/**
	 * @brief Indicates wether ServiceThread should keep running
	 */
	bool fKeepGoing;
	/**
	 * @brief Processor where the processor is located.
	 */
	long    fProcessor;
	/**
	 * @brief Id of the object
	 */
	OOPObjectId fObjId;
	/**
	 * @brief Id of the last created object
	 */
	long    fLastCreated;
	/**
	 * @brief The taskmanager with which this datamanager relates
	 */
	TPZAutoPointer<OOPTaskManager> fTM;
	
	/**
	 * @brief The datamanager autopointer with which this datamanager relates
	 */
	TPZAutoPointer<OOPDataManager> fDM;
	
	/**
	 * @brief Collections of objects
	 */
	map < OOPObjectId, OOPMetaData > fObjects;
	/**
	 * @brief Holds MetaData objects which had been changed. \n
	 * DM receives those object at the end of a task execution
	 */
	set <OOPObjectId> fChangedObjects;
	/**
	 * @brief Generates a new object ID
	 */
	OOPObjectId GenerateId ();
public:
	/**
	 * @brief Returns a pointer to the data structure
	 * @param ObjId : Id of object which must have its data structure pointer returned
	 */
private:
	OOPMetaData Data (OOPObjectId ObjId);
	/**
	 * @brief Returns the Data Version object of the Meta data identified by Id. \n
	 * Necessary for inquiring the current version of the MetaData object.
	 * @param Id Identifies the object to be inquired
	 */
	//    OOPDataVersion GetDataVersion( const OOPObjectId & Id);
	
	
	std::list< std::pair<int, OOPAccessTag> > fMessages;
	
};

/**
 * @brief Implements a task which owns (Undocumented)
 * @ingroup task
 */
class   OOPDMOwnerTask:public OOPDaemonTask
{
public:
	void LogMe(std::ostream & out);
	void LogMeReceived(std::ostream & out);
	
	/** @brief Data associated with the owner task */
	/** @note The possible values are: \n
	 * fAccessMode : whether the owner task is granting read or write access \n
	 * fObjectAutoPtr : pointer to the data \n
	 * fObjectId : id of the object associated with the owner task \n
	 * fProcessor : processor which originated the owner task \n
	 * fTaskId : not used \n
	 * fVersion : version of the object being transmitted
	 */
	OOPAccessTag fTag;
	
	/**
	 * @brief Set of access requests that need to be submitted when changing ownership
	 */
	std::set<OOPAccessTag> fTransferRequests;
	
	/**
	 * @brief Constructor
	 */
	OOPDMOwnerTask ();
	
	/**
	 * @brief Constructor
	 * @param tag Type of ower task
	 */
	OOPDMOwnerTask (OOPAccessTag &tag): OOPDaemonTask(tag.Proc()), fTag(tag)
	{
	}
	
	/**
	 * @brief Constructor
	 * @param tag Type of ower task
	 */
	OOPDMOwnerTask (OOPAccessTag &tag, std::set<OOPAccessTag> &requests) :
	OOPDaemonTask(tag.Proc()), fTag(tag),fTransferRequests(requests)
	{
	}
	
	virtual ~OOPDMOwnerTask();
	virtual OOPMReturnType Execute ();
	virtual int ClassId () const
	{
		return TDMOWNERTASK_ID;
	}
	virtual void Read (TPZStream & buf, void * context);
	
	virtual void Write (TPZStream & buf, int withclassid);
};


/**
 * @brief Implements a request task (Undocumented)
 * @ingroup managerdata
 */
class OOPDMRequestTask : public OOPDaemonTask
{
public:
	void LogMe(std::ostream & out);
	/**
	 * @brief Dependency data of the request
	 */
	OOPAccessTag fDepend;
	/**
	 * @brief Constructor
	 */
	OOPDMRequestTask (int processor, const OOPAccessTag & depend);
	OOPDMRequestTask (const OOPDMRequestTask & task);
	OOPDMRequestTask () ;
	
	virtual OOPMReturnType Execute ();
	virtual int ClassId () const
	{
		return TDMREQUESTTASK_ID;
	}
	virtual void Read (TPZStream & buf, void * context = 0);
	static TPZSaveable *Restore (TPZStream & buf, void * context=0);
	virtual void Write (TPZStream  & buf, int withclassid = 0);
};

#endif
