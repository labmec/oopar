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
	OOPObjectId taskid;
	while (fAccessList.HasIncompatibleTask (fVersion, taskid)) {
		cout << "OOPMetaData::Verify.. task canceled ";
		taskid.Print (cout);
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
//              this->fProcVersionAccess = fProc;
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
		if (ac->fProcessor == DM->GetProcID ()) {
			ac->fIsGranted = 1;
			OOPMDataDepend depend (this->Id (), ac->fState,
					       ac->fVersion);
			if (ac->fState == EWriteAccess)
				fTaskWrite = ac->fTaskId;
			if (ac->fState == EVersionAccess)
				fTaskVersion = ac->fTaskId;
			TM->NotifyAccessGranted (ac->fTaskId, depend, this);
		}
		else {
			GrantAccess (ac->fState, ac->fProcessor);
			cout << "OOPMetaData::VerifyAccessRequest Send a grant access message to the processor" << endl;
#ifndef WIN32
#warning "Send a grant access message to the processor"
#endif
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
	if (!fTaskVersion.IsZero ())
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
	fAccessList.ReleaseAccess (taskid, depend);
	if (depend.State () == EVersionAccess) {
		this->fProcVersionAccess = -1;
		this->fTaskVersion.Zero ();
		// remove this processor from the list of suspended read
		// access
		list < int >::iterator i = fSuspendAccessProcessors.begin ();
		while (i != fSuspendAccessProcessors.end ()) {
			if (*i == DM->GetProcID ()) {
				fSuspendAccessProcessors.erase (i);
				break;
			}
			i++;
		}
		cout << "OOPMetaData::ReleaseAccess Send messages "
			" granting read access to the other processors" <<
			endl;
#ifndef WIN32
#warning "Send messages to the other processors"
#endif
	}
	else if (depend.State () == EWriteAccess) {
		fTaskWrite.Zero ();
		// grant read access to the owning processor
		fReadAccessProcessors.push_back (fProc);
	}
	CheckTransitionState ();
	VerifyAccessRequests ();
}

void OOPMetaData::CheckTransitionState ()
{
	if (fToDelete && !fAccessList.HasExecutingTasks ()) {
		DM->DeleteObject (fObjId);
		return;
	}
	if (fTrans == ENoTransition)
		return;
	if (!fAccessList.HasExecutingTasks ()) {
		if (fTrans == ECancelReadTransition) {
			list < int >::iterator i =
				fReadAccessProcessors.begin ();
			while (i != fReadAccessProcessors.end ()) {
				if (*i == DM->GetProcID ()) {
					fReadAccessProcessors.erase (i);
					if (!IamOwner ()) {
#ifndef WIN32
#warning "must send cancelreadaccessconfirmation"
#endif

						delete this->fObjPtr;
						fObjPtr = 0;
						cout << "OOPMetaData::CheckTransitionState must send CancelReadAccessConfirmation\n";
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
				fSuspendAccessProcessors.push_back (DM->
								    GetProcID
								    ());
				if (!IamOwner ()) {
#ifndef WIN32
#warning "must send cancelsuspendaccessconfirmation"
#endif
					cout << "OOPMetaData::CheckTransitionState must send SuspendAccessConfirmation\n";
				}
			}
		}
	}
	if (fTrans == ECancelReadTransition) {
		if (!fReadAccessProcessors.size ()) {
			fTrans = ENoTransition;
		}
	}
	else if (fTrans == ESuspendReadTransition) {
		if (fSuspendAccessProcessors.size () ==
		    fReadAccessProcessors.size ()) {
			fTrans = ENoTransition;
		}
	}
}



OOPMDataState OOPMetaData::State () const
{
	if (fTaskWrite.IsZero ()) {
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
	fAccessList.AddAccessRequest (taskId, depend, processor);
	VerifyAccessRequests ();
	if (!IamOwner () && !fAccessList.HasAccessGranted (taskId, depend)) {
		cout << "OOPMetaData::SubmitAccessRequest must debug\n";
		this->SendAccessRequest (depend);
	}
}

void OOPMetaData::SetExecute (const OOPObjectId & taskId,
			      const OOPMDataDepend & depend, bool condition)
{
	fAccessList.SetExecute (taskId, depend, condition);
}



OOPMetaData::OOPMetaData (OOPSaveable * ObPtr, const OOPObjectId & ObjId,
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
	cout << "OOPMetaData::TransferObject not implemented\n";
#ifndef WIN32
#warning "OOPMetaData::TransferObject is not implemented"
#endif
}


void OOPMetaData::HandleMessage (OOPDMOwnerTask & ms)
{
}

bool OOPMetaData::HasReadAccess () const
{
	return (fReadAccessProcessors.size () != 0);
}

bool OOPMetaData::HasVersionAccess () const
{
	if (fProcVersionAccess == DM->GetProcID ())
		return true;
	return false;
}

bool OOPMetaData::HasReadAccess (const int Procid) const
{
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
	this->fToDelete = 1;
	if (fReadAccessProcessors.size ()) {
#ifndef WIN32
#warning "mandar recado para deletar os objetos nesses processadores"
#endif
	}
	if (!(fProcVersionAccess != -1)) {
#ifndef WIN32
#warning "mandar recado para deletar os objetos nesse processador"
#endif
	}
	fAccessList.RevokeAccessAndCancel ();
	if (!fAccessList.HasExecutingTasks ()) {
		DM->DeleteObject (fObjId);
	}
}


void OOPMetaData::RequestDelete ()
{
	if (this->fProc == DM->GetProcID ()) {
		DeleteObject ();
	}
	else {
#ifndef WIN32
#warning "mandar um recado para o processador dono do dado"
#endif
	}
}


void OOPMetaData::CancelReadAccess ()
{

#ifndef WIN32
#warning "Send messages to other processors"
#endif

	// send suspend messages to all processors and revoke the access
	// granted
	fAccessList.RevokeAccess (*this);
	if (!fAccessList.HasExecutingTasks ()) {
		list < int >::iterator i = fReadAccessProcessors.begin ();
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

	// send suspend messages to all processors and revoke the access
	// granted
	fAccessList.RevokeAccess (*this);
	if (!fAccessList.HasExecutingTasks ()) {
		fSuspendAccessProcessors.push_back (DM->GetProcID ());
	}
	if (fReadAccessProcessors.size () != fSuspendAccessProcessors.size ()) {
		fTrans = ESuspendReadTransition;
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

}
void OOPMetaData::GrantVersionAccess (OOPObjectId TaskId, int ProcId,
				      OOPMDataState AccessRequest,
				      OOPDataVersion version)
{
}

void OOPMetaData::GrantAccess (OOPMDataState state, int processor)
{

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
	out << "TData structure" << endl;
	out << "fAccessList size " << fAccessList.NElements () << endl;
	out.flush ();
}

void OOPMetaData::SetVersion (const OOPDataVersion & ver,
			      const OOPObjectId & taskid)
{
	if (fTaskWrite == taskid || fTaskVersion == taskid) {
		fVersion = ver;
	}
	else {
		cout << "OOPMetaData::SetVersion not executed\n";
	}
}

long OOPMetaData::GetClassID ()
{
	return OOPMETADATA_ID;
}

void OOPMetaData::SendAccessRequest (const OOPMDataDepend & depend)
{

	OOPDMRequestTask *req = new OOPDMRequestTask (fProc, depend);
	TM->Submit (req);
}
