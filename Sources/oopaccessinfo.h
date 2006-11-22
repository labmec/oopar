#ifndef OOPACCESSINFOH
#define OOPACCESSINFOH
#include <deque>
#include <set>
#include <list>
#include <iostream>
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "ooppardefs.h"
#include "oopmdatadepend.h"
using namespace std;
class   OOPMetaData;
/**
 * Implements functionalities concerning access information on a data.
 * It is sufficient to describe all the possible access requirements from a TTask to any data
 */
class   OOPAccessInfo
{
      public:
    /**
     * Describes the type of access state 
     */
	OOPMDataState fState;
    /**
     * Identifies the TaskId willing to access the data object 
     */
	OOPObjectId fTaskId;
    /**
     * Identifies in which version the task needs the data to be 
     */
	OOPDataVersion fVersion;
	/**
	 * Indicates whether the corresponding access was granted
	 */
	int     fIsGranted;
    /**
     * Indicates if data is being accessed (i.e. task is executing)
     */
	int     fIsAccessing;
	/**
	 * Processor which filed the request
	 */
	int     fProcessor;
    /**
     * Constructor with initial parameters
	 * @param TaskId Id of task requiring access on the data.
	 * @param st Type of access on the data
	 * @param version Version required on the data
	 * @param proc Processor where the access should occur
     */
	        OOPAccessInfo (const OOPObjectId & TaskId,
			       const OOPMDataState & st,
			       const OOPDataVersion & version, int processor)
	{
		fTaskId = TaskId;
		fState = st;
		fVersion = version;
		fIsGranted = 0;
		fIsAccessing = 0;
		fProcessor = processor;
	}
	OOPAccessInfo (const OOPObjectId & taskid,
		       const OOPMDataDepend & depend, int processor)
	{
		fTaskId = taskid;
		fState = depend.State ();
		fVersion = depend.Version ();
		fIsGranted = 0;
		fIsAccessing = 0;
		fProcessor = processor;
	}
	/**
	 * Operator equal overloaded
	 */
	OOPAccessInfo & operator = (const OOPAccessInfo & aci)
	{
		fTaskId = aci.fTaskId;
		fState = aci.fState;
		fVersion = aci.fVersion;
		fIsGranted = aci.fIsGranted;
		fIsAccessing = aci.fIsAccessing;
		fProcessor = aci.fProcessor;
		return *this;
	}
	/**
	 * Copy constructor
	 * @param aci AccessInfo object to be copied
	 */
	OOPAccessInfo (const::OOPAccessInfo & aci)
	{
		fTaskId = aci.fTaskId;
		fState = aci.fState;
		fVersion = aci.fVersion;
		fIsGranted = aci.fIsGranted;
		fIsAccessing = aci.fIsAccessing;
		fProcessor = aci.fProcessor;
	}
	bool operator == (const OOPAccessInfo & other)
	{
		return (fTaskId == other.fTaskId && fState == other.fState
			&& fVersion == other.fVersion
			&& fProcessor == other.fProcessor);
	}
void    Print (std::ostream & out = std::cout)
	{
		out << "Is Accessing ? " << (bool) fIsAccessing << endl;
		out << "Is Granted ? " << fIsGranted << endl;
		out << "Data State " << fState << endl;
		out << "Processor " << fProcessor << endl;
		out << "TaskId " << fTaskId << endl;
		out << "Version" << fVersion << endl;
	}
void    ShortPrint (std::ostream & out = std::cout) const
	{
		out << "A?:" << (bool) fIsAccessing << ";";
		out << "G?:" << fIsGranted << ";";
		out << "S:" << fState << ";";
		out << "P:" << fProcessor << ";";
		out << "T:" << fTaskId << ";";
		out << "V:" << fVersion;
	}

bool    CanExecute (const OOPMetaData & object) const;
};

inline std::ostream &operator<<(std::ostream &out, const OOPAccessInfo &info)
{
  info.ShortPrint(out);
  return out;
}
/**
 * This class manages the set of access requests which were filed
 * to an OOPar object (data + OOPMetaData)
 */
class   OOPAccessInfoList
{
	list < OOPAccessInfo > fList;
      public:
void
  PostPoneWriteAccessGranted ();

  void GrantForeignAccess(OOPAccessInfo & info,OOPMetaData & object);
		  
  void ShortPrint(std::ostream & out);
		 
  void Print(std::ostream & out);
  /**
   * returns the number of elements in the list
   */
  int     NElements ();
/**
 * This method adds an access request as passed by SubmitAccessRequest
 * It does no verifications
 * @param taskid Id of the task corresponding to the reques
 * @param depend Dependency information 
 */
	void    AddAccessRequest (const OOPObjectId & taskid,
				  const OOPMDataDepend & depend,
				  int processor);
/**
 * Verifies whether there is an access request can be granted for the given
 * object.
 * @param object the OOPMetaData object to which this list refers
 * @return true if an access request was found which can be granted
 */
  bool  VerifyAccessRequests (OOPMetaData & object);
/**
 * Verifies whether an access request is incompatible with the version/state
 * of the corresponding object
 * @param taskid if a request was found then the taskid will indicate the corresponding task
 * @return true if an incompatible task was found
 */
	bool    HasIncompatibleTask (const OOPDataVersion & version,
				     OOPObjectId & taskid);
/**
 * Indicates whether any access request of type ReadAccess has been granted
 */
	bool    HasReadAccessGranted () const;
/**
 * Indicates whether the task has the specified access request granted
 */
	bool    HasAccessGranted (const OOPObjectId & taskid,
				  const OOPMDataDepend & depend) const;
/**
 * Indicates whether any access request of type WriteAccess has been granted
 */
	bool    HasWriteAccessGranted () const;
/**
 * Indicates whether any access request of type VersionAccess has been granted
 */
	bool HasVersionAccessGranted () const;
/**
 * Indicates whether write requests are filed with appropriate version
 */
	bool    HasWriteAccessRequests (const OOPDataVersion & object) const;

/**
 * Indicates whether read requests are filed with appropriate version
 */
	bool    HasReadAccessRequests (const OOPDataVersion & object) const;

/**
 * Indicates whether version requests are filed with appropriate version
 */
	bool    HasVersionAccessRequests (const OOPDataVersion & object)
		const;
/**
 * Deletes the corresponding access request record from the list
 */
	void    ReleaseAccess (const OOPObjectId & taskid,
			       const OOPMDataDepend & depend);
  /**
   * Deletes the corresponding access request record from the list
   */
  void ReleaseAccess(list < OOPAccessInfo >::iterator & ac);
/**
 * Flags if the task is going into/out-of execution
 */
	void    SetExecute (const OOPObjectId & taskid,
			    const OOPMDataDepend & depend, bool condition);
 /**
 * Revokes all access requests and cancels the tasks which are not executing
 */
	void    RevokeAccessAndCancel ();
 /**
 * Revokes all access requests
 */
	void    RevokeWriteAccess (const OOPMetaData & obj);
 /**
  * Returns true if a task is accessing the data
  */
	bool    HasExecutingOrReadGrantedTasks ();
/**
 * Transfer the access requests to the given processor
 */
	void TransferAccessRequests(OOPObjectId &id, int processor);
/**
 * Resend the granted access requests (because a read access has been 
 * canceled)
	void ResendGrantedAccessRequests(OOPObjectId &id, int owningproc);
 */
 
/**
 * copy operator
 */
	OOPAccessInfoList &operator=(const OOPAccessInfoList& copy) {
		fList = copy.fList;
		return *this;
	}
};
#endif
