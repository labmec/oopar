#ifndef OOPACCESSINFOH
#define OOPACCESSINFOH


#include <deque>
#include <set>
#include <list>
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "ooppardefs.h"
#include "oopmdatadepend.h"
using namespace std;

class OOPMetaData;


/**
 * Implements functionalities concerning access information on a data.
 * It is sufficient to describe all the possible access requirements from a TTask to any data
 */
class OOPAccessInfo {
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
	int fIsGranted;
    /**
     * Indicates if data is being accessed (i.e. task is executing)
     */
  	int fIsAccessing;

    /**
     * Constructor with initial parameters
	 * @param TaskId Id of task requiring access on the data.
	 * @param st Type of access on the data
	 * @param version Version required on the data
	 * @param proc Processor where the access should occur
     */
	OOPAccessInfo(const OOPObjectId &TaskId, const OOPMDataState &st,const OOPDataVersion &version){
		fTaskId= TaskId;
		fState = st;
		fVersion = version;
		fIsGranted = 0;
		fIsAccessing = 0;
	}
	
	OOPAccessInfo(const OOPObjectId &taskid,const OOPMDataDepend &depend) {
		fTaskId= taskid;
		fState = depend.State();
		fVersion = depend.Version();
		fIsGranted = 0;
		fIsAccessing = 0;
	}
	
	/**
	 * Operator equal overloaded
	 */
	OOPAccessInfo & operator = (const OOPAccessInfo & aci){
		fTaskId= aci.fTaskId;
		fState = aci.fState;
		fVersion = aci.fVersion;
		fIsGranted = aci.fIsGranted;
		fIsAccessing = aci.fIsAccessing;
		return *this;
	}
	/**
	 * Copy constructor
	 * @param aci AccessInfo object to be copied
	 */
	OOPAccessInfo(const ::OOPAccessInfo & aci){
		fTaskId= aci.fTaskId;
		fState = aci.fState;
		fVersion = aci.fVersion;
		fIsGranted = aci.fIsGranted;
		fIsAccessing = aci.fIsAccessing;
	}

	bool OOPAccessInfo::operator ==(const OOPAccessInfo &other) {
		return (fTaskId == other.fTaskId && fState == other.fState && fVersion == other.fVersion);
	}

	void Print(ostream & out = cout){
		out << "Is Accessing ? " << (bool)fIsAccessing << endl;
		out << "Is Granted ? " << fIsGranted << endl;
		out << "Data State " << fState << endl;
		out << "TaskId " << endl;
		fTaskId.Print(out);
		out << "Version" << endl;
		fVersion.Print(out);
	}

	bool CanExecute(const OOPMetaData &object) const;
	
};

/**
 * This class manages the set of access requests which were filed
 * to an OOPar object (data + OOPMetaData)
 */
class OOPAccessInfoList {
	list<OOPAccessInfo> fList;
public:

	/**
	 * returns the number of elements in the list
	 */
	int NElements();
/**
 * This method adds an access request as passed by SubmitAccessRequest
 * It does no verifications
 * @param taskid Id of the task corresponding to the reques
 * @param depend Dependency information 
 */
void AddAccessRequest(const OOPObjectId &taskid, const OOPMDataDepend &depend);


/**
 * Verifies whether there is an access request can be granted for the given
 * object.
 * @param object the OOPMetaData object to which this list refers
 * @param ac If an access request was found, its reference will be stored into ac
 * @return true if an access request was found which can be granted
 */
bool VerifyAccessRequests(const OOPMetaData &object, OOPAccessInfo * &ac);

/**
 * Verifies whether an access request is incompatible with the version/state
 * of the corresponding object
 * @param taskid if a request was found then the taskid will indicate the corresponding task
 * @return true if an incompatible task was found
 */
bool HasIncompatibleTask(const OOPDataVersion &version, OOPObjectId &taskid) const;

/**
 * Indicates whether any access request of type ReadAccess has been granted
 */
bool HasReadAccessGranted() const;
/**
 * Indicates whether any access request of type WriteAccess has been granted
 */
bool HasWriteAccessGranted() const;

/**
 * Indicates whether write requests are filed with appropriate version
 */
bool HasWriteAccessRequests(const OOPDataVersion &object) const;

/**
 * Indicates whether version requests are filed with appropriate version
 */
bool HasVersionAccessRequests(const OOPDataVersion &object) const;

/**
 * Deletes the corresponding access request record from the list
 */
	void ReleaseAccess(const OOPObjectId &taskid,const OOPMDataDepend &depend);

/**
 * Flags if the task is going into/out-of execution
 */
void SetExecute(const OOPObjectId &taskid, const OOPMDataDepend &depend, bool condition);
};

#endif
