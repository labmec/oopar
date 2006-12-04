// -*- c++ -*-
#ifndef OOPMDATADEPENDH
#define OOPMDATADEPENDH
#include "oopobjectid.h"
#include "ooppardefs.h"
#include "oopdataversion.h"
//#include "oopmetadata.h"
#include <deque>
//class OOPStorageBuffer;
using namespace std;
class   OOPMetaData;
/**
 * Implements the dependency relation between a Task a Data
 */
class   OOPMDataDepend
{
public:
  void InvalidatePtr();
  ~OOPMDataDepend(){}		 
  void    Print (std::ostream & out = std::cout) const;
  std::ostream &ShortPrint (std::ostream & out = std::cout) const;
  std::ostream &SuperShortPrint(std::ostream &out) const;
  void LogMe(std::ostream &out);
  /**
   * Operator overloaded
   */
   bool    operator == (const OOPMDataDepend & dd) const;
   /**
    *  operator = to copy the content
    */
    OOPMDataDepend &operator=(const OOPMDataDepend &copy);
  /**
   * Copy constructor
   */
   OOPMDataDepend (const::OOPMDataDepend & dd);
  /**
   * return ths ObjectId to which the dependency refers
   */
   const   OOPObjectId & Id () const
   {
     return fDataId;
   }
  /**
   * returns the version to which the dependency refers
   */
   const   OOPDataVersion & Version () const
   {
     return fVersion;
   }
   /**
   * Increments the version of the datadepend object
   */
   void IncrementVersion()
   {
     fVersion.Increment();
   }
  /**
   * returns the state corresponding to the dependency
   */
  const OOPMDataState & State () const
  {
    return fNeed;
  }
  /**
   * method which will serialize the object
   */
	void    Write (TPZStream  & buf);
  /**
   * method to reconstruct the object
   */
  void Read (TPZStream  & buf, void * context = 0);
  void SetState(OOPMDataState state){
    fNeed = state;
  }
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
	        OOPMetaData * fObjPtr;
      public:
        OOPMetaData * ObjPtr () const;
	void SetObjPtr (OOPMetaData * objptr);
  /**
   * Empty constructor
   */
  OOPMDataDepend ()
  {
          fObjPtr = 0;
  }
  // if fVersion == -1 , there is no version dependency
  /**
   * Constructor with initial parameters
   */
  OOPMDataDepend (const OOPObjectId & id,
			OOPMDataState st, const OOPDataVersion & ver);
  /**
   * Returns access status.
   * returns 0 if no access
   * returns the pointer to the task which is accessing the data
   */
  bool Status()
  {
    // cout << "Pointer " << fObjPtr << endl;
    return fObjPtr != 0;
  }
	
};
class   OOPMDataDependList
{
  std::vector < OOPMDataDepend > fDependList;
public:
  ~OOPMDataDependList(){}		  
  /**
   * this method submits the data dependency to the DataManager
   * @returns 1 if successful 0 if the task should be deleted because of incompatibility
   */
  int SubmitDependencyList (const OOPObjectId & taskid);
  /**
   * Signal the OOPMetaData objects that the task is executing
   * @param condition true if the task enters execution, false if it finished executing
   */
  void SetExecuting (const OOPObjectId & taskid, bool condition);
  /**
   * Prints the list of data dependencies
   */
  void Print (std::ostream & out);
  void ShortPrint (std::ostream & out);
  /**
   * This method will release all access requests filed in this container
   */
  void ReleaseAccessRequests (const OOPObjectId & taskid);
  /**
   * @return the number of elements in the list
   */
  int NElements () const;
  /**
   * appends an element to the list
   * @param depend element appended to the list
   */
  void AppendDependency (const OOPMDataDepend & depend);
  /**
   * Signals the dependency list that a accessrequest has been granted
   * @param depend Dependent data as requested
   * @param ObjPtr pointer to the OOPMetaData object to which the task has access
   */
  void GrantAccess (const OOPMDataDepend & depend,
			     OOPMetaData * ObjPtr);
  /**
   * Signals the dependency list that a accessrequest has been revoked
   * @param depend Dependent data as requested
   */
  void RevokeAccess (const OOPMDataDepend & depend);
  /**
   * deletes all elements
   */
  void Clear ();
  /**
   * zeroes all the OOPMetaDataPointers
   */
  void ClearPointers ();
  /**
   * @return 1 if all access requests have been granted
   */
  int CanExecute ();
  void operator= (const OOPMDataDependList & copy)
  {
    fDependList = copy.fDependList;
  }
  /**
   * Access method while functionality is not fully implemented
   */
  OOPMDataDepend & Dep (int i) 
  {
    return fDependList[i];
  }

  const OOPMDataDepend & Dep (int i) const
  {
    return fDependList[i];
  }

  OOPMDataDepend & Dep (OOPObjectId & Id);
  /**
   * method which will serialize the object
   */
  void Write (TPZStream & buf);
  /**
   * method to reconstruct the object
   */
  void Read (TPZStream & buf, void * context = 0);
};
inline std::ostream &operator<<(std::ostream &out, const OOPMDataDepend &obj) {
	return obj.ShortPrint(out);
}
#endif
