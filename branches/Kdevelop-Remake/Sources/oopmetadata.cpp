#include "oopmetadata.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMetaData"));
#endif

#ifdef OOP_MPE
#include "oopevtid.h"
#endif

OOPMetaData::OOPMetaData (TPZAutoPointer<TPZSaveable> ObPtr, const OOPObjectId & ObjId,
			  const int ProcId, const OOPDataVersion & ver)
{
  fProc = ProcId;
  fObjId = ObjId;
  SubmitVersion(ObPtr,ver);
//  fObjPtr = ObPtr;
}
	
OOPMetaData::OOPMetaData (const OOPObjectId & ObjId,
			  const int ProcId)
{
  fProc = ProcId;
  fObjId = ObjId;
}

OOPMetaData::~OOPMetaData()
{
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Destructor for obj " << Id() << " Clear available versions size = " << fAvailableVersions.size();
    LOGPZ_DEBUG(logger, sout.str());
  }
#endif
#ifdef LOGPZ
  {
    stringstream sout;
    map<OOPDataVersion , TPZAutoPointer<TPZSaveable> >::iterator it;
    for (it=fAvailableVersions.begin(); it!=fAvailableVersions.end(); it++)
    {
      sout << " reference count " << it->second.Count() << " ";
    }
    LOGPZ_DEBUG(logger, sout.str());
  }
#endif
  fAvailableVersions.clear();  
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Available versions size = " << fAvailableVersions.size();
    LOGPZ_DEBUG(logger, sout.str());
  }
#endif
}

void OOPMetaData::ClearAllVersions()
{
  fAvailableVersions.clear();
}

TPZAutoPointer<TPZSaveable> OOPMetaData::Ptr (const OOPDataVersion & version)
{
  if(!fAvailableVersions.size())
  {
    return NULL;
  }
  std::map<OOPDataVersion, TPZAutoPointer<TPZSaveable> >::iterator it;
  it = fAvailableVersions.find(version);
  if(it!=fAvailableVersions.end())
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Exact match found for Version " << version
    << " Object Id : " << Id() << " class id " << it->second->ClassId();
    LOGPZ_DEBUG(logger, sout.str());
#endif
    if(!it->second)
    {
#ifdef LOGPZ
      stringstream sout;
      sout << "Exact match found for Version " << version
      << " Object Id : " << Id()
      << " BUT POINTER IS NULL ";
      sout << sout.str();
      LOGPZ_ERROR(logger, sout.str());
#endif
    }
    return it->second;
  }else
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Exact match Not found for Version " << version
    << " Object Id : " << Id() << " Checking for ANY VERSION requirements."
    << " Going for the first compatible version : ";
    for(it=fAvailableVersions.begin();it!=fAvailableVersions.end();it++) sout << it->first << " {" << (it->first == version) << "} ";
    LOGPZ_DEBUG(logger, sout.str());
#warning "Imprimir versoes"    
#endif
  
    std::map<OOPDataVersion, TPZAutoPointer<TPZSaveable> >::reverse_iterator rit;
    rit = fAvailableVersions.rbegin();
    for(;rit!=fAvailableVersions.rend();rit++)
    {
      if(version.AmICompatible(rit->first))//rit->first.AmICompatible(version))
      {
#ifdef LOGPZ
        stringstream sout;
        sout << " Found Version " << version << " available on ObjectID " << Id();
        LOGPZ_DEBUG(logger, sout.str());
#endif
        if(!rit->second)
        {
    #ifdef LOGPZ
          stringstream sout;
          sout << "Found Available Version " << version
          << " Object Id : " << Id()
          << " BUT POINTER IS NULL ";
          LOGPZ_ERROR(logger, sout.str());
    #endif
        }
        return rit->second;
      }else
      {
#ifdef LOGPZ
        stringstream sout;
        sout << " Incompatible Versions " << version << " and " << rit->first;
        LOGPZ_DEBUG(logger, sout.str());
#endif
      }
    }
#ifdef LOGPZ
    {
    stringstream sout;
    sout << " Pointer Availability. Pointer no longer available for Version " << version << " available versions ";
    rit = fAvailableVersions.rbegin();
    for(;rit!=fAvailableVersions.rend();rit++)
    {
      sout << rit->first << " ";
    }
    
    LOGPZ_ERROR(logger, sout.str());
    }
#endif
    return TPZAutoPointer<TPZSaveable >(NULL);
  }
}

void OOPMetaData::SubmitTag(OOPAccessTag & Tag)
{
  if(!(fObjId))
  {
    fObjId = Tag.Id();
  }
  SubmitVersion(Tag.GetPointer(), Tag.Version());
}

void OOPMetaData::SubmitVersion(TPZAutoPointer <TPZSaveable> NewPtr, const OOPDataVersion & nextversion )
{
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Submitting object id " << Id() << " classid " << NewPtr->ClassId();
    LOGPZ_DEBUG(logger, sout.str());
  }
#endif
  //std::map<OOPDataVersion, TPZAutoPointer<TPZSaveable> >::iterator it;
    if(fAvailableVersions.find(nextversion)==fAvailableVersions.end())
    {
#ifdef LOGPZ
      {
      stringstream sout;
      sout << "More than one Task accessing object " << Id()
      << " New Version will be submitted : "
      << nextversion << " with pointer " << NewPtr;
      LOGPZ_DEBUG(logger, sout.str());
      }
#endif
      std::pair<OOPDataVersion, TPZAutoPointer<TPZSaveable> > item(nextversion, NewPtr);
      fAvailableVersions.insert(item);
    } else
    {
#ifdef LOGPZ
      {
      stringstream sout;
      sout << "Inconsistency detected on SubmitVersion for " << Id()
      << " for Version " << nextversion 
      << " NO ACTION TAKEN !!!";
      }
#endif
    }
}
int OOPMetaData::AccessCounter(OOPDataVersion & version)
{
  map<OOPDataVersion , TPZAutoPointer<TPZSaveable > >::iterator it;
  it = fAvailableVersions.find(version);
  if(it!=fAvailableVersions.end())
  {
    return it->second.Count();
  }
  return 0;
}
int OOPMetaData::AccessCounter(const OOPDataVersion & version) 
{
  map<OOPDataVersion , TPZAutoPointer<TPZSaveable > >::iterator it;
  it = fAvailableVersions.find(version);
  if(it!=fAvailableVersions.end())
  {
    return it->second.Count();
  }
  return 0;
}
void OOPMetaData::VerifyAccessRequests ()
{
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Entering VerifyAccessRequests for Obj " << this->fObjId << " access requests ";
    fAccessList.Print(sout);
    LOG4CXX_DEBUG(logger,sout.str());

#endif
  }
  if(!fAvailableVersions.size())
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " VerifyAccessRequests with empty AvailableVersions " << this->fObjId;
    LOG4CXX_DEBUG(logger,sout.str());
#endif
    return;
  }
  OOPDataVersion version = Version();
  OOPAccessTag tag = fAccessList.IncompatibleRequest(version);
  while (tag)
  {
    if(tag.Proc() != DM->GetProcID())
    {
#warning "Verify sending of OwnerTask here"
      OOPDMOwnerTask * otask = new OOPDMOwnerTask(tag);
      otask->Submit();
      
      // send an owner task with the new version, so that tasks will be canceled there
    }
    else
    {
      OOPObjectId taskid = tag.Id();
#ifdef LOGPZ
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << " OOPMetaData::Verify.. task canceled " << taskid;
      LOG4CXX_DEBUG(logger,sout.str());
#endif
      TM->CancelTask (taskid);
    }
    tag = fAccessList.IncompatibleRequest(version);
  }
  tag = fAccessList.GetCompatibleRequest(version,EReadAccess);
  while(tag)
  {
    if(tag.Proc() == DM->GetProcID())
    {
      TM->GrantAccess(tag);
    } 
    else
    {
      OOPDMOwnerTask * otask = new OOPDMOwnerTask(tag);
      otask->Submit();
    }
    tag = fAccessList.GetCompatibleRequest(version,EReadAccess);
  }
  
}
OOPObjectId OOPMetaData::Id () const
{
  return fObjId;
}
void OOPMetaData::ClearVersion(const OOPDataVersion & version)
{
  map< OOPDataVersion , TPZAutoPointer < TPZSaveable > >::iterator it;
  it = fAvailableVersions.find(version);
  if(it!=fAvailableVersions.end())
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Cleaning Pointer for Version\n" << it->first ;
    LOG4CXX_DEBUG(logger,sout.str());
#endif    
    fAvailableVersions.erase(it);
  }
}
/**
 * returns true if the current processor is owner of the object
 */
bool OOPMetaData::IamOwner () const
{
  return Proc () == DM->GetProcID ();
}

void OOPMetaData::SubmitAccessRequest (const OOPAccessTag &tag)
{
  fAccessList.InsertTag(tag);
  VerifyAccessRequests();
}

void OOPMetaData::SetId (OOPObjectId & id)
{
	fObjId = id;
}
void OOPMetaData::HandleMessage (OOPDMOwnerTask & ms)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Calling HandleMessage for obj " << fObjId << " message type " << ms.fType;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  switch(ms.fTag.AccessMode() ) {
    case EGrantReadAccess:
    {
      if(ms.fTag.GetPointer())
      {
        SubmitVersion(ms.fTag.GetPointer(),ms.fTag.Version());
      }
      fProc = ms.fTag.Proc();
#warning "Formerly ProcOrigim from OwnerTask"//      fProcOrigin;
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << "Grant read access received from proc " << ms.fProcOrigin;
        LOGPZ_DEBUG(logger,sout.str());
#endif        
      }
      this->VerifyAccessRequests();
      break;
    }
    case ETransferOwnership:
    {
      if(Ptr(ms.fTag.Version()))
      {
        LOGPZ_ERROR(logger, "Receiving transfer ownership for existing object !");
      }
      {
#ifdef LOGPZ
        stringstream sout;
        sout << "Receiving transfer ownership for Obj " << fObjId << " from processor " << ms.fProcOrigin << " with version "<<
          ms.fVersion << " and pointer " << ms.fObjPtr;
        LOGPZ_INFO(logger,sout.str());
#endif 
      }
      if(ms.fTag.GetPointer())//fObjPtr)
      {
        SubmitVersion(ms.fTag.GetPointer(), ms.fTag.Version() );
      }
      fProc = DM->GetProcID();
      this->VerifyAccessRequests();
      break;
    }
    default:
    {
#ifdef LOGPZ      
      stringstream sout;
      sout << "OOPMetaData::HandleMessage "<< fObjId << " unhandled message type " << ms.fType;
      LOGPZ_ERROR(logger,sout.str());
#endif
    }
    break;
  }
     
}



OOPDataVersion OOPMetaData::Version () const
{
  int count = 0;
  count = fAvailableVersions.size();
  if(count)
  {
    return fAvailableVersions.rbegin()->first;
  }
#ifdef LOGPZ    
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " " << fObjId << " unhandled case ! NO OBJECTS";
  LOGPZ_ERROR(logger,sout.str());
#endif    
  
  return OOPDataVersion();
}

void OOPMetaData::Print (std::ostream & out)
{
	out << "\nObj Id " << fObjId << endl;
	map<OOPDataVersion, TPZAutoPointer<TPZSaveable> >::iterator it;
	for(it=fAvailableVersions.begin(); it!=fAvailableVersions.end(); it++)
	{
	   out << " version " << it->first << " classid " << it->second->ClassId() << " count " << it->second.Count() << " pointer " << (void *) it->second.operator->() << endl;
	}
	out << "Owning processor " << fProc << endl;
	out << "fAccessList size " << fAccessList.Count() << endl;
	//fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::ShortPrint(std::ostream & out)
{
#warning "Implementar correto"
	out << "D:" << fObjId << ":" << fProc << ":V:" << /*fVersion
	 <<*/ ":AL:" << fAccessList.Count() << ":";
	//fAccessList.ShortPrint(out);
	out.flush ();
}
void OOPMetaData::PrintLog (std::ostream & out)
{
#warning "Implementar correto"
	out << "\nObj Id " << fObjId << " version " << /*fVersion
		<< */" processor " << fProc << endl;
	out << " OOPData structure" << endl;
	out << "fAccessList size " << fAccessList.Count() << endl;
	//fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::SendAccessRequest (const OOPAccessTag &tag)
{
	OOPDMRequestTask *req = new OOPDMRequestTask (tag);
	TM->SubmitDaemon (req);
}
