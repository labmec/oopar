#include "oopaccessinfo.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"

#include <sstream>

#include "pzlog.h"
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPAccessInfo"));
#endif

bool OOPAccessInfo::CanExecute (const OOPMetaData & object) const
{
  if (fIsGranted || fIsAccessing) {
#ifdef LOGPZ  
    stringstream sout;
    sout << "OOPAccessInfo::CanExecute should not be called for an object which is being accessed\n";
    LOGPZ_WARN(logger, sout.str());
#endif    
    return false;
  }
	// if the version is not right, don't even consider granting access
	if (!fVersion.CanExecute (object.Version ()))
		return false;
	if (!object.CanGrantAccess ())
		return false;
	switch (this->fState) {
	case EReadAccess:
//          cout << __PRETTY_FUNCTION__ << "version access " << object.HasVersionAccessTask () << " has write access " <<
//            object.HasWriteAccess (fTaskId) << " has read access " << object.HasReadAccess () << endl;
		if (object.HasVersionAccessTask ())
			return false;
		if (object.HasWriteAccess (fTaskId))
			return true;
		if (object.HasReadAccess ())
			return true;
		return false;
		break;
	case EWriteAccess:
		if (object.HasVersionAccessTask ())
			return false;
		if (!object.HasWriteAccessTask ())
			return true;
		if (object.HasWriteAccess (fTaskId))
			return true;
		return false;
		break;
	case EVersionAccess:
		if (!object.HasVersionAccess ())
			return false;
		if (object.HasVersionAccessTask ())
			return false;
		return true;
		break;
	default:
    {
#ifdef LOGPZ    
      stringstream sout;
      sout << "OOPAccessInfo::CanExecute inconsistent\n";
      LOGPZ_WARN(logger, sout.str());
#endif      
    }
		break;
	}
	return false;
}
/**
 * This method adds an access request as passed by SubmitAccessRequest
 * It does no verifications
 */
void OOPAccessInfoList::AddAccessRequest (const OOPObjectId & taskid,
					  const OOPMDataDepend & depend,
					  int processor)
{
	fList.push_back (OOPAccessInfo (taskid, depend, processor));
}
/**
 * Verifies whether there is an access request can be granted for the given
 * object.
 * @param object the OOPMetaData object to which this list refers
 * @param ac If an access request was found, its reference will be stored into ac
 * @return true if an access request was found which can be granted
 */
bool OOPAccessInfoList::VerifyAccessRequests (const OOPMetaData & object,
                                              list <OOPAccessInfo >::iterator & ac)
{
#ifdef LOGPZ
  stringstream sout;
#endif  
	ac = fList.end ();
	if (!object.CanGrantAccess ())
	{
#ifdef LOGPZ  
    stringstream sout;  
    sout <<  __PRETTY_FUNCTION__ << "VerifyAccessRequests object returned CanGrantAccess false\n";
    LOGPZ_WARN (logger, sout.str());
#endif    
    return false;
	}
	list < OOPAccessInfo >::iterator i;
	if (!HasReadAccessGranted () && !HasWriteAccessGranted ()) {
		i = fList.begin ();
		while (i != fList.end ()) {
			if (!(i->fIsGranted) && i->fState == EVersionAccess) {
				if (i->CanExecute (object)) {
					ac = i;
					return true;
				}
			}
			i++;
		}
	}
	if (!HasWriteAccessGranted()) {
		i = fList.begin ();
		while (i != fList.end ()) {
			if (!(i->fIsGranted) && i->fState == EReadAccess) {
				/*if (HasReadAccessGranted ())
					return false;*/
//      cout << __PRETTY_FUNCTION__ << __LINE__ << " Can execute returned "<< i->CanExecute(object) << endl;
				if (i->CanExecute (object)) {
					ac = i;
					return true;
				}
			}
			i++;
		}
	}
	if(!object.HasReadAccess() && object.IamOwner()){
		i = fList.begin ();
		while (i != fList.end ()) {
			if (!(i->fIsGranted) && i->fState == EWriteAccess) {
				if (i->CanExecute (object)) {
					ac = i;
					return true;
				}
			}
			i++;
		}
	}
	/*
	if (!HasReadAccessGranted ()) {
		i = fList.begin ();
		while (i != fList.end ()) {
			if (!(i->fIsGranted) && i->fState == EWriteAccess) {
				if (HasReadAccessGranted ())
					return false;
				if (i->CanExecute (object)) {
					ac = i;
					return true;
				}
			}
			i++;
		}
	}
	i = fList.begin ();
	while (i != fList.end ()) {
		if (!(i->fIsGranted) && i->fState == EReadAccess) {
			if (i->CanExecute (object)) {
				ac = i;
				return true;
			}
		}
		i++;
	}
	*/
	return false;
}
/**
 * Verifies whether an access request is incompatible with the version/state
 * of the corresponding object
 * @param taskid if a request was found then the taskid will indicate the corresponding task
 * @return true if an incompatible task was found
 */
bool OOPAccessInfoList::HasIncompatibleTask (const OOPDataVersion & version,
					     OOPObjectId & taskid)
{
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while (i != fList.end ()) {
		// can't cancel executing tasks
    bool AmICompatibleResult = true;
    if(!i->fIsAccessing) AmICompatibleResult = (i->fVersion).AmICompatible (version);
    if(!AmICompatibleResult) 
    {
#ifdef LOGPZ    
      stringstream sout;
      sout << "False AmICompatible from " << __PRETTY_FUNCTION__ 
           << " IsAccessing returns " << i->fIsAccessing << std::endl;
      LOGPZ_WARN(logger, sout.str());
#endif      
    }
		if (!AmICompatibleResult
		    && !i->fIsAccessing) {
			if (i->fTaskId.IsZeroOOP ()) {
				// This is a different processor requesting
				// access
				fList.erase (i);
				i = fList.begin ();
				continue;
			}
			else {
				// no need to erase, the task will be
				// canceled
				taskid = i->fTaskId;
				return true;
			}
		}
		i++;
	}
	return false;
}
/**
 * Indicates whether any access request of type ReadAccess has been granted
 */
bool OOPAccessInfoList::HasReadAccessGranted () const
{
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EReadAccess && i->fIsGranted)
			return true;
		i++;
	}
	return false;
}
/**
 * Indicates whether any access request of type WriteAccess has been granted
 */
bool OOPAccessInfoList::HasWriteAccessGranted () const
{
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EWriteAccess && i->fIsGranted)
			return true;
		i++;
	}
	return false;
}
bool OOPAccessInfoList::HasVersionAccessGranted () const
{
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EVersionAccess && i->fIsGranted)
			return true;
		i++;
	}
	return false;
}

void 
OOPAccessInfoList::PostPoneWriteAccessGranted () 
{
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EWriteAccess && i->fIsGranted){
			i->fIsGranted = 0;
		}
		i++;
	}
}
void OOPAccessInfoList::ReleaseAccess (const OOPObjectId & taskid,
				       const OOPMDataDepend & depend)
{
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fTaskId == taskid && i->fState == depend.State ()
		    && i->fVersion == depend.Version ()) {
			fList.erase (i);
			break;
		}
		i++;
	}
	if(i == fList.end()) {
#ifdef LOGPZ  
    stringstream sout;
    sout << "InfoList::ReleaseAccess didn't find Task Id = " << taskid << " depend = " << depend <<endl;
    LOGPZ_INFO(logger, sout.str());
#endif    
		Print(cout);
		cout.flush();
	}
	
}
bool OOPAccessInfoList::HasAccessGranted (const OOPObjectId & taskid,
                                          const OOPMDataDepend & depend) const
{
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fTaskId == taskid && i->fState == depend.State ()
		    && i->fVersion == depend.Version ()) {
			return i->fIsGranted != 0;
			break;
		}
		i++;
	}
	return false;
}
void OOPAccessInfoList::ReleaseAccess (list < OOPAccessInfo >::iterator & ac)
{
	if (ac != fList.end ())
		fList.erase (ac);
}
int OOPAccessInfoList::NElements ()
{
	return fList.size ();
}
/**
 * Indicates whether write requests are filed with appropriate version
 */
bool OOPAccessInfoList::
HasWriteAccessRequests (const OOPDataVersion & version) const
{
	if (HasWriteAccessGranted ())
		return false;
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EWriteAccess
		    && i->fVersion.CanExecute (version))
			return true;
		i++;
	}
	return false;
}
bool OOPAccessInfoList::
HasReadAccessRequests (const OOPDataVersion & version) const
{
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EReadAccess
		    && i->fVersion.CanExecute (version))
			return true;
		i++;
	}
	return false;
}
/**
 * Indicates whether version requests are filed with appropriate version
 */
bool OOPAccessInfoList::
HasVersionAccessRequests (const OOPDataVersion & dataversion) const
{
	list < OOPAccessInfo >::const_iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EVersionAccess && i->fIsGranted)
			return false;
		i++;
	}
	i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fState == EVersionAccess
		    && i->fVersion.CanExecute (dataversion))
			return true;
		i++;
	}
	return false;
}
void OOPAccessInfoList::SetExecute (const OOPObjectId & taskid,
                                    const OOPMDataDepend & depend,
                                    bool condition)
{
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fTaskId == taskid && i->fState == depend.State ()
		    && i->fVersion == depend.Version ()) {
			i->fIsAccessing = condition;
			break;
		}
		i++;
	}
}
 /**
 * Revokes all access requests and cancels the tasks which are not executing
 */
void OOPAccessInfoList::RevokeAccessAndCancel ()
{
	OOPObjectId taskid;
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fIsAccessing) {
		}
		else if (taskid.IsZeroOOP () && i->fIsGranted) {
			taskid = i->fTaskId;
			OOPMDataDepend depend (taskid, i->fState,
					       i->fVersion);
			TM->RevokeAccess (taskid, depend);
		}
		else if (i->fTaskId == taskid && i->fIsGranted) {
			OOPMDataDepend depend (taskid, i->fState,
					       i->fVersion);
			TM->RevokeAccess (taskid, depend);
		}
		i++;
		if (i == fList.end () && !taskid.IsZeroOOP ()) {
			TM->CancelTask (taskid);
			taskid.Zero ();
			i = fList.begin ();
		}
	}
}
 /**
 * Revokes all access which have been granted
 */
void OOPAccessInfoList::RevokeWriteAccess (const OOPMetaData & obj)
{
  list < OOPAccessInfo >::iterator i = fList.begin ();
  while (i != fList.end ())
  {
    if (i->fIsAccessing)
    {
#ifdef LOGPZ    
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << "cant revoke access for an executing task taskid " << i->fTaskId << " obj " << obj.Id();
      LOGPZ_ERROR(logger,sout.str());
#endif      
    }
    else if (i->fIsGranted && !(i->fState == EReadAccess))
    {
      OOPMDataDepend depend (obj.Id(), i->fState, i->fVersion);
      TM->RevokeAccess (i->fTaskId, depend);
      i->fIsGranted=0;
    }
    i++;
  }
}
 /**
  * Returns true if a task is accessing the data
  */
bool OOPAccessInfoList::HasExecutingOrReadGrantedTasks ()
{
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while (i != fList.end ()) {
		if (i->fIsAccessing || (i->fState == EReadAccess && i->fIsGranted))
			return true;
		i++;
	}
	return false;
}
/**
 * Transfer the access requests to the given processor
 */
void OOPAccessInfoList::TransferAccessRequests(OOPObjectId &id, int processor) {
	list < OOPAccessInfo >::iterator i = fList.begin ();
	// Send the requests of tasks executing on the current processor
	// Keep a copy of the requests
	while(i != fList.end()) {
		if(i->fProcessor == DM->GetProcID()) {
			OOPMDataDepend depend(id,i->fState,i->fVersion);
			OOPDMRequestTask *reqt = new OOPDMRequestTask(processor,depend);
			reqt->fProcOrigin = i->fProcessor;
			TM->SubmitDaemon(reqt);
		}
		i++;
	}
	i = fList.begin();
	while(i != fList.end()) {
		if(i->fProcessor == processor || i->fProcessor == DM->GetProcID()) {
			i++;
		// Send the requests which came from other processors to be filed at
		// the target processor
		// delete the requests from the current object because the ownership was
		// transferred
		} else {
			OOPMDataDepend depend(id,i->fState,i->fVersion);
			OOPDMRequestTask *reqt = new OOPDMRequestTask(processor,depend);
			reqt->fProcOrigin = i->fProcessor;
			TM->SubmitDaemon(reqt);
			fList.erase(i);
			i=fList.begin();
		}
	}
}
/**
 * Resend the granted access requests (because a read access has been 
 * canceled)
void OOPAccessInfoList::ResendGrantedAccessRequests(OOPObjectId &id, int owningproc) {
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while(i != fList.end()) {
		if(i->fIsGranted && !i->fIsAccessing) {
			OOPMDataDepend depend(id,i->fState,i->fVersion);
			OOPDMRequestTask *reqt = new OOPDMRequestTask(owningproc,depend);
			reqt->fProcOrigin = i->fProcessor;
			TM->SubmitDaemon(reqt);
		}
		i++;
	}
}
 */

void OOPAccessInfoList::Print(std::ostream & out) {
	list < OOPAccessInfo >::iterator i = fList.begin ();
	while(i != fList.end()) {
		i->Print(out);
		i++;
	}
}
