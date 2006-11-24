#include "oopmetadata.h"
#include "oopdatamanager.h"
#include "oopcommmanager.h"
#include "ooptaskmanager.h"
#include <vector>
class   OOPDataVersion;
class   OOPMetaData;
class   OOPDMOwnerTask;
//class OOPMDataState;
class   OOPSaveable;
class   OOPObjectId;


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
  f_PtrBeingModified = false;
  fProc = ProcId;
  fObjId = ObjId;
  fTrans = ENoTransition;
  fToDelete = 0;
  fTrace = 0;
  SubmitVersion(ver, ObPtr);
//  fObjPtr = ObPtr;
}
	
OOPMetaData::OOPMetaData (const OOPObjectId & ObjId,
			  const int ProcId)
{
  f_PtrBeingModified = false;
  fProc = ProcId;
  fObjId = ObjId;
  fTrans = ENoTransition;
  fToDelete = 0;
  fTrace = 0;
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

TPZAutoPointer<TPZSaveable> OOPMetaData::Ptr (OOPDataVersion & version)
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

void OOPMetaData::SubmitVersion(const OOPDataVersion & nextversion, TPZAutoPointer <TPZSaveable> NewPtr)
{
#ifdef LOGPZ
      {
      stringstream sout;
      sout << "Submitting object id " << Id() << " classid " << NewPtr->ClassId();
      LOGPZ_DEBUG(logger, sout.str());
      }
#endif
  //std::map<OOPDataVersion, TPZAutoPointer<TPZSaveable> >::iterator it;
  if(f_PtrBeingModified)
  {
    if(fAvailableVersions.find(nextversion)==fAvailableVersions.end())
    {
#ifdef LOGPZ
      {
      stringstream sout;
      sout << "Only one Task accessing this object" << Id()
      << " New Version will be submitted, deleting all other versions, hope it won't harm any task :-) ";
      LOGPZ_DEBUG(logger, sout.str());
      }
#endif
      ClearAllVersions();
#ifdef LOGPZ
      {
      stringstream sout;
      sout << "New version sucessfully submitted for object " << Id()
      << " New Version " << nextversion;
      LOGPZ_DEBUG(logger, sout.str());
      }
#endif
      std::pair<OOPDataVersion, TPZAutoPointer<TPZSaveable> > item(nextversion, NewPtr);
      fAvailableVersions.insert(item);
    } else
    {
#ifdef LOGPZ
      stringstream sout;
      sout << "WRONG VERSION ARITHMETIC !!!!! for object " << Id()
      << " Version " << nextversion << " already exists !!!";
      LOGPZ_ERROR(logger, sout.str());
#endif
    }
  } else
  {
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
}

bool OOPMetaData::PointerBeingModified() const{
  return f_PtrBeingModified;
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
  OOPObjectId taskid;
  while (fAccessList.HasIncompatibleTask (Version(), taskid))
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " OOPMetaData::Verify.. task canceled " << taskid;
    LOG4CXX_DEBUG(logger,sout.str());
#endif
    TM->CancelTask (taskid);
  }
  if (fTrans != ENoTransition){
#ifdef LOGPZ    
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Verify leaving because fTrans = " << fTrans ;
    LOG4CXX_DEBUG(logger,sout.str());
#endif    
    return;
  }
  fAccessList.VerifyAccessRequests(*this);
}
OOPObjectId OOPMetaData::Id () const
{
  return fObjId;
}
bool OOPMetaData::CanGrantAccess () const
{
  if (fToDelete)
    return false;
  return true;
}
/**
 * returns true if the current processor is owner of the object
 */
bool OOPMetaData::IamOwner () const
{
  return Proc () == DM->GetProcID ();
}
void OOPMetaData::ReleaseAccess (const OOPObjectId & taskid,
                                 const OOPMDataDepend & depend)
{
#warning "Commented out -- Must be reimplemented"
  fAccessList.ReleaseAccess (taskid, depend);
  if(fToDelete)
  {
    CheckTransitionState ();
  } else
  {
    CheckTransitionState ();
    VerifyAccessRequests ();
  }
}
void OOPMetaData::CheckTransitionState ()
{
#warning "Commented out -- Must check only for deletion state"
/*  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Calling CheckTransitionState for obj " << fObjId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  if(fToDelete)
  {
    fAccessList.RevokeAccessAndCancel ();
    if (!fAccessList.HasExecutingOrReadGrantedTasks ())
    {
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << "Deleting object " << fObjId;
        LOGPZ_DEBUG(logger,sout.str());
#endif        
      }
      LogDM->LogGeneric(DM->GetProcID(), fObjId, "Deleting Object");
      DM->DeleteObject (fObjId);
      return;
    }
  }
  if (fTrans == ENoTransition)
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "No transition for obj " << fObjId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
    return;
  }
  //if(fTrans == ECancelReadTransition) fAccessList.ResendGrantedAccessRequests(fObjId,this->fProc);
  //fAccessList.RevokeWriteAccess(*this);
  if (!fAccessList.HasExecutingOrReadGrantedTasks ())
  {
    if (fTrans == ECancelReadTransition)
    {
      if (fReadAccessProcessors.find(DM->GetProcID()) != fReadAccessProcessors.end ())
      {
        fReadAccessProcessors.erase(DM->GetProcID());
        if (!IamOwner ())
        {
          OOPDMOwnerTask *town = new OOPDMOwnerTask(ECancelReadAccessConfirmation,fProc);
          //alterei aqui!!!
          town->fObjId=fObjId;
          town->fObjPtr = 0;
          town->fVersion = this->fVersion;
          town->fProcOrigin = DM->GetProcID();
          LogDM->SendOwnTask(town);
          TM->SubmitDaemon(town);
          delete this->fObjPtr;
          fObjPtr = 0;
          {
#ifdef LOGPZ            
            stringstream sout;
            sout << "sending CancelReadAccessConfirmation for obj " << this->fObjId << "from proc " << DM->GetProcID() << " to proc " << fProc ;
            LOGPZ_DEBUG(logger,sout.str());
#endif            
          }
          fReadAccessProcessors.clear();
        }
      }
    }
    else if (fTrans == ESuspendReadTransition)
    {
      if(fSuspendAccessProcessors.find(DM->GetProcID()) == fSuspendAccessProcessors.end())
      {
        fSuspendAccessProcessors.insert(DM->GetProcID());
        if (!IamOwner ())
        {
          {
#ifdef LOGPZ            
            stringstream sout;
            sout << "Sending suspend access confirmation for obj " << fObjId << " to processor " << fProc;
            LOGPZ_DEBUG(logger,sout.str());
#endif            
          }
          OOPDMOwnerTask *town = new OOPDMOwnerTask(ESuspendAccessConfirmation,fProc);
          //alterei aqui!!!!
          town->fObjId=fObjId;
          town->fObjPtr = this->fObjPtr;
          town->fVersion = this->fVersion;
          LogDM->SendOwnTask(town);
          TM->SubmitDaemon(town);
        }
      }
    }
    if (fTrans == ECancelReadTransition)
    {
      if (!fReadAccessProcessors.size ())
      {
        {
#ifdef LOGPZ          
          stringstream sout;
          sout << "all read accesses have been canceled " << fObjId << " on proc " << DM->GetProcID();
          LOGPZ_DEBUG(logger,sout.str());
#endif          
        }
        fTrans = ENoTransition;
      }
      else
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << "CancelReadTransition these processors remain for obj " << fObjId << " on proc " << DM->GetProcID();
        set < int >::iterator i;
        for (i = this->fReadAccessProcessors.begin(); i != fReadAccessProcessors.end(); i++)
        {
          sout << *i << ' ';
        }
        LOGPZ_DEBUG(logger,sout.str());
#endif        
      }
    }
    else if (fTrans == ESuspendReadTransition)
    {
      if (fSuspendAccessProcessors.size () && fSuspendAccessProcessors.size () == fReadAccessProcessors.size ())
      {
        {
#ifdef LOGPZ          
          stringstream sout;
          sout << "All read access have been suspended " << fObjId << " on proc " << DM->GetProcID();
          LOGPZ_DEBUG(logger,sout.str());
#endif          
        }
        fTrans = ENoTransition;
      }
    }
  }*/
}
/**
  I own the object : 
  Simply queue the access request
  I don't own the object
  Queue the access request
  the OOPMDataDependObject needs to send a request message if the access cannot be granted
*/
void OOPMetaData::SubmitAccessRequest (const OOPObjectId & taskId,
				       const OOPMDataDepend & depend,
				       int processor)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "SubmitAccessRequest objid " << fObjId << " task " << taskId << " depend " << depend << " proc " << processor;
    LOGPZ_DEBUG(logger,sout.str());
    cout << "SubmitAccessRequest objid " << fObjId << " task " << taskId << " depend " << depend << " proc " << processor;
#endif    
  }
  fAccessList.AddAccessRequest (taskId, depend, processor);
  if (!IamOwner ())
  {
    VerifyAccessRequests();
    if(!fAccessList.HasAccessGranted (taskId, depend))
    {
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << "Sending access request from proc " << DM->GetProcID() << " with depend " << depend << " to proc " << fProc;
        LOGPZ_DEBUG(logger,sout.str());
#endif        
      }
      this->SendAccessRequest (depend);
    }
  } else 
  {
    VerifyAccessRequests ();
  }
}
void OOPMetaData::SetExecute (const OOPObjectId & taskId,
			      const OOPMDataDepend & depend, bool condition)
{
	fAccessList.SetExecute (taskId, depend, condition);
}
void OOPMetaData::SetId (OOPObjectId & id)
{
	fObjId = id;
}
/**
   This method should only be called if I own the object
   Send a TDMOwnerTask transferring the ownership to the processor
*/
void OOPMetaData::TransferObject (int ProcId)
{
#ifdef LOGPZ
  {
    stringstream sout; 
    OOPDataVersion lastver = Version();
    sout << "Transfer object " << fObjId << " to proc " << ProcId << " with classid "<<
    Ptr(lastver)->ClassId();
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  OOPDMOwnerTask *town = new OOPDMOwnerTask(ETransferOwnership,ProcId);
  town->fObjId=fObjId;
  town->fVersion = this->Version();
  town->fObjPtr = this->Ptr(town->fVersion);
  this->fProc = ProcId;
  TM->SubmitDaemon(town);
  fAccessList.TransferAccessRequests(fObjId,ProcId);
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
  switch(ms.fType) {
    case EGrantReadAccess:
    {
/*      if(fObjPtr && ms.fObjPtr)
      {
        LOGPZ_WARN(logger, "Receives the pointer to the object again!");
        if(ms.fProcOrigin != fProc || !(fVersion == ms.fVersion))
        {
#ifdef LOGPZ          
          stringstream sout;
          sout << __PRETTY_FUNCTION__ << "incompatible ownertask for obj " << fObjId << " version " << fVersion <<
              " ms.fVersion " << ms.fVersion << " fProc " << fProc << " ms.fProcOrigin " << ms.fProcOrigin;
          LOGPZ_ERROR(logger,sout.str());
#endif          
        }
      }*/
      if(ms.fObjPtr)
      {
        //fObjPtr = ms.fObjPtr;
        if(fAvailableVersions.find(ms.fVersion) != fAvailableVersions.end())
        {
#ifdef LOGPZ 
          stringstream sout;
          sout << "ERROR on HandleMessage - Version already exists " << fObjId 
          << " Version " << ms.fVersion;
          LOGPZ_ERROR(logger,sout.str());
#endif
        } else 
        {
          std::pair<OOPDataVersion, TPZAutoPointer<TPZSaveable> > item(ms.fVersion, ms.fObjPtr);
          fAvailableVersions.insert(item);
        }
      }
      fProc = ms.fProcOrigin;
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
      if(Ptr(ms.fVersion))
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
      // isto deveria pelo menos gerar um log...
      //if(fObjPtr && ms.fObjPtr) delete fObjPtr;
      if(ms.fObjPtr)
      {// fObjPtr = ms.fObjPtr;
        SubmitVersion(ms.fVersion, ms.fObjPtr);
      }
      fProc = DM->GetProcID();
      this->VerifyAccessRequests();
      break;
    }
    case ENotifyDeleteObject:
    {
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << "Notify delete object received";
        LOGPZ_DEBUG(logger,sout.str());
#endif        
      }
      this->DeleteObject();
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

void OOPMetaData::DeleteObject ()
{

  this->fToDelete = 1;
  fAvailableVersions.clear();
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "deleting object " << fObjId ;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  
  //CheckTransitionState();
}

void OOPMetaData::RequestDelete ()
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Calling " << __PRETTY_FUNCTION__;
    LOGPZ_DEBUG (logger,sout.str());
#endif    
  }
	
	if (this->fProc == DM->GetProcID ()) {
		DeleteObject ();
	}
	else {
    {
#ifdef LOGPZ      
      stringstream sout;
      sout << "OOPMetaData::RequestDelete should send an owner message " << fObjId ;
      LOGPZ_ERROR(logger,sout.str());
#endif      
    }
	}
}

void OOPMetaData::GrantWriteAccess (OOPObjectId & TaskId, OOPMDataState st,
				    OOPDataVersion & version)
{
}
void OOPMetaData::GrantReadAccess (OOPObjectId TaskId, int ProcId,
				   OOPMDataState AccessRequest,
				   OOPDataVersion version)
{
#ifdef LOGPZ  
  stringstream sout;
  sout << "grant read access for obj " << fObjId << " is used?";
  LOGPZ_WARN(logger,sout.str());
#endif  
}
void OOPMetaData::GrantAccess (OOPMDataState state, int processor)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << "Entering GrantAccess for Object " << this->fObjId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  switch(state) {
  case EWriteAccess: 
  {
#ifdef LOGPZ      
    stringstream sout;
    sout << "OOPMetaData::GrantAccess transferring object" << fObjId << " State "
         << state << " to proc " << processor ;
    LOGPZ_DEBUG(logger,sout.str());
#endif      
  }
    TransferObject(processor);
    break;
  default:
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPMetaData::GrantAccess " << fObjId << " unhandled state " << state ;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
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

void OOPMetaData::TraceMessage (OOPDMOwnerTask & ms)
{
    int myproc = CM->GetProcID ();
    ofstream tout ("trace.txt", ios::app);
    tout << "DataID:";
    ms.fObjId.Print (tout);
    tout << " fObjId:";
    fObjId.Print (tout);
    tout << " fTrans:" << fTrans << " Proc:" << myproc;
    tout << " ProcOrigin:" << ms.fProcOrigin << endl;
    switch (ms.fType) {
	case ENoMessage:{
		tout << "\tENoMessage" << endl;
		break;
        }
	case ETransferOwnership:{
		tout << "\tETransferOwnership" << endl;
		break;
	}
	case EGrantReadAccess:{
		tout << "\tEGrantReadAccess" << endl;
		break;
	}
	case ENotifyDeleteObject:{
		tout << "\tENotifyDeleteObject" << endl;
		break;
	}
	default:{
		tout << "\tCannot Trace!" << endl;
		break;
        }
    }
}
void OOPMetaData::TraceMessage (char *message)
{
#warning "Implementar correto"
	ofstream tout ("trace.txt", ios::app);
	tout << "fObjId:";
	fObjId.Print (tout);
	tout << " fProc:" << fProc << " fVersion:";
// 	fVersion.Print (tout);
	tout << " fTrans:" << fTrans << message << endl;
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
	out << "fAccessList size " << fAccessList.NElements () << endl;
	fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::ShortPrint(std::ostream & out)
{
#warning "Implementar correto"
	out << "D:" << fObjId << ":" << fProc << ":V:" << /*fVersion
	 <<*/ ":AL:" << fAccessList.NElements () << ":";
	fAccessList.ShortPrint(out);
	out.flush ();
}
void OOPMetaData::PrintLog (std::ostream & out)
{
#warning "Implementar correto"
	out << "\nObj Id " << fObjId << " version " << /*fVersion
		<< */" processor " << fProc << endl;
	out << " OOPData structure" << endl;
	out << "fAccessList size " << fAccessList.NElements () << endl;
	fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::SendAccessRequest (const OOPMDataDepend & depend)
{
	OOPDMRequestTask *req = new OOPDMRequestTask (fProc, depend);
	TM->SubmitDaemon (req);
}
