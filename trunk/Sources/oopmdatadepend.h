// -*- c++ -*-
#ifndef OOPMDATADEPENDH
#define OOPMDATADEPENDH

#include "oopobjectid.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopmetadata.h"
#include <deque>
using namespace std;

/**
 * Implements the dependency relation between a Task a Data
 */
class OOPMDataDepend {
public:
  void Print(ostream &out = cout) const;
  /**
   * Operator overloaded
   */
  bool operator == (const OOPMDataDepend & dd) const;
  /**
   * Copy constructor
   */
  OOPMDataDepend(const ::OOPMDataDepend & dd);

  /**
   * return ths ObjectId to which the dependency refers
   */
  const OOPObjectId &Id() const { return fDataId;}

  /**
   * returns the version to which the dependency refers
   */
  const OOPDataVersion &Version() const { return fVersion;}

  /**
   * returns the state corresponding to the dependency
   */
  const OOPMDataState &State() const { return fNeed;}
private:
  /**
   * Id of the data needed to execute the task
   */
  OOPObjectId fDataId;	
  /**
   * Type of access required
   */
  OOPMDataState fNeed;	
  /**
   * Version required for the task
   */
  OOPDataVersion fVersion;	
private:
  /**
   * Pointer to the required object itself
   */
  OOPMetaData *fObjPtr;
public:	
  OOPMetaData * ObjPtr();
  void SetObjPtr(OOPMetaData * objptr);
  /**
   * Empty constructor
   */
  OOPMDataDepend() {fObjPtr=0;}
  //if fVersion == -1 , there is no version dependency
  /**
   * Constructor with initial parameters
   */
  OOPMDataDepend (const OOPObjectId &id, 
		  OOPMDataState st,
		  const OOPDataVersion &ver);
  /**
   * Returns access status.
   * returns 0 if no access
   * returns the pointer to the task which is accessing the data
   */
  bool Status() { 
    cout << "Pointer " << fObjPtr << endl;
    return fObjPtr != 0;
  }

};

class OOPMDataDependList {

  deque<OOPMDataDepend> fDependList;

public:
  /**
   * this method submits the data dependency to the DataManager
   * @returns 1 if successful 0 if the task should be deleted because of incompatibility
   */
  int SubmitDependencyList(const OOPObjectId &taskid);

  /**
   * Prints the list of data dependencies
   */
  void Print(ostream &out);

  /**
   * This method will release all access requests filed in this container
   */
  void ReleaseAccessRequests(const OOPObjectId &taskid);

  /**
   * @return the number of elements in the list
   */
  int NElements();

  /**
   * appends an element to the list
   * @param depend element appended to the list
   */
  void AppendDependency(const OOPMDataDepend &depend);

  /**
   * Signals the dependency list that a accessrequest has been granted
   * @param depend Dependent data as requested
   * @param ObjPtr pointer to the OOPMetaData object to which the task has access
   */
  void GrantAccess(const OOPMDataDepend &depend, OOPMetaData *ObjPtr);

  /**
   * deletes all elements
   */
  void Clear();

  /**
   * @return 1 if all access requests have been granted
   */
  int CanExecute();

  void operator=(const OOPMDataDependList &copy) {
	  fDependList = copy.fDependList;
  }

  /**
   * Access method while functionality is not fully implemented
   */
  OOPMDataDepend &Dep(int i) {
	  return fDependList[i];
  }

};

#endif
