
#include "oopaccessinfo.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"

bool OOPAccessInfo::CanExecute(const OOPMetaData &object) const {
	
	if(fIsGranted || fIsAccessing) {
		cout << "OOPAccessInfo::CanExecute should not be called for an object which is being accessed\n";
		return false;
	}
	// if the version is not right, don't even consider granting access
	if(!fVersion.CanExecute(object.Version())) return false;
	if(!object.CanGrantAccess()) return false;
	switch(this->fState) {
	case EReadAccess:
		if(object.HasVersionAccess()) return false;
		if(object.HasWriteAccess(fTaskId)) return true;
		if(object.HasReadAccess()) return true;
		return false;
		break;
	case EWriteAccess:
		if(object.HasVersionAccess()) return false;
		if(!object.HasWriteAccessTask()) return true;
		if(object.HasWriteAccess(fTaskId)) return true;
		return false;
		break;
	case EVersionAccess:
		if(!object.HasVersionAccess()) return false;
		if(object.HasVersionAccessTask()) return false;
		return true;
		break;
	default:
		cout << "OOPAccessInfo::CanExecute inconsistent\n";
		break;
	}
	return false;
}

/**
 * This method adds an access request as passed by SubmitAccessRequest
 * It does no verifications
 */
void OOPAccessInfoList::AddAccessRequest(const OOPObjectId &taskid, const OOPMDataDepend &depend){
	fList.push_back(OOPAccessInfo(taskid,depend));
}


/**
 * Verifies whether there is an access request can be granted for the given
 * object.
 * @param object the OOPMetaData object to which this list refers
 * @param ac If an access request was found, its reference will be stored into ac
 * @return true if an access request was found which can be granted
 */
bool OOPAccessInfoList::VerifyAccessRequests(const OOPMetaData &object, OOPAccessInfo * &ac)  {

	ac = 0;
	if(!object.CanGrantAccess()) return false;

	list<OOPAccessInfo>::iterator i;
	if(!HasReadAccessGranted() && !HasWriteAccessGranted()) {
		i = fList.begin();
		while(i != fList.end()) {
			if(!(i->fIsGranted) && i->fState == EVersionAccess) {
				if(i->CanExecute(object)) {
					ac = &(*i);
					return true;
				}
			}
			i++;
		}
	}
	if(!HasReadAccessGranted()) {
		i= fList.begin();
		while(i != fList.end()) {
			if(!(i->fIsGranted) && i->fState == EWriteAccess) {
				if(HasReadAccessGranted()) return false;
				if(i->CanExecute(object)) {
					ac = &(*i);
					return true;
				}
			}
			i++;
		}
	}
	i= fList.begin();
	while(i != fList.end()) {
		if(!(i->fIsGranted) && i->fState == EReadAccess) {
			if(i->CanExecute(object)) {
				ac = &(*i);
				return true;
			}
		}
		i++;
	}
	return false;
}

/**
 * Verifies whether an access request is incompatible with the version/state
 * of the corresponding object
 * @param taskid if a request was found then the taskid will indicate the corresponding task
 * @return true if an incompatible task was found
 */
bool OOPAccessInfoList::HasIncompatibleTask(const OOPDataVersion &version, OOPObjectId &taskid) const {

	list<OOPAccessInfo>::const_iterator i = fList.begin();
	while(i != fList.end()) {
		// can't cancel executing tasks
		if(!(i->fVersion).AmICompatible(version) && ! i->fIsAccessing) {
			taskid = i->fTaskId;
			return true;
		}
		i++;
	}
	return false;
}

/**
 * Indicates whether any access request of type ReadAccess has been granted
 */
bool OOPAccessInfoList::HasReadAccessGranted() const {
	list<OOPAccessInfo>::const_iterator i = fList.begin();
	while(i != fList.end()) {
		if(i->fState == EReadAccess && i->fIsGranted) return true;
		i++;
	}
	return false;
}
/**
 * Indicates whether any access request of type WriteAccess has been granted
 */
bool OOPAccessInfoList::HasWriteAccessGranted() const {
	list<OOPAccessInfo>::const_iterator i = fList.begin();
	while(i != fList.end()) {
		if(i->fState == EWriteAccess && i->fIsGranted) return true;
		i++;
	}
	return false;
}

void OOPAccessInfoList::ReleaseAccess(const OOPObjectId &taskid, const OOPMDataDepend &depend)
{
	list<OOPAccessInfo>::iterator i = fList.begin();
	while(i != fList.end()) {
		if(i->fTaskId == taskid && i->fState == depend.State() && i->fVersion == depend.Version()) {
			fList.erase(i);
			break;
		}
		i++;
	}
}

int OOPAccessInfoList::NElements()
{
	return fList.size();
}


/**
 * Indicates whether write requests are filed with appropriate version
 */
bool OOPAccessInfoList::HasWriteAccessRequests(const OOPDataVersion &version) const {

	list<OOPAccessInfo>::const_iterator i = fList.begin();
	while(i != fList.end()) {
		if(i->fState == EWriteAccess && !i->fIsGranted && i->fVersion.CanExecute(version)) return true;
		i++;
	}
	return false;


}
/**
 * Indicates whether version requests are filed with appropriate version
 */
bool OOPAccessInfoList::HasVersionAccessRequests(const OOPDataVersion &dataversion) const {

	list<OOPAccessInfo>::const_iterator i = fList.begin();
	while(i != fList.end()) {
		if(i->fState == EVersionAccess && !i->fIsGranted && i->fVersion.CanExecute(dataversion)) return true;
		i++;
	}
	return false;

}

void OOPAccessInfoList::SetExecute(const OOPObjectId &taskid, const OOPMDataDepend &depend, bool condition)
{
	list<OOPAccessInfo>::iterator i = fList.begin();
	while(i != fList.end()) {
		if(i->fTaskId == taskid && i->fState == depend.State() && i->fVersion == depend.Version()) {
			i->fIsAccessing = condition;
			break;
		}
		i++;
	}
}

