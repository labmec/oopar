#include "oopmdatadepend.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopmetadata.h"
OOPMDataDepend::OOPMDataDepend (const OOPObjectId & id, OOPMDataState st,
				const OOPDataVersion & ver)
{
	fDataId = id;
	fNeed = st;
	fVersion = ver;
	fObjPtr = 0;
}
OOPMDataDepend::OOPMDataDepend (const::OOPMDataDepend & dd)
{
	fDataId = dd.fDataId;
	fNeed = dd.fNeed;
	fVersion = dd.fVersion;
	fObjPtr = dd.fObjPtr;
}
OOPMetaData *OOPMDataDepend::ObjPtr ()
{
	return fObjPtr;
}
void OOPMDataDepend::SetObjPtr (OOPMetaData * objptr)
{
	fObjPtr = objptr;
}
bool OOPMDataDepend::operator == (const OOPMDataDepend & dd) const
{
	return ((fDataId == dd.fDataId && fNeed == dd.fNeed && fVersion == dd.fVersion));	// return 
												// true;
	// return false;
}
void OOPMDataDepend::Print (ostream & out) const
{
	out << "Data Id" << endl;
	fDataId.Print (out);
	out << "Access State \t" << fNeed << endl;
	out << "Object Pointer \t" << fObjPtr << endl;
	out << "Required version " << endl;
	fVersion.Print (out);
}
ostream &OOPMDataDepend::ShortPrint(ostream &out) const {
	out << "obj " << fDataId << " acc " << fNeed << " vers " << fVersion;
	return out;
}
void OOPMDataDepend::Pack (OOPSendStorage * buf)
{
	fDataId.Pack (buf);
	int need = fNeed;
	buf->PkInt (&need);
	fVersion.Pack (buf);
	if (fObjPtr) {
		cout << "OOPMDataDepend::Pack the object pointer should be zero\n";
	}
}
  /**
   * method to reconstruct the object
   */
int OOPMDataDepend::Unpack (OOPReceiveStorage * buf)
{
	fDataId.Unpack (buf);
	int need;
	buf->UpkInt (&need);
	fNeed = (OOPMDataState) need;
	fVersion.Unpack (buf);
	fObjPtr = 0;
	return 1;
}
int OOPMDataDependList::SubmitDependencyList (const OOPObjectId & taskid)
{
	if (fDependList.size () == 0) {
		TM->TransfertoExecutable (taskid);
		return 1;
	}
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		if (!DM->SubmitAccessRequest (taskid, *i)) {
			cout << "OOPMDataDependList::SubmitDependencyList failed for task id ";
			taskid.Print (cout);
			cout << "With dependency ";
			(*i).Print (cout);
			deque < OOPMDataDepend >::iterator j;
			for (j = fDependList.begin (); j != i; j++) {
				DM->ReleaseAccessRequest (taskid, *j);
			}
			return 0;
		}
	}
	return 1;
}
void OOPMDataDependList::SetExecuting (const OOPObjectId & taskid,
				       bool condition)
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		i->ObjPtr ()->SetExecute (taskid, *i, condition);
	}
}
void OOPMDataDependList::ReleaseAccessRequests (const OOPObjectId & taskid)
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		DM->ReleaseAccessRequest (taskid, *i);
	}
}
void OOPMDataDependList::ClearPointers ()
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		(*i).SetObjPtr (0);
	}
}
void OOPMDataDependList::Print (ostream & out)
{
	out << "OOPMDataDependList printout\n";
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		i->ShortPrint (out) << endl;
	}
}
int OOPMDataDependList::NElements ()
{
	return fDependList.size ();
}
void OOPMDataDependList::AppendDependency (const OOPMDataDepend & depend)
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		if (*i == depend) {
			cerr << "OOPMDataDependList::AppendDependency duplicate dependency __FILE__ __LINE__ \n";
		}
	}
	fDependList.push_back (depend);
}
void OOPMDataDependList::GrantAccess (const OOPMDataDepend & depend,
				      OOPMetaData * ObjPtr)
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		if (*i == depend && !i->Status ()) {
			i->SetObjPtr (ObjPtr);
			break;
		}
	}
	if (i == fDependList.end ()) {
		cerr << "OOPMDataDepend::GrantAccess didn't find the corresponding dependency\n";
		depend.ShortPrint (cerr);
		cerr << endl;
		Print (cerr);
	}
}
void OOPMDataDependList::RevokeAccess (const OOPMDataDepend & depend)
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		if (*i == depend && i->Status ()) {
			i->SetObjPtr (0);
			break;
		}
	}
	if (i == fDependList.end ()) {
		cerr << "OOPMDataDepend::RevokeAccess didn't find the corresponding dependency\n";
		depend.ShortPrint (cerr);
		cerr << endl;
		Print (cerr);
	}
}
void OOPMDataDependList::Clear ()
{
	fDependList.clear ();
}
int OOPMDataDependList::CanExecute ()
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		if (!i->Status ())
			return 0;
	}
	return 1;
}
void OOPMDataDependList::Pack (OOPSendStorage * buf)
{
	deque < OOPMDataDepend >::iterator i;
	int nel = fDependList.size ();
	buf->PkInt (&nel);
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		i->Pack (buf);
	}
}
  /**
   * method to reconstruct the object
   */
int OOPMDataDependList::Unpack (OOPReceiveStorage * buf)
{
	int nel;
	buf->UpkInt (&nel);
	int id;
	OOPMDataDepend temp;
	for (id = 0; id < nel; id++) {
		temp.Unpack (buf);
		fDependList.push_back (temp);
	}
	return 1;
}
