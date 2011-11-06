/**
 * @file
 * @brief Contains the OOPAccessTagList class which implements a list of OOPAccessTags.
 */

#ifndef OOPACCESSTAGLIST_H
#define OOPACCESSTAGLIST_H

#include "oopaccesstag.h"
#include <set>
#include <vector>

class OOPDataManager;

/**
 * @brief Implements a list of OOPAccessTags. 
 * Actually the container of OOPAccessTag is implemented as Vector \n
 * Responsible for managing a vector of AccessTag. \n
 * Used mostly by OOPTask class.
 * @author Edimar Cesar Rylo <a href="ecrylo@uol.com.br">Lattes</a>
 * @since 2007
 */
class OOPAccessTagList {
	
public:
	/**
	 * @brief Default constructor
	 */
  OOPAccessTagList();
	/**
	 * @brief Default destructor
	 */
  ~OOPAccessTagList();
	
  /**
   * @brief Grants required access to the tags contained in the fTagList
   */
  void GrantAccess(const OOPAccessTag & granted);
  /**
   * @brief Ivalidates the TPZAutoPointer contained object pointer
   */
  void Clear(TPZAutoPointer<OOPDataManager> DM);
  /**
   * @brief Appends a new tag to fTagList
   */
  void AppendTag(const OOPAccessTag & tag);
  /**
   * @brief Indicates if all the Tags on the TagList have its AutoPtr valids. \n
   * This indicates that all requirements are fullfiled
   */
  bool CanExecute();
  /**
   * @brief Returns the ObjectPointer for the ith object on the list
   */
  TPZSaveable * ObjectPtr(int idepend)
  {
    return fTagList[idepend].AutoPointer().operator->();
  }
  /** 
   * @brief Atomically increments all data versions to which the access mode was WriteAccess 
   */
  void IncrementWriteDependent();
  /** 
   * @brief Returns the amount of Items on the TagList
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
   * @brief After Incrementing the DataVersions. All new versions has to be submitted to the DM
   */
  void SubmitIncrementedVersions(TPZAutoPointer<OOPDataManager> DM);
  void PostRequests(OOPObjectId & Id, TPZAutoPointer<OOPDataManager> DM);

	OOPAccessTag GetTag(int i);
private:
  /**
   * @brief Holds the list the OOPAccessTag objects
   */
  vector<OOPAccessTag> fTagList;

};

#endif

