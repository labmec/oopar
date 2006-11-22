#include "oopaccessinfo.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"
#include "ooperror.h"

#include <sstream>

#include "pzlog.h"
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMetaData"));
#endif

bool OOPAccessInfo::CanExecute (const OOPMetaData & object) const
{
  if (fIsGranted || fIsAccessing) {
#ifdef LOGPZ  
    stringstream sout;
    sout << "OOPAccessInfo::CanExecute should not be called for an object which is being accessed\n";
    LOGPZ_ERROR(logger, sout.str());
#endif    
    return false;
  }
  // if the version is not right, don't even consider granting access
  if (!fVersion.CanExecute (object.Version ()))
  {
#ifdef LOGPZ  
    stringstream sout;
    sout << "OOPAccessInfo::CanExecute returning false fVersion " << fVersion << " object version " << object.Version();
    LOGPZ_DEBUG(logger, sout.str());
#endif    
      return false;
  }
  switch (this->fState) {
  case EReadAccess:
    if(!object.PointerBeingModified())
    {
#ifdef LOGPZ  
      stringstream sout;
      sout << "OOPAccessInfo::CanExecute returning true";
      LOGPZ_DEBUG(logger, sout.str());
#endif    
      return true;
    } 
    else
    {
      return false;
    }
    break;
  case EWriteAccess:
    if (!object.PointerBeingModified ())
    {
#ifdef LOGPZ  
      stringstream sout;
      sout << "OOPAccessInfo::CanExecute returning true for WriteAccess";
      LOGPZ_DEBUG(logger, sout.str());
#endif
      return true;
    } else
    {
#ifdef LOGPZ  
      stringstream sout;
      sout << "OOPAccessInfo::CanExecute Pointer being modified";
      LOGPZ_DEBUG(logger, sout.str());
#endif
      return false;
    }
    break;
  default:
    {
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPAccessInfo::CanExecute inconsistent\n";
    LOGPZ_ERROR(logger, sout.str());
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
  OOPAccessInfo tmp(taskid,depend,processor);
  if(taskid.IsZeroOOP() && processor == DM->GetProcID())
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " taskid is zero and request is for my processor dependency "
        << depend << " processor " << processor;
    LOG4CXX_ERROR(logger,sout.str());
    return;
  }
  
  if(find(fList.begin(),fList.end(),(tmp)) != fList.end())
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " repeated access request ";
    tmp.Print(sout);
    sout << "Existing list ";
    Print(sout);
    LOG4CXX_ERROR(logger, sout.str());
  }
  else
  {
    fList.push_back (OOPAccessInfo (taskid, depend, processor));
  }
}

void OOPAccessInfoList::GrantForeignAccess(OOPAccessInfo & info,OOPMetaData & object)
{

  //Checks for the required access, creates the corresponding OwnerTask and submit it to TM
  switch (info.fState) {
    case EReadAccess:
    {
      //OwnerMessage with read access request granted
      OOPDMOwnerTask * town = new OOPDMOwnerTask(EGrantReadAccess, info.fProcessor);
      town->fObjId = object.Id();
      town->fVersion = object.Version();
      town->fObjPtr = object.Ptr(town->fVersion);
#ifdef LOGPZ    
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << " Granting foreign Read Access to Processor "
      << info.fProcessor << " Version " << object.Version() << " for ID " << object.Id()
      << " with pointer " << town->fObjPtr << " of classid " << town->fObjPtr->ClassId();
      LOGPZ_DEBUG(logger, sout.str());
#endif      
      TM->SubmitDaemon(town);
      fList.remove(info);
      break;
    }
    case EWriteAccess:
    {
      //OwnerMessage with transferownership type.
      object.TransferObject(info.fProcessor);
      break;
    }
    default:
    {
      {
#ifdef LOGPZ    
      stringstream sout;
      sout << "OOPAccessInfo::GrantForeignAccess inconsistent OwnerTask Required\n";
      LOGPZ_ERROR(logger, sout.str());
#endif      
      }
      break;
    }
  }

}

/**
 * Verifies whether there is an access request can be granted for the given
 * object.
 * @param object the OOPMetaData object to which this list refers
 * @param ac If an access request was found, its reference will be stored into ac
 * @return true if an access request was found which can be granted
 */
bool OOPAccessInfoList::VerifyAccessRequests(OOPMetaData & object)
{
  list < OOPAccessInfo >::iterator i;
  i = fList.begin ();
  while (i != fList.end ()) {
    if (!(i->fIsGranted)){
      if (i->CanExecute (object)) 
      {
        if(i->fProcessor == DM->GetProcID())
        {
#ifdef LOGPZ
          std::stringstream sout;
          sout << "Giving access request for " << *i << " to object " << object.Id();
          LOGPZ_DEBUG(logger,sout.str());
#endif
          OOPMDataDepend dep(object.Id(),i->fState,i->fVersion);
          TM->NotifyAccessGranted(i->fTaskId,dep,&object);
          i->fIsGranted = true;
          i++;
        }
        else
        {
#ifdef LOGPZ
          {
          std::stringstream sout;
          sout << "Granting foreign access for " << *i << " to object " << object.Id();
          LOGPZ_DEBUG(logger,sout.str());
          }
#endif
          GrantForeignAccess(*i,object);
          //fList.erase(i);
#ifdef LOGPZ
          {
          std::stringstream sout;
          sout << "After Granting foreign access list size " << fList.size();
          LOGPZ_DEBUG(logger,sout.str());
          }
#endif
          i=fList.begin();
        }
      }
      else
      {
         i++;
      }
    }
    else 
    {
      i++;
    }
  }
#ifdef LOGPZ
          {
          std::stringstream sout;
          sout << "Leaving " << __PRETTY_FUNCTION__;
          LOGPZ_DEBUG(logger,sout.str());
          }
#endif
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
  while (i != fList.end ())
  {
		// can't cancel executing tasks
    bool AmICompatibleResult = true;
    if(!i->fIsAccessing) AmICompatibleResult = (i->fVersion) >= (version);
#ifdef PZLOG    
    if(!AmICompatibleResult) 
    {
      stringstream sout;
      sout << "False AmICompatible from " << __PRETTY_FUNCTION__ 
          << " Access request is ";
      i->Print(sout);
      sout << "Version = " << version << " taskid " << taskid;
      LOG4CXX_ERROR(logger, sout.str());
    }
#endif
    if (!AmICompatibleResult && !i->fIsAccessing) {
      if (i->fTaskId.IsZeroOOP ())
      {
        // This is a different processor requesting
        // access
#ifdef PZLOG        
	stringstream sout;
        sout << "Deleting access request ";
        i->Print(sout);
        LOGPZ_ERROR(logger, sout.str());
#endif
// send an ownertask stating the new version of the data
        fList.erase (i);
        i = fList.begin ();
        continue;
      }
      else
      {
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
  list < OOPAccessInfo >::const_iterator i;
  for (i=fList.begin(); i != fList.end (); i++) {
    if (i->fState == EReadAccess && i->fIsGranted)
    {
      return true;
    }
  }
  return false;
}
/**
 * Indicates whether any access request of type WriteAccess has been granted
 */
bool OOPAccessInfoList::HasWriteAccessGranted () const
{
  list < OOPAccessInfo >::const_iterator i;
  for (i = fList.begin(); i != fList.end (); i++) {
    if (i->fState == EWriteAccess && i->fIsGranted)
    {
      return true;
    }
  }
  return false;
}

void OOPAccessInfoList::ReleaseAccess (const OOPObjectId & taskid,
				       const OOPMDataDepend & depend)
{
  list < OOPAccessInfo >::iterator i = fList.begin ();
  for (i=fList.begin(); i != fList.end (); i++) 
  {
    if (i->fTaskId == taskid && i->fState == depend.State ()
        && i->fVersion == depend.Version ()) {
      fList.erase (i);
      return;
    }
    i++;
  }
#ifdef LOGPZ  
  stringstream sout;
  sout << "InfoList::ReleaseAccess didn't find Task Id = " << taskid << " depend = " << depend <<endl;
  LOGPZ_INFO(logger, sout.str());
#endif    
  Print(cout);
  cout.flush();
	
}
bool OOPAccessInfoList::HasAccessGranted (const OOPObjectId & taskid,
                                          const OOPMDataDepend & depend) const
{
  list < OOPAccessInfo >::const_iterator i = fList.begin ();
  for (i=fList.begin(); i != fList.end (); i++) 
  {
    if (i->fTaskId == taskid && i->fState == depend.State ()
              && i->fVersion == depend.Version () && i->fIsGranted) {
      return true;
    }
  }
  return false;
}
int OOPAccessInfoList::NElements ()
{
  return fList.size ();
}
/**
 * Indicates whether version requests are filed with appropriate version
 */
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
    int count = fList.size();
    if (i->fIsAccessing) {
    }
    else if (i->fIsGranted) {
      TM->CancelTask(i->fTaskId);
    }
    if(fList.size() != (unsigned)count)
    {
      i=fList.begin();
    } else
    {
      i++;
    }
  }
}
/**
 * Transfer the access requests to the given processor
 */
void OOPAccessInfoList::TransferAccessRequests(OOPObjectId &id, int processor) {
  list < OOPAccessInfo >::iterator i = fList.begin ();
  // Send the requests of tasks executing on the current processor
  // Keep a copy of the requests
  for(i=fList.begin(); i != fList.end(); i++) {
    if(i->fProcessor == DM->GetProcID()) {
      OOPMDataDepend depend(id,i->fState,i->fVersion);
      OOPDMRequestTask *reqt = new OOPDMRequestTask(processor,depend);
      reqt->fProcOrigin = DM->GetProcID();
      TM->SubmitDaemon(reqt);
    }
  }
  
  i=fList.begin();
  while(i != fList.end()) {
    if(i->fProcessor == DM->GetProcID()) {
      i++;
    }
    else if(i->fProcessor == processor)
    {
      {
        std::stringstream sout;
        sout << __PRETTY_FUNCTION__ << " erasing ";
        (*i).Print(sout);
        LOG4CXX_DEBUG(logger,sout.str());
      }
      fList.erase(i);
      i=fList.begin();
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

void OOPAccessInfoList::Print(std::ostream & out) {
  list < OOPAccessInfo >::iterator i = fList.begin ();
  while(i != fList.end()) {
    i->Print(out);
    i++;
  }
}
void OOPAccessInfoList::ShortPrint(std::ostream & out) {
  list < OOPAccessInfo >::iterator i = fList.begin ();
  while(i != fList.end()) {
    i->ShortPrint(out);
    i++;
  }
}
