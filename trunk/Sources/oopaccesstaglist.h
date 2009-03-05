//
// C++ Interface: oopaccesstaglist
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPACCESSTAGLIST_H
#define OOPACCESSTAGLIST_H

#include "oopaccesstag.h"
#include <set>
#include <vector>

class OOPDataManager;
extern OOPDataManager *DM;



/**
 * Implements a list of OOPAccessTags
 * Actually the container of OOPAccessTag is implemented as Vector
 * Responsible for managing a vector of AccessTag.
 * Used mostly by OOPTask class.
 */
class OOPAccessTagList{
public:
  OOPAccessTagList();

  ~OOPAccessTagList();
  /**
   * Grants required access to the tags contained in the fTagList
   */
  void GrantAccess(const OOPAccessTag & granted);
  /**
   * Ivalidates the TPZAutoPointer contained object pointer
   */
  void Clear();
  /**
   * Appends a new tag to fTagList
   */
  void AppendTag(const OOPAccessTag & tag);
  /**
   * Indicates if all the Tags on the TagList have its AutoPtr valids.
   * This indicates that all requirements are fullfiled
   */
  bool CanExecute();
  /**
   * Returns the ObjectPointer for the ith object on the list
   */
  TPZSaveable * ObjectPtr(int idepend)
  {
    return fTagList[idepend].AutoPointer().operator->();
    //return fTagList[idepend].fObjectAutoPtr;//  .AutoPointer().operator->();
  }
  /**
   * Atomically increments all data versions to which the access mode was WriteAccess
   */
  void IncrementWriteDependent();
  /**
   * Returns the amount of Items on the TagList
   */
  int Count()
  {
    return fTagList.size();
  }
  void Print(std::ostream & out);
  void ShortPrint(std::ostream & out);
  void Read(TPZStream & buf, void *context);
  void Write(TPZStream & buf, int withclassid);
  /**
   * After Incrementing the DataVersions. All new versions has to be submitted to the DM
   */
  void SubmitIncrementedVersions();
  void PostRequests(OOPObjectId & Id);
	
	OOPAccessTag GetTag(int i);
private:
  /**
   * Holds the list the OOPAccessTag objects
   */
  vector<OOPAccessTag> fTagList;

};

#endif

