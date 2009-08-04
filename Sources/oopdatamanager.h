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
//#include <semaphore.h>
#include "tpzautopointer.h"

#include <boost/interprocess/sync/interprocess_semaphore.hpp>


class TPZSaveable;
class   OOPStorageBuffer;
class   OOPStorageBuffer;
class   OOPSaveable;
using namespace std;
//class   OOPMetaData;
class   OOPDMOwnerTask;
class   OOPDMRequestTask;
class   OOPCurrentLocation;
class OOPTaskManager;

/**
 * Identifies possible messages submitted to the DataManager.
 * A brief description of each type are also available.
 */
enum DMMessageType {
  /**
   * Identifies a message which carries a Data object along with the OOPAccessTag.
   * This message will be translated on the creation of a OOPMetaData object with a AutoPointer valid on the TPZSaveable object.
   */
  EDMData,

  /**
   * Identifies a message which will be translated in the OOPDMOwnerMessage.
   */
  EDMOwner,

  /**
   * Identifies a message which will generate a AccessRequest posted on the current processor
   */
  EDMRequest,

  /**
   * Identifies a message which will be translated on AccessRequest task submitted to a different processor. Foreign access request.
   */
  EDMForeignRequest
};


/**
 * Implements all the data management in the OOPar environment.
 * Acts as daemon in all processors which are part of the parallel environment.
 * Any data involved in the parallelization must be submitted to the environment
 * through the Data Manager.
 */
class   OOPDataManager
{
//  friend class OOPMetaData;
public:
  /**
   * Initialization of the DataManager on the indicated processor
   * @param Procid : Processor where the data manager should be initialized
   */
  OOPDataManager (int Procid, TPZAutoPointer<OOPTaskManager> TM);
  /**
   * Simple destructor
   */
  ~OOPDataManager ();

  /**
   * the autopointer to the task manager with which the data manager relates
   */
  TPZAutoPointer<OOPTaskManager> TM();

  /**
   * the autopointer to the data manager corresponding to himself
   */
  TPZAutoPointer<OOPDataManager> DM();
  /**
   * Clear the pointer so the object can be deleted
   */
  void ClearPointer();
  /**
   * Terminate the execution thread
   */
  void JoinThread();

  /**
   * Encapsulates the call for SubmittAllObjects.
   * Only for minemonic reasons
   */
  void HandleMessages();
  /**
   * Inserts a ObjectChanged entry on the fChangedObject list
   */
  void ObjectChanged(const OOPObjectId & Id);
  /**
   * Dumps on disk the current state of the Manager object
   */
  void SnapShotMe(std::ostream & out);

  /**
   * Checks the list of available objects since a new version had been submitted
   */
  void ObjectChanged(std::set<OOPObjectId> & set);
  /**
   * The following three methods appends objects to the fMessages list.
   * Each of them submits messages of a different type.
   * PostData appends a changed Saveable object,it differs from SubmitObject which returns a ObjectId.
   * In this case the Object identified in the Tag already has an Id assigned to it.
   */
  void PostData(OOPAccessTag & tag);
  /**
   * SubmitOwnerMessage appends a OwnerMessage to the list. OwnerMessages are basically instructions to the DM concerning
   * some data. Later on the DM perform the required action.
   */
  void PostOwnerMessage(OOPAccessTag &tag);
  /**
   * Submits access requests to the objects maintained by the DM
   * The object to which the AccessRequest is related to is identified on the Tag parameter
   */
  void PostAccessRequest(OOPAccessTag & depend);
  void PostForeignAccessRequest(OOPAccessTag & depend);
  /**
   * Return the processor id which owns the current object
   */
  int     GetProcID ()
  {
    return fProcessor;
  }
  /**
   * Register the object on the data manager
   * Submits the pointer to the TSaveable object to the DataManager
   * @param *obj : Pointer to TSaveable object which is to be submited
   * @param trace : Indicates if submited object is traceable or not
   */
  OOPObjectId SubmitObject (TPZSaveable * obj);
  /**
   * Moves all Posted objects from the fMessages list to the fObjects list
   * It actually processes messages translating them in actions.
   */
  void SubmitAllObjects();
  void ExtractObjectFromTag(OOPAccessTag & tag);
  void ExtractOwnerTaskFromTag(OOPAccessTag & tag);
  void ExtractRequestFromTag(OOPAccessTag & tag);
  void ExtractForeignRequestFromTag(OOPAccessTag & tag);
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
   * Verifies access requests for OOPMetaData objects stores fObjects
   */
  void FlushData();
  void WaitWakeUpCall()
  {
    //sem_wait(&fServiceSemaphore);
		fServiceSemaphore->wait();
  }
  void WakeUpCall()
  {
    //sem_post(&fServiceSemaphore);
		fServiceSemaphore->post();
  }
  void SetKeepGoing(bool go);
  int StartService();
  void PostRequestDelete(OOPObjectId & Id);
  void RequestDelete(OOPObjectId & Id);

	/**
	 * Joins the DM service thread
	 */
	void Wait();

	  /**
	   * return the mutex which will synchronize the locking operations
	   */
	  pthread_mutex_t *Mutex()
	  {
		  return &fMutex;
	  }


private:
  pthread_t fServiceThread;
  /**
   * Service thread execution method for the DM
   */
  static void * ServiceThread(void * data);
  /**
   * Semaphore for the DM service thread
   */
  //sem_t fServiceSemaphore;
	boost::interprocess::interprocess_semaphore * fServiceSemaphore;

	/**
	 * the mutex object around which we will be locking
	 */
	pthread_mutex_t fMutex;

	/**
   * Indicates wether ServiceThread should keep running
   */
  bool fKeepGoing;
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
   * the taskmanager with which this datamanager relates
   */
  TPZAutoPointer<OOPTaskManager> fTM;

  /**
   * the datamanager autopointer with which this datamanager relates
   */
  TPZAutoPointer<OOPDataManager> fDM;

  /**
   * Maximum Id number that can be created
   */
  //	long    fMaxId;
  /**
   * Collections of objects
   */
  map < OOPObjectId, OOPMetaData > fObjects;
  /**
   * Holds MetaData objects which had been changed.
   * DM receives those object at the end of a task execution
   */
  set <OOPObjectId> fChangedObjects;
  /**
   * Generates a new object ID
   */
  OOPObjectId GenerateId ();
public:
  /**
   * Returns a pointer to the data structure
   * @param ObjId : Id of object which must have its data structure pointer returned
   */
private:
  OOPMetaData Data (OOPObjectId ObjId);
  /**
   * Returns the Data Version object of the Meta data identified by Id.
   * Necessary for inquiring the current version of the MetaData object.
   * @param Id Identifies the object to be inquired
   */
  //    OOPDataVersion GetDataVersion( const OOPObjectId & Id);


  std::list< std::pair<int, OOPAccessTag> > fMessages;

};
/**
 * Implements a task which owns (Undocumented)
 */
class   OOPDMOwnerTask:public OOPDaemonTask
{
public:

  /// data associated with the owner task
  /**
  * fAccessMode : whether the owner task is granting read or write access
  * fObjectAutoPtr : pointer to the data
  * fObjectId : id of the object associated with the owner task
  * fProcessor : processor which originated the owner task
  * fTaskId : not used
  * fVersion : version of the object being transmitted
  */
  OOPAccessTag fTag;

  /// set of access requests that need to be submitted when changing ownership
  std::set<OOPAccessTag> fTransferRequests;
  /**
   * Constructor
   * @param t : type of ower task
   * @param proc : processor which owns the message
   */
  OOPDMOwnerTask ();

  OOPDMOwnerTask (OOPAccessTag &tag): OOPDaemonTask(tag.Proc()), fTag(tag)
  {
  }

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
  // static TPZSaveable *Restore (TPZStream & buf, void * context = 0);
  virtual void Write (TPZStream & buf, int withclassid);
  // Apenas para DEBUG.
  // virtual void Work() { Debug( "\nTSaveable::Work." ); }
  // virtual void Print() { Debug( " TSaveable::Print." ); }
};

/**
 * Implements a request task (Undocumented)
 */
class OOPDMRequestTask : public OOPDaemonTask
{
public:
  void LogMe(std::ostream & out);
  /**
   * Dependency data of the request
   */
  OOPAccessTag fDepend;
  /**
   * Constructor
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

//extern OOPDataManager *DM;

#endif
