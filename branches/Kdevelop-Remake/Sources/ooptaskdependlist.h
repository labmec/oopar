//
// C++ Interface: ooptaskdependlist
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPTASKDEPENDLIST_H
#define OOPTASKDEPENDLIST_H

#include "tpzautopointer.h"
#include "pzsave.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopmdatadepend.h"
/**
This class defines the objects to which the task has access during execution

@author Edimar Cesar Rylo
*/
class OOPTaskDependList{

  /**
   * Structure which defines the data a task can access
   */
  struct OOPTaskData {
    /**
     * Pointer to the saveable object which will be accessed
     */
    TPZAutoPointer<TPZSaveable> fData;
    /**
     * Type of access granted to the data
     */
    OOPMDataState fAccess;
    /**
     * Version of the data object
     */
    OOPDataVersion fVersion;
    OOPTaskData() : fData(), fAccess(ENoAccess), fVersion()
    {
    }
    OOPTaskData(const OOPTaskData &copy) : fData(copy.fData), fAccess(copy.fAccess),
      fVersion(copy.fVersion)
    {
    }

    OOPTaskData &operator=(const OOPTaskData &copy)
    {
      fData = copy.fData;
      fAccess = copy.fAccess;
      fVersion = copy.fVersion;
      return *this;
    }
  };
public:
  /**
   * Default empty constructor
   */
  OOPTaskDependList();
  /**
   * Constructor based on the data dependency list of the taskcontrol object
   */
  OOPTaskDependList(OOPMDataDependList &datalist);

  /**
   * Copy constructor, you never know...
   */
  OOPTaskDependList(const OOPTaskDependList &copy);

  /**
   * operator=, otherwise the language creates one for us...
   */
  OOPTaskDependList &operator=(const OOPTaskDependList &copy);

  /**
   * Destructor, does nothing special
   */
  virtual ~OOPTaskDependList();

  /**
   * Initialization function
   */
  void SetDependency(const OOPMDataDependList &datalist);

  /**
   * Interface for printout
   */
  void Print(std::ostream &out) const;

  /**
   * Direct access function to the taskdata
   */
  OOPTaskData &Data(int i);

  /**
   * Pointer to the ith object of the dependency list
   */
  TPZAutoPointer<TPZSaveable> ObjPtr(int i);

  /**
   * Access type granted to the ith object
   */
  OOPMDataState AccessType(int i);

  /**
   * Version of the ith object (note that this is a reference to the actual object)
   */
  OOPDataVersion &Version(int i);

  /** 
   * Increments the version of the ith object by one
   */
  void IncrementVersion(int i);

  /**
   * Number of dependent data objects
   */
  int NDepend() const;

private:

  /**
   * Vector containing the dependency information objects
   */
  std::vector<OOPTaskData> fDependList;
};

#endif
