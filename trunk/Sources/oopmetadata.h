// -*- c++ -*-
#ifndef TMETADATAH
#define TMETADATAH

#include <deque>
#include <set>
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "pzsave.h"
#include "tpzautopointer.h"
#include "oopaccesstagmultiset.h"
using namespace std;
// NOT IMPLEMENTED :
class   OOPDataManager;
//extern OOPDataManager *DM;
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
  bool ShouldDelete(){return fShouldDelete;}
  void SubmitTag(OOPAccessTag & Tag, TPZAutoPointer<OOPDataManager> DM);
  int AccessCounter(OOPDataVersion & version);
  int AccessCounter(const OOPDataVersion & version) ;
  /**
   * Removes the Data pointer for version from the map of versions
   */
  void ClearVersion(const OOPDataVersion & version);
  virtual ~OOPMetaData();

private:
  /**
   * List of tasks which requires specific access to this data.
   */
  OOPAccessTagMultiSet fAccessList;
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
  std::map<OOPDataVersion, TPZAutoPointer<TPZSaveable> > fAvailableVersions;
public:
  void ClearAllVersions();
  /**
   * MetaData objects holds more than a single version for each Saveable object.
   * Each different version is therefore stored in a map of OOPVersion to Saveable pointer.
   * This Method provides the necessary interface for submitting new versions for a given object
   */
  void SubmitVersion(TPZAutoPointer <TPZSaveable> &NewPtr,const OOPDataVersion  & nextversion,
		  TPZAutoPointer<OOPDataManager> DM);
  /**
   * Different levels of printing.
   * Amount of information and layout are modified in each method
   */
  void PrintLog (std::ostream & out);
  void Print (std::ostream & out);
  void ShortPrint(std::ostream & out);
  /**
   * Sets the Id of current data
   * @param id Id to be set
   */
  void    SetId (OOPObjectId & id);
  /**
   * Empty constructor
   */
  OOPMetaData ();
  OOPMetaData (const OOPMetaData &copy);
  OOPMetaData &operator=(const OOPMetaData &copy);
  /**
   * Constructor
   * @param *ObPtr Pointer to object TSaveable
   * @param ObjId Id of object
   * @param proc Processor number which owns TData.
   */
  OOPMetaData (const OOPObjectId & ObjId,
			     const int proc);
  OOPMetaData (TPZAutoPointer<TPZSaveable> ObPtr, const OOPObjectId & ObjId,
                    const int ProcId, const OOPDataVersion & ver, TPZAutoPointer<OOPDataManager> DM);
  virtual int ClassId () const
  {
    return OOPMETADATA_ID;
  }
  /**
   * Checks if some task on the task access list is satisfied by the current data state
   */
  void    VerifyAccessRequests (TPZAutoPointer<OOPDataManager> DM);
  bool fShouldDelete;
public:
  /**
   * Submits a task which requires access on current data.
   * @param depend dependency type requested.
   */
  void    SubmitAccessRequest (const OOPAccessTag & depend, TPZAutoPointer<OOPDataManager> DM);
private:
  /**
   * The access request is sent to the owning processor if it cannot
   * be honoured on the local processor
   */
  void    SendAccessRequest (const OOPAccessTag & depend, TPZAutoPointer<OOPDataManager> DM);
public:
  /**
   * Transfer an object based on the parameters.
   * @param ProcId Identifies processor Id
   */
  void    TransferObject (int ProcId);
  /**
   * Takes the appropriate action to satisfy the request.
   * This method is used when the data belongs to this processor.
   * Takes action on an incoming message
   * @param &ms Identifies owner of the task.
   */
  void    HandleOwnerMessage (OOPAccessTag & ms, TPZAutoPointer<OOPDataManager> DM);
  /**
   * Returns the processor to which the object belongs
   */
  int Proc () const;
  /**
   * returns true if the current processor is owner of the object
   */
  bool IamOwner (int localprocessor) const;
  /**
   * Sends a TDMOwnerTask granting the access state to the processor
   */
  void    GrantForeignAccess (OOPAccessTag & tag);
  /**
   * Returns the version of the data
   */
  OOPDataVersion Version () const;

private:
  /**
   * Give assess to the data of the metadata
   */
  TPZAutoPointer<TPZSaveable> Ptr(const OOPDataVersion &ver);

public:
  /**
   * Returns current object Id
   */
  OOPObjectId Id () const;
};
inline int OOPMetaData::Proc () const
{
  return fProc;
}
#endif
