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

/*
SEQUENCES
  A task wants to access the data, the processor does not own the data
  -> File the request
  -> If the request cannot be satisfied, send the request to the owning processor
  -> Else just fill the request
  A request comes in from a different processor
  -> File the request as processor access request
  The access request is handled by the VerifyAccessRequest method
  If the access can be granted, send an access granted message to the processor
  the granted access needs to be registered in the object
  In case of version access
	the receiving processor needs to notify when it doesnt need the access anymore
  A request needs a transition state in order to be honoured. In these two cases
  -> satisfy a version access request
  -> satisfy a write access request
  To satisfy a version access request
  -> revoke all accesses granted
  -> if there is an executing task set the transition state to ESuspendReadState
  -> suspend all read accesses
  -> put the object in transition state till the suspends are confirmed
  To satisfy a write access request
  -> cancel all read accesses
  -> put the object in transition state till the cancels are confirmed
  An object which is not owned by the processor can be in the following transistion
  states just like the object owned by the processor
  An access request can put an object into natural transition (by the filed request)
  A pending write or version access request will not be satisfied as long as there is an
  executing task is accessing the data
  An object which doesnt own the object can have read access or can be be in
    suspended read access state. This state is recognized by the variable
	fReadAccessProcessors
  On a release access request, the state of the object can change as much as by
    the response of an ownertask.
  */
/*
  I don't own the object
  I have a "compatible" version access request
	-> a TDMOwnertask needs to grant access
	   Version access is granted is fVersionProc == current processor
  I have a "compatible" write access request
    -> a TDMOwnertask will make me the owner of the object
  I have a "compatible" read access request
    -> a TDMOwnertask will give me read access
  I own the object
  I have a "compatible" version access request
    -> I need to suspend all read accesses
	-> The state of the object will be ESuspendReadAccess
	-> I need to wait for confirmation from each accessing processor
	-> I will grant the version access
  I have a "compatible" write access request
    -> I need to cancel all read accessess
	-> The state of the object will be ECancelReadAccess
	-> I need to wait for confirmation from each accessing processor
	-> I will grant the write access
TRANSITION STATES
  I own the object
	ECancelReadTransition : I am waiting till no processor has read access
	ESuspendReadTransition : I am waiting till all processor confirm suspend read access
  I don't own the object
    ECancelReadTransition : I am waiting till no task is accessing the data to send
	    a CancelReadAccessConfirmation message
	ESuspendReadTransition : I am waiting till no task is accessing the data to send
	    a SuspendReadAccessConfirmation message
*/
void OOPMetaData::VerifyAccessRequests ()
{
	DataLog << GLogMsgCounter << endl;
	DataLog << "Entering VerifyAccessRequests for Obj " << this->fObjId << "\n";
	GLogMsgCounter++;
	DataLog.flush();
	//Isso ta errado
	OOPObjectId taskid;
	while (fAccessList.HasIncompatibleTask (fVersion, taskid)) {
		DataLog << "OOPMetaData::Verify.. task canceled " << taskid << endl;
		DataLog.flush();
		TM->CancelTask (taskid);
	}
	if (fTrans != ENoTransition)
		return;
/*
	How do we know the state of the object??
	If any executing task is accessing the object for write or version access
	   there is nothing to do (only one task can access the object at a time)
	Follow the sequence :
		-> first try to satisfy a version access request, first on this
		   processor, then on a different processor.
		-> try to satisfy a write access request on this processor
		-> transfer ownership if is a write access request for a different processor
    If there is an outstanding version access request.
	-> If there is an executing task, there is nothing to do
	-> If there are processors with read access, there is nothing to do
	    BUT HOW DO I KNOW THE OBJECT SENT THE CANCEL MESSAGES??
		-> Either I send the cancel message over and over
		-> Either I forget to send the message
		The messages were sent if the object is in a transition state
	If there are reading processors and the object is not in a transition
	then the messages need to be sent out and the object is put in a transition state
    If the object is in a transition state, the code will not get that far,
	  MESSAGES SHOULD BE SENT FROM HERE
*/
	list < OOPAccessInfo >::iterator ac;
	if (fAccessList.HasVersionAccessRequests (fVersion)
	    && fTrans == ENoTransition) {
		SuspendReadAccess ();
		// we should invoke a procedure to revoke all access requests
//		if(IamOwner() && fReadAccessProcessors.size() == fSuspendAccessProcessors.size()) this->fProcVersionAccess = fProc;
	}
	else if (DM->GetProcID () == fProc
		 && fAccessList.HasWriteAccessRequests (fVersion)
		 && fTrans == ENoTransition) {
		CancelReadAccess ();
		// we should invoke a procedure to revoke all read access
		// requests
	}
	// We need to put verification if there is a valid version access
	// request
	// or valid write access request
	// In these cases all read access must be either suspended or revoked
	while (fAccessList.VerifyAccessRequests (*this, ac)) {
		OOPAccessInfo *access = ac.operator->();
		if (access->fProcessor == DM->GetProcID ()) {
			ac->fIsGranted = 1;
			OOPMDataDepend depend (this->Id (), ac->fState,
					       ac->fVersion);
			if (ac->fState == EWriteAccess) {
				fTaskWrite = ac->fTaskId;
			}
			if (ac->fState == EVersionAccess) {
				fTaskVersion = ac->fTaskId;
				fProcVersionAccess = ac->fProcessor;
			}
			DataLog << "Grant access for Obj " << fObjId << " to " << ac->fTaskId << " with depend " << depend << endl;
			DataLog.flush();
			LogDM->GrantAccessLog(DM->GetProcID(),fObjId,ac->fState,ac->fVersion,ac->fProcessor,ac->fTaskId, State());
			TM->NotifyAccessGranted (ac->fTaskId, depend, this);
		}
		else {
			DataLog << "Sending grant access for obj " << fObjId << " with state " << ac->fState << " to processor" << ac->fProcessor << endl;
			GrantAccess (ac->fState, ac->fProcessor);
			
			fAccessList.ReleaseAccess (ac);
		}
	}
}
OOPObjectId OOPMetaData::Id () const
{
	return fObjId;
}
bool OOPMetaData::CanGrantAccess () const
{
	if (fTrans != ENoTransition)
		return false;
	if (fProcVersionAccess != -1
	    && fProcVersionAccess != DM->GetProcID ())
		return false;
	if (fToDelete)
		return false;
	if (!fTaskVersion.IsZeroOOP ())
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
	DataLog << GLogMsgCounter << ":";
	DataLog << fObjId << ":Entering ReleaseAccess:";
	TransferDataLog << fObjId;
	TransferDataLog.flush();
	DataLog.flush();
	
	GLogMsgCounter++;
	fAccessList.ReleaseAccess (taskid, depend);
	if (depend.State () == EVersionAccess) {
		LogDM->LogReleaseAccess(DM->GetProcID(),fObjId,depend.State(), fProcVersionAccess, fTaskVersion, State(),fVersion);
		this->fProcVersionAccess = -1;
		this->fTaskVersion.Zero ();
		// remove this processor from the list of suspended read
		// access
		if(!IamOwner()) {
			DataLog << "Sending suspend suspend read access for obj " << fObjId << " to processor "<< DM->GetProcID() << endl;
			DataLog.flush();
			TransferDataLog << ":suspend read access " << fObjId << " to processor "<< DM->GetProcID() << endl;
			TransferDataLog.flush();
			OOPDMOwnerTask *town = new OOPDMOwnerTask(ESuspendSuspendAccess,this->fProc);
			town->fObjId = fObjId;
			town->fVersion = fVersion;
			LogDM->SendOwnTask(town); 
			TM->SubmitDaemon(town);
			
		} else {
			list < int >::iterator i = fSuspendAccessProcessors.begin ();
			while (i != fSuspendAccessProcessors.end ()) {
				if (*i != DM->GetProcID ()) {
					OOPObjectId auxId;
					LogDM->LogReleaseAccess(DM->GetProcID(),fObjId,depend.State(), *i, auxId, State(), fVersion);
					DataLog << "Sending suspend suspend read access for obj " << fObjId << " to processor "<< *i << endl;
					TransferDataLog << ":suspend read access " << fObjId << " to processor "<< *i << endl;
					TransferDataLog.flush();
					OOPDMOwnerTask *town = new OOPDMOwnerTask(ESuspendSuspendAccess,*i);
					town->fObjId = fObjId;
					town->fVersion = fVersion;
					LogDM->SendOwnTask(town);
					TM->SubmitDaemon(town);
				}
				i++;
			}
		}
		fSuspendAccessProcessors.clear();
	}
	else if (depend.State () == EWriteAccess) {
		LogDM->LogReleaseAccess(DM->GetProcID(),fObjId,depend.State(), fProc, fTaskWrite, State(), fVersion);
		fTaskWrite.Zero ();
		// grant read access to the owning processor
		DataLog << "granting read access for obj " << fObjId << " to processor " << fProc << endl;
		TransferDataLog << "granting read access " << fObjId << " to processor " << fProc << endl;
		TransferDataLog.flush();
		DataLog.flush();
		fReadAccessProcessors.push_back (fProc);
	}
	if(fToDelete) {
		CheckTransitionState ();
	} else {
		CheckTransitionState ();
		VerifyAccessRequests ();
	}
}
void OOPMetaData::CheckTransitionState ()
{
	DataLog << GLogMsgCounter << endl;
	DataLog << "Calling CheckTransitionState\n";
	DataLog.flush();
	GLogMsgCounter++;
	
	if(fToDelete) {
		fAccessList.RevokeAccessAndCancel ();
		if (!fAccessList.HasExecutingTasks ()) {
			DataLog << "Deleting object " << fObjId << endl;
			LogDM->LogGeneric(DM->GetProcID(), fObjId, "Deleting Object");
			DataLog.flush();
			DM->DeleteObject (fObjId);
			return;
		}
	}
	if (fTrans == ENoTransition) {
		return;
	}
	if(fTrans == ECancelReadTransition) fAccessList.ResendGrantedAccessRequests(fObjId,this->fProc);
	fAccessList.RevokeAccess(*this);
	if (!fAccessList.HasExecutingTasks ()) {
		if (fTrans == ECancelReadTransition) {
			list < int >::iterator i =
				fReadAccessProcessors.begin ();
			while (i != fReadAccessProcessors.end ()) {
				if (*i == DM->GetProcID ()) {
					fReadAccessProcessors.erase (i);
					if (!IamOwner ()) {
						OOPDMOwnerTask *town = new OOPDMOwnerTask(ECancelReadAccessConfirmation,fProc);
						//alterei aqui!!!
						town->fObjId=fObjId;
						town->fObjPtr = this->fObjPtr;
						town->fVersion = this->fVersion;
						town->fProcOrigin = DM->GetProcID();
						LogDM->SendOwnTask(town);
						TM->SubmitDaemon(town);
						delete this->fObjPtr;
						fObjPtr = 0;
						DataLog << "sending CancelReadAccessConfirmation for obj " << this->fObjId << "from proc " << DM->GetProcID() << " to proc " << fProc << endl;
						DataLog.flush();
					}
					break;
				}
				i++;
			}
		}
		else if (fTrans == ESuspendReadTransition) {
			list < int >::iterator i =
				fSuspendAccessProcessors.begin ();
			while (i != fSuspendAccessProcessors.end ()) {
				if (*i == DM->GetProcID ()) {
					break;
				}
				i++;
			}
			if (i == fSuspendAccessProcessors.end ()) {
				fSuspendAccessProcessors.push_back (DM->GetProcID());
				if (!IamOwner ()) {
					DataLog << "Sending suspend access confirmation for obj " << fObjId << " to processor " << fProc << endl;
					DataLog.flush();
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
	}
	if (fTrans == ECancelReadTransition) {
		if (!fReadAccessProcessors.size ()) {
			DataLog << "all read accesses have been canceled " << fObjId << " on proc " << DM->GetProcID() <<  endl;
			DataLog.flush();
			fTrans = ENoTransition;
		}
	}
	else if (fTrans == ESuspendReadTransition) {
		if (fSuspendAccessProcessors.size () && fSuspendAccessProcessors.size () ==
		    fReadAccessProcessors.size ()) {
			DataLog << "All read access have been suspended " << fObjId << " on proc " << DM->GetProcID() << endl;
			DataLog.flush();
			fTrans = ENoTransition;
		}
	}
}
OOPMDataState OOPMetaData::State () const
{
	if (fTaskWrite.IsZeroOOP ()) {
		return EWriteAccess;
	}
	else {
		if (fReadAccessProcessors.size () > 0) {
			return EReadAccess;
		}
		else {
			return ENoAccess;
		}
	}
}
/*
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
	DataLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	DataLog << "SubmitAccessRequest task " << taskId << " depend " << depend << " proc " << processor << endl;
	DataLog.flush();
	LogDM->SubmitAccessRequestLog(DM->GetProcID(),Id(),ENoMessage,depend.State(),State(),depend.Version(),processor,taskId);
	fAccessList.AddAccessRequest (taskId, depend, processor);
	if (!IamOwner ()) {
		VerifyAccessRequests();
		if(!fAccessList.HasAccessGranted (taskId, depend)) {
			//DataLog << "OOPMetaData::SubmitAccessRequest must debug\n";
			DataLog << "Sending access request from proc " << DM->GetProcID() << " with depend " << depend << " to proc " << fProc << endl;
			DataLog.flush();
			this->SendAccessRequest (depend);
		}
	} else {
		VerifyAccessRequests ();
	}
}
void OOPMetaData::SetExecute (const OOPObjectId & taskId,
			      const OOPMDataDepend & depend, bool condition)
{
	fAccessList.SetExecute (taskId, depend, condition);
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
	fProcVersionAccess = -1;
	fTaskVersion.Zero ();
	fTaskWrite.Zero ();
	if(IamOwner()) fReadAccessProcessors.push_back(ProcId);
}
void OOPMetaData::SetId (OOPObjectId & id)
{
	fObjId = id;
}
/*
   This method should only be called if I own the object
   Send a TDMOwnerTask transferring the ownership to the processor
*/
void OOPMetaData::TransferObject (int ProcId)
{
	DataLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	//TransferDataLog << GLogMsgCounter << ":";
	TransferDataLog << fObjId;
	OOPDMOwnerTask *town = new OOPDMOwnerTask(ETransferOwnership,ProcId);
	//alterei aqui
	town->fObjId=fObjId;
	town->fObjPtr = this->fObjPtr;
	town->fVersion = this->fVersion;
	this->fProc = ProcId;
	this->fObjPtr = 0;
	LogDM->SendOwnTask(town);
	TM->SubmitDaemon(town);
	fAccessList.TransferAccessRequests(fObjId,ProcId);
	DataLog << "Transfer object " << fObjId << " to proc " << ProcId << "\n";
	TransferDataLog << ":Transfer object " << fObjId << " to proc " << ProcId << "\n";
	TransferDataLog.flush();
	DataLog.flush();
}
void OOPMetaData::HandleMessage (OOPDMOwnerTask & ms)
{
	DataLog << GLogMsgCounter << endl;
	DataLog << "Calling HandleMessage for obj " << fObjId << "\n";
	DataLog.flush();
	GLogMsgCounter++;
	LogDM->ReceiveOwnTask(&ms);
	switch(ms.fType) {
	case ESuspendAccess:
		DataLog << "Suspend access coming from " << ms.fProcOrigin << endl;
		fTrans = ESuspendReadTransition;
		fProc = ms.fProcOrigin;
		CheckTransitionState();
		break;
	case ESuspendAccessConfirmation: {
	//Receiving a confirmation message from a processor
		if(IamOwner() && ms.fProcOrigin != fProcVersionAccess) {
			DataLog << "suspend access confirmation coming from proc " << ms.fProcOrigin << " and different from proc-version-acc " << fProcVersionAccess << endl;
			DataLog.flush();
		}
		fProcVersionAccess = -1;
		if(!IamOwner() || !HasReadAccess(ms.fProcOrigin)) {
			DataLog << "OOPMetaData::HandleMessage " << fObjId << " SuspendReadAccess at a processor which isnt owner or which for a processor without read access\n";
			DataLog.flush();
			break;
		}
		// insert the processor in the list of processors with suspended access
		list<int>::iterator is = fSuspendAccessProcessors.begin();
		while(is != fSuspendAccessProcessors.end() && *is != ms.fProcOrigin) is++;
		if(is != fSuspendAccessProcessors.end()) fSuspendAccessProcessors.push_back(ms.fProcOrigin);
		CheckTransitionState();
		this->VerifyAccessRequests();
		break;
	}
	case EGrantReadAccess: {
		if(fObjPtr && ms.fObjPtr) {
			cout << "OOPMetaData receives the pointer to the object again!\n";
			delete ms.fObjPtr;
			ms.fObjPtr = 0;
		}
		if(ms.fObjPtr) fObjPtr = ms.fObjPtr;
		ms.fObjPtr = 0;
		fProc = ms.fProcOrigin;
		fVersion = ms.fVersion;
		fReadAccessProcessors.push_back(DM->GetProcID());
		DataLog << "Grant read access received from proc " << ms.fProcOrigin << endl;
		this->VerifyAccessRequests();
		break;
	}
	case ECancelReadAccess: {
		DataLog << "Cancel read access received from " << ms.fProcOrigin << endl;
		fTrans = ECancelReadTransition;
		fProc = ms.fProcOrigin;
		CheckTransitionState();
		break;
	}
	case ECancelReadAccessConfirmation: {
		if(!IamOwner()) {
			DataLog << "HandleMessage cancel read access confirmation received by not owning proc\n" <<
				"obj " << fObjId << " proc " << DM->GetProcID() << endl;
			DataLog.flush();
			break;
		}
		DataLog << "Cancel read access confirmation received from " << ms.fProcOrigin << endl;
		list<int>::iterator i = fReadAccessProcessors.begin();
		while(i != fReadAccessProcessors.end()) {
			if(*i == ms.fProcOrigin) {
				fReadAccessProcessors.erase(i);
				break;
			}
		}
		break;
	}
	case ESuspendSuspendAccess: {
		DataLog << "Receiving Suspend Suspend Access from processor " << ms.fProcOrigin << endl;
		DataLog.flush();
		fVersion = ms.fVersion;
		list<int>::iterator i = fSuspendAccessProcessors.begin();
		while(i != fSuspendAccessProcessors.end()) {
			if(*i == DM->GetProcID()) {
				fSuspendAccessProcessors.erase(i);
				break;
			}
		}
		fProcVersionAccess = -1;
		fTaskVersion = OOPObjectId();
		if(IamOwner()) {
			i = fSuspendAccessProcessors.begin();
			while(i != fSuspendAccessProcessors.end()) {
				if(*i != this->fProc) {
					OOPDMOwnerTask *town = new OOPDMOwnerTask(ESuspendSuspendAccess,*i);
					town->fProcOrigin = DM->GetProcID();
					town->fObjId = fObjId;
					LogDM->SendOwnTask(town);
					TM->SubmitDaemon(town);
				}
				i++;
			}
		}
		fSuspendAccessProcessors.clear();
		VerifyAccessRequests();
		break;
	}
	case ETransferOwnership: {
		if(fObjPtr) {
			cout << "OOPMetaData receiving transfer ownership with pointer \n";
			DataLog << "OOPMetaData receiving transfer ownership with pointer \n";
		}
		DataLog << "Receiving transfer ownership for Obj " << fObjId << " from processor " << ms.fProcOrigin << endl;
		DataLog.flush();
		if(fObjPtr && ms.fObjPtr) delete fObjPtr;
		if(ms.fObjPtr) fObjPtr = ms.fObjPtr;
		ms.fObjPtr = 0;
		fVersion = ms.fVersion;
		fProc = DM->GetProcID();
		if(!HasReadAccess(fProc)) fReadAccessProcessors.push_back(fProc);
		this->VerifyAccessRequests();
		break;
	}
	case EGrantVersionAccess: {
		DataLog << "Grant version access received from proc " << ms.fProcOrigin << endl;
		fVersion = ms.fVersion;
		fProcVersionAccess = DM->GetProcID();
		fProc = ms.fProcOrigin;
		VerifyAccessRequests();
		if(fTaskVersion.IsZeroOOP()) {
			DataLog << "grantversion access for obj " << fObjId << " did not find corresponding task\n";
			VerifyAccessRequests();
			DataLog.flush();
			OOPDMOwnerTask *town = new OOPDMOwnerTask(ESuspendSuspendAccess,fProc);
			town->fProcOrigin = DM->GetProcID();
			town->fObjId = fObjId;
			LogDM->SendOwnTask(town);
			TM->SubmitDaemon(town);
			fSuspendAccessProcessors.clear();
		}
		break;
	}
	case ENotifyDeleteObject:
		DataLog << "Notify delete object received\n";
		this->DeleteObject();
		break;
	default:
		DataLog << "OOPMetaData::HandleMessage "<< fObjId << " unhandled message type " << ms.fType << endl;
		DataLog.flush();
		break;
	}
}
bool OOPMetaData::HasReadAccess () const
{
	return (fReadAccessProcessors.size () != 0);
}
bool OOPMetaData::HasVersionAccess () const
{
	if (fProcVersionAccess == DM->GetProcID ()) return true;
	if (fProcVersionAccess == -1 && IamOwner()) return true;
	return false;
}
bool OOPMetaData::HasReadAccess (const int Procid) const
{
	if(fTrans != ENoTransition) return false;
	if(fSuspendAccessProcessors.size()) return false;
#ifdef WIN32
	list < int >::const_iterator i = fReadAccessProcessors.begin ();
	while (i != fReadAccessProcessors.end () && *i != Procid)
		i++;
	if (i == fReadAccessProcessors.end ())
		return false;
	return true;
#else
	list < int >::const_iterator i = fReadAccessProcessors.begin ();
	i = find (fReadAccessProcessors.begin (),
		  fReadAccessProcessors.end (), Procid);
	if (i == fReadAccessProcessors.end ()) {
		return false;
	}
	else {
		return true;
	}
#endif
}
bool OOPMetaData::HasWriteAccess (const OOPObjectId & taskid) const
{
	if (fTaskWrite == taskid)
		return true;
	return false;
}
void OOPMetaData::DeleteObject ()
{
	DataLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	
	this->fToDelete = 1;
	DataLog << "deleting object " << fObjId << endl;
	LogDM->LogGeneric(DM->GetProcID(), fObjId, "deleting object");
	DataLog.flush();
	if (IamOwner()) {
		list<int>::iterator i = fReadAccessProcessors.begin();
		while(i != fReadAccessProcessors.end()) {
			if(*i != DM->GetProcID()) {
				OOPDMOwnerTask *town = new OOPDMOwnerTask(ENotifyDeleteObject,*i);
				town->fObjId = fObjId;
				LogDM->SendOwnTask(town);
				TM->SubmitDaemon(town);
			}
			i++;
		}
		fReadAccessProcessors.clear();
		fSuspendAccessProcessors.clear();
	}
	if (!(fProcVersionAccess != -1)) {
		OOPDMOwnerTask *town = new OOPDMOwnerTask(ENotifyDeleteObject,fProcVersionAccess);
		town->fObjId = fObjId;
		LogDM->SendOwnTask(town);
		TM->SubmitDaemon(town);
	}
	CheckTransitionState();
//	fAccessList.RevokeAccessAndCancel ();
//	if (!fAccessList.HasExecutingTasks ()) {
//		DM->DeleteObject (fObjId);
//	}
}
void OOPMetaData::RequestDelete ()
{
	DataLog << GLogMsgCounter << endl;
	DataLog << "Calling RequestDelete";
	LogDM->LogGeneric(DM->GetProcID(),fObjId,"Calling RequestDelete");
	DataLog.flush();
	GLogMsgCounter++;
	
	if (this->fProc == DM->GetProcID ()) {
		DeleteObject ();
	}
	else {
		DataLog << "OOPMetaData::RequestDelete should send an owner message " << fObjId << endl;
		DataLog.flush();
#ifndef WIN32
#warning "mandar um recado para o processador dono do dado"
#endif
	}
}
void OOPMetaData::CancelReadAccess ()
{
	list < int >::iterator i = fReadAccessProcessors.begin ();
	while(i != fReadAccessProcessors.end()) {
		if(*i != DM->GetProcID()) {
			OOPDMOwnerTask *town = new OOPDMOwnerTask(ECancelReadAccess,*i);
			town->fObjId = fObjId;
			LogDM->SendOwnTask(town);
			TM->SubmitDaemon(town);
		}
		i++;
	}
	// send suspend messages to all processors and revoke the access
	// granted
	fAccessList.RevokeAccess (*this);
	if (!fAccessList.HasExecutingTasks ()) {
		i = fReadAccessProcessors.begin ();
		while (i != fReadAccessProcessors.end ()) {
			if (*i == DM->GetProcID ()) {
				fReadAccessProcessors.erase (i);
				break;
			}
			i++;
		}
	}
	if (HasReadAccess ())
		fTrans = ECancelReadTransition;
}
void OOPMetaData::SuspendReadAccess ()
{
	DataLog << GLogMsgCounter << endl;
	DataLog << "Calling SuspendReadAccess\n";
	DataLog.flush();
	GLogMsgCounter++;
	// send suspend messages to all processors and revoke the access
	// granted
	fAccessList.RevokeAccess (*this);
	if (!fAccessList.HasExecutingTasks () && HasReadAccess()) {
		DataLog << "suspending read access for obj " << fObjId << " at proc " << DM->GetProcID() << endl;
		DataLog.flush();
		fSuspendAccessProcessors.push_back (DM->GetProcID ());
	}
	if (fReadAccessProcessors.size () != fSuspendAccessProcessors.size ()) {
		fTrans = ESuspendReadTransition;
	}
	int nread = fReadAccessProcessors.size();
	int iproc;
	list<int>::const_iterator ir = fReadAccessProcessors.begin();
	while(ir != fReadAccessProcessors.end()) {
		if(*ir != DM->GetProcID()) {
			DataLog << "Sending suspend read access for obj " << fObjId << " to proc " << *ir << endl;
			DataLog.flush();
			OOPDMOwnerTask *town = new OOPDMOwnerTask(ESuspendAccess, *ir);
			//alterei aqui
			town->fObjId=fObjId;
			town->fObjPtr = this->fObjPtr;
			town->fVersion = this->fVersion;
			LogDM->SendOwnTask(town);
			TM->SubmitDaemon(town);
		}
		ir++;
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
	DataLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	DataLog << "grant read access for obj " << fObjId << " is used?\n";
	DataLog.flush();
}
void OOPMetaData::GrantVersionAccess (OOPObjectId TaskId, int ProcId,
				      OOPMDataState AccessRequest,
				      OOPDataVersion version)
{
}
void OOPMetaData::GrantAccess (OOPMDataState state, int processor)
{
	DataLog << GLogMsgCounter << endl;
	DataLog << "Entering GrantAccess for Object " << this->fObjId << endl;
	GLogMsgCounter++;
	switch(state) {
	case EVersionAccess: {
		DataLog << "Sending grant version access for obj " << fObjId << " to proc " << processor << endl;
		DataLog.flush();
		OOPDMOwnerTask *town = new OOPDMOwnerTask(EGrantVersionAccess,processor);
		town->fObjId = this->fObjId;
		town->fVersion = this->fVersion;
		town->fProcOrigin = DM->GetProcID();
		TM->SubmitDaemon(town);
		fProcVersionAccess = processor;
		LogDM->SendGrantAccessLog(town,processor);
		break;
	}
	case EReadAccess : {
		DataLog << "Sending grant read access for obj " << fObjId << " to proc " << processor << endl;
		DataLog.flush();
		OOPDMOwnerTask *town = new OOPDMOwnerTask(EGrantReadAccess,processor);
		town->fObjId = this->fObjId;
		town->fObjPtr = this->fObjPtr;
		town->fVersion = this->fVersion;
		town->fProcOrigin = DM->GetProcID();
		TM->SubmitDaemon(town);
		LogDM->SendGrantAccessLog(town,processor);
		this->fReadAccessProcessors.push_back(processor);
		break;
	}
	case EWriteAccess: 
		DataLog << "OOPMetaData::GrantAccess transferring object" << fObjId << " State "
			<< state << " to proc " << processor << endl;
		DataLog.flush();
		TransferObject(processor);
		break;
	default:
		DataLog << "OOPMetaData::GrantAccess " << fObjId << " unhandled state " << state << endl;
		DataLog.flush();
		
	break;
	}
}
OOPDataVersion OOPMetaData::Version () const
{
	return fVersion;
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
	case ENoMessage:
		tout << "\tENoMessage" << endl;
		break;
	case ECancelReadAccess:{
		tout << "\tECancelReadAccess" << endl;
		break;
	}
	case ECancelReadAccessConfirmation:
		tout << "\tECancelReadAccessConfirmation" << endl;
		break;
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
	default:
		tout << "\tCannot Trace!" << endl;
		break;
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
void OOPMetaData::Print (ostream & out)
{
	out << "\nObj Id " << fObjId << " version " << fVersion
		<< " processor " << fProc << endl;
	out << " OOPData structure" << endl;
	out << "fAccessList size " << fAccessList.NElements () << endl;
	fAccessList.Print(out);
	out.flush ();
}
void OOPMetaData::PrintLog (ostream & out)
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
	DataLog << GLogMsgCounter << endl;
	
	GLogMsgCounter++;
	if (fTaskWrite == taskid || fTaskVersion == taskid) {
		LogDM->LogSetVersion(DM->GetProcID(),fObjId,fVersion,ver, State(),taskid);
		fVersion = ver;
		DataLog << "Setting Version for Obj " << this->fObjId << " to version "
			<< ver << "\n";
		
	}
	else {
		DataLog << "OOPMetaData::SetVersion not executed for Obj "<< fObjId << "\n";
	}
	DataLog.flush();
}
void OOPMetaData::SendAccessRequest (const OOPMDataDepend & depend)
{
	OOPDMRequestTask *req = new OOPDMRequestTask (fProc, depend);
	LogDM->SendReqTask(req);
	TM->SubmitDaemon (req);
}
