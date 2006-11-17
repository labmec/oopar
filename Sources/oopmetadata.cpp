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

OOPMetaData::OOPMetaData (TPZSaveable * ObPtr, const OOPObjectId & ObjId,
			  const int ProcId, const OOPDataVersion ver)
{
  fProc = ProcId;
  fObjPtr = ObPtr;
  fObjId = ObjId;
  fTrans = ENoTransition;
  fToDelete = 0;
  fTrace = 0;
  fVersion = ver;
  fTaskWrite.Zero ();
  if(IamOwner()) fReadAccessProcessors.insert(ProcId);
  f_PtrBeingModified = false;
}
	
OOPMetaData::OOPMetaData (TPZSaveable * ObPtr, const OOPObjectId & ObjId,
			  const int ProcId)
{
  fProc = ProcId;
  fObjPtr = ObPtr;
  fObjId = ObjId;
  fTrans = ENoTransition;
  fToDelete = 0;
  fTrace = 0;
  fTaskWrite.Zero ();
  if(IamOwner()) fReadAccessProcessors.insert(ProcId);
  f_PtrBeingModified = false;
}
bool OOPMetaData::PointerBeingModified() const{
  return f_PtrBeingModified;
}
void OOPMetaData::VerifyAccessRequests ()
{
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Entering VerifyAccessRequests for Obj " << this->fObjId;
    LOG4CXX_DEBUG(logger,sout.str());

#endif    
  }
  OOPObjectId taskid;
  while (fAccessList.HasIncompatibleTask (fVersion, taskid))
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
  if (depend.State () == EWriteAccess)
  {
    LogDM->LogReleaseAccess(DM->GetProcID(),fObjId,depend.State(), fProc, fTaskWrite, State(), fVersion);

    fTaskWrite.Zero ();
    // grant read access to the owning processor
    {
#ifdef LOGPZ      
      stringstream sout;
      sout << " granting read access for obj " << fObjId << " to processor " << fProc << " Zeroed fTaskWrite ";
      LOGPZ_DEBUG(logger,sout.str());
#endif      
    }
  }
  else if(depend.State () == EReadAccess)
  {
    OOPDataVersion locver(depend.Version());
    OOPObjectId locid(taskid);
    LogDM->LogReleaseAccess(DM->GetProcID(),fObjId,depend.State(), fProc, locid, State(), locver);
  }
      
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

OOPMDataState OOPMetaData::State () const
{
  if (!fTaskWrite.IsZeroOOP ())
  {
    return EWriteAccess;
  }
  else
  {
    if (fReadAccessProcessors.size () > 0)
    {
      return EReadAccess;
    }
    else
    {
      return ENoAccess;
    }
  }
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
  LogDM->SubmitAccessRequestLog(DM->GetProcID(),Id(),ENoMessage,depend.State(),State(),depend.Version(),processor,taskId);
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
  OOPDMOwnerTask *town = new OOPDMOwnerTask(ETransferOwnership,ProcId);
  town->fObjId=fObjId;
  town->fObjPtr = this->fObjPtr;
  town->fVersion = this->fVersion;
  this->fProc = ProcId;
  LogDM->SendOwnTask(town);
  TM->SubmitDaemon(town);
  fAccessList.TransferAccessRequests(fObjId,ProcId);
  {
#ifdef LOGPZ    
    stringstream sout; 
    sout << "Transfer object " << fObjId << " to proc " << ProcId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
}
void OOPMetaData::HandleMessage (OOPDMOwnerTask & ms)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Calling HandleMessage for obj " << fObjId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  LogDM->ReceiveOwnTask(&ms);
  switch(ms.fType) {
    case EGrantReadAccess:
    {
      if(fObjPtr && ms.fObjPtr)
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
      }
      if(ms.fObjPtr) fObjPtr = ms.fObjPtr;
      fProc = ms.fProcOrigin;
      fVersion = ms.fVersion;
      fReadAccessProcessors.insert(DM->GetProcID());
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

bool OOPMetaData::HasReadAccess () const
{
	return (fReadAccessProcessors.size () != 0);
}
bool OOPMetaData::HasReadAccess (const int Procid) const
{
  if(fTrans != ENoTransition) return false;
  if(fReadAccessProcessors.count(Procid))
  {
    return true;
  }
  else
  {
    return false;
  }
}
bool OOPMetaData::HasWriteAccess (const OOPObjectId & taskid) const
{
	if (fTaskWrite == taskid)
		return true;
	return false;
}
void OOPMetaData::DeleteObject ()
{

  this->fToDelete = 1;
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "deleting object " << fObjId ;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  
  //  LogDM->LogGeneric(DM->GetProcID(), fObjId, "deleting object");

  if (IamOwner())
  {
    fReadAccessProcessors.erase(DM->GetProcID());
    set<int>::iterator i;
    for(i = fReadAccessProcessors.begin(); i != fReadAccessProcessors.end(); i++)
    {
      OOPDMOwnerTask *town = new OOPDMOwnerTask(ENotifyDeleteObject,*i);
      town->fObjId = fObjId;
      LogDM->SendOwnTask(town);
      TM->SubmitDaemon(town);
    }
/*    fSuspendAccessProcessors.clear();
    if (!(fProcVersionAccess != -1) && ! fReadAccessProcessors.count(fProcVersionAccess))
    {
      OOPDMOwnerTask *town = new OOPDMOwnerTask(ENotifyDeleteObject,fProcVersionAccess);
      town->fObjId = fObjId;
      LogDM->SendOwnTask(town);
      TM->SubmitDaemon(town);
    }
    fReadAccessProcessors.clear();*/
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
	LogDM->LogGeneric(DM->GetProcID(),fObjId,"Calling RequestDelete");
	
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
  return fVersion;
}
void OOPMetaData::IncrementVersion (const OOPObjectId &taskid) 
{
  if (fTaskWrite == taskid ) {
    OOPDataVersion ver = fVersion;
    ++ver;
    LogDM->LogSetVersion(DM->GetProcID(),fObjId,fVersion,ver, State(),taskid);
    {
#ifdef LOGPZ      
      stringstream sout;
      sout << "Incrementing Version for Obj " << this->fObjId << " to version "
           << ver;
      LOGPZ_DEBUG(logger,sout.str());
#endif      
    }
#ifdef OOP_MPE
      stringstream auxsout;
/*      auxsout << "D:" << Id().GetId() << ":" << Id().GetProcId()
        << "T:" << fTaskWrite.Id().GetId() << ":" << fTaskWrite.Id().GetProcId()
        << "V:";*/
      ShortPrint(auxsout);
      OOPSoloEvent evt("incrementversion", auxsout.str());
#endif

      ++fVersion;
    }else{
    {
#ifdef LOGPZ      
      stringstream sout;
      sout << "OOPMetaData::IncrementVersion not executed for Obj "<< fObjId << " fTaskWrite " << fTaskWrite << " taskid " << taskid;
      LOGPZ_ERROR(logger,sout.str());
#endif      
    }
  }
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
	ofstream tout ("trace.txt", ios::app);
	tout << "fObjId:";
	fObjId.Print (tout);
	tout << " fProc:" << fProc << " fVersion:";
	fVersion.Print (tout);
	tout << " fTrans:" << fTrans << message << endl;
}
void OOPMetaData::Print (std::ostream & out)
{
	out << "\nObj Id " << fObjId << " version " << fVersion
		<< " processor " << fProc << endl;
	out << " OOPData structure" << endl;
	out << "fAccessList size " << fAccessList.NElements () << endl;
	fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::ShortPrint(std::ostream & out)
{
	out << "D:" << fObjId << ":" << fProc << ":V:" << fVersion
	 << ":AL:" << fAccessList.NElements () << ":";
	fAccessList.ShortPrint(out);
	out.flush ();
}
void OOPMetaData::PrintLog (std::ostream & out)
{
	out << "\nObj Id " << fObjId << " version " << fVersion
		<< " processor " << fProc << endl;
	out << " OOPData structure" << endl;
	out << "fAccessList size " << fAccessList.NElements () << endl;
	fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::SetVersion (const OOPDataVersion & ver,
			      const OOPObjectId & taskid)
{
  if (fTaskWrite == taskid ) {
		LogDM->LogSetVersion(DM->GetProcID(),fObjId,fVersion,ver, State(),taskid);
		fVersion = ver;
    {
#ifdef LOGPZ
      stringstream sout;
      sout << "Setting Version for Obj " << this->fObjId << " to version "
           << ver;
      LOGPZ_DEBUG(logger,sout.str());
#endif
    }
	}
	else {
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPMetaData::SetVersion not executed for Obj "<< fObjId ;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
	}
}
void OOPMetaData::SendAccessRequest (const OOPMDataDepend & depend)
{
	OOPDMRequestTask *req = new OOPDMRequestTask (fProc, depend);
	LogDM->SendReqTask(req);
	TM->SubmitDaemon (req);
}
