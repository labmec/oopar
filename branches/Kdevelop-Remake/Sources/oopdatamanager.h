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
#include "tpzautopointer.h"
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
  /**
   * Initialization of the DataManager on the indicated processor
   * @param Procid : Processor where the data manager should be initialized
   */
  OOPDataManager (int Procid);
  /**
   * Simple destructor
   */
  ~OOPDataManager ();
  /**
   * Inserts a ObjectChanged entry on the fChangedObject list
   */
  void ObjectChanged(OOPObjectId & Id);
  /**
   * Dumps on disk the current state of the Manager object
   */
  void SnapShotMe();

  /**
   * Checks the list of available objects since a new version had been submitted
   */
  void ObjectChanged(std::set<OOPObjectId> & set);
  /**
   * Submits a object to the DataManager.
   * In this case, the object won't be assigned a new Id
   */
  void SubmitData(OOPAccessTag & tag);
  
  void SubmitOwnerMessage(OOPAccessTag &tag);
  void SubmitAccessRequest(OOPAccessTag & depend);
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
private:
public:
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
  //	long    fMaxId;
  /**
   * Collections of objects
   */
  map < OOPObjectId, OOPMetaData *> fObjects;
  /**
   * Recently submitted objects
   * Still not available for the TM.
   */
  list <OOPMetaData * > fSubmittedObjects;
  /**
   * Holds MetaData objects which had been changed.
   * DM receives those object at the end of a task execution
   */
  list <OOPMetaData * > fChangedObjects;
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
  
  std::list< std::pair<int, OOPAccessTag> > fMessages;

};
/**
 * Implements a task which owns (Undocumented)
 */
class   OOPDMOwnerTask:public OOPDaemonTask
{
public:
  
  OOPAccessTag fTag;
  /**
   * Constructor
   * @param t : type of ower task
   * @param proc : processor which owns the message
   */
  OOPDMOwnerTask ();
  
  OOPDMOwnerTask (OOPAccessTag &tag);
  
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
template class TPZRestoreClass<OOPDMOwnerTask,TDMOWNERTASK_ID>;
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
  OOPDMRequestTask (int proc, const OOPAccessTag & depend);
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
template class TPZRestoreClass<OOPDMRequestTask, TDMREQUESTTASK_ID>;

extern OOPDataManager *DM;

#endif
