#include "oopmdatadepend.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopmetadata.h"
class OOPStorageBuffer;

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMDataDepend"));
static LoggerPtr tasklogger(Logger::getLogger("OOPAR.OOPTaskManager.task"));


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
void OOPMDataDepend::Print (std::ostream & out) const
{
	out << "Data Id" << endl;
	fDataId.Print (out);
	out << "Access State \t" << fNeed << endl;
	out << "Object Pointer \t" << fObjPtr << endl;
	out << "Required version " << endl;
	fVersion.Print (out);
}
std::ostream &OOPMDataDepend::ShortPrint(std::ostream &out) const {
	out << "obj " << fDataId << " acc " << fNeed << " vers " << fVersion;
	return out;
}

void OOPMDataDepend::LogMe(std::ostream &out) {
	out << "regarding obj " << fDataId << "\t\tState ";
	/*
	ENoAccess,
	EReadAccess,
	EWriteAccess,
	EVersionAccess

	*/
	switch (fNeed )
	{
		case  ENoAccess:
			out << "ENoAccess\t";
			break;
		case  EReadAccess:
			out << "EReadAccess\t";
			break;
		case  EWriteAccess:
			out << "EWriteAccess\t";
			break;
		case  EVersionAccess:
			out << "EVersionAccess\t";
			break;
		default:
			out << "Uninitialized fNeed attribute\t";
			break;
	}
	out << "Version " << fVersion;
}

void OOPMDataDepend::Write (TPZStream  & buf)
{
	fDataId.Write (buf);
	int need = fNeed;
	buf.Write (&need);
	fVersion.Write(buf);
	if (fObjPtr) {
    LOG4CXX_ERROR(logger, "Pack the object pointer should be zero");
	}
}
  /**
   * method to reconstruct the object
   */
void OOPMDataDepend::Read (TPZStream & buf, void * context)
{
	fDataId.Read (buf);
	int need;
	buf.Read (&need);
	fNeed = (OOPMDataState) need;
	fVersion.Read(buf);
	fObjPtr = 0;
	
}
int OOPMDataDependList::SubmitDependencyList (const OOPObjectId & taskid)
{
	if (fDependList.size () == 0) {
		TM->TransfertoExecutable (taskid);
		return 1;
	}
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
   {
     stringstream sout;
     sout << __PRETTY_FUNCTION__ << " task id " << taskid << " submitting " << *i;
     LOG4CXX_DEBUG(tasklogger,sout.str());
   }
		if (!DM->SubmitAccessRequest (taskid, *i)) {
      stringstream sout;
      sout << "SubmitDependencyList failed for task id ";
      taskid.Print (sout);
      sout << "With dependency ";
      (*i).Print (sout);
      LOG4CXX_ERROR(logger,sout.str());
			deque < OOPMDataDepend >::iterator j;
			for (j = fDependList.begin (); j != i; j++) {
				DM->ReleaseAccessRequest (taskid, *j);
			}
			fDependList.clear();
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
void OOPMDataDependList::Print (std::ostream & out)
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
      stringstream sout;
			sout << "OOPMDataDependList::AppendDependency duplicate dependency ";
      LOG4CXX_WARN(logger,sout.str());
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
    stringstream sout;
    sout << "OOPMDataDepend::GrantAccess didn't find the corresponding dependency : ";
    depend.ShortPrint (sout);
    LOG4CXX_WARN(logger,sout.str());
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
    stringstream sout;
    sout << "OOPMDataDepend::RevokeAccess didn't find the corresponding dependency";
    depend.ShortPrint (sout);
    LOG4CXX_WARN(logger,sout.str());
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
void OOPMDataDependList::Write (TPZStream  & buf)
{
	deque < OOPMDataDepend >::iterator i;
	int nel = fDependList.size ();
	buf.Write (&nel);
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		i->Write (buf);
	}
}
  /**
   * method to reconstruct the object
   */
void OOPMDataDependList::Read (TPZStream  & buf, void * context)
{
	int nel;
	buf.Read (&nel);
	int id;
	OOPMDataDepend temp;
	for (id = 0; id < nel; id++) {
		temp.Read (buf);
		fDependList.push_back (temp);
	}
	
}

OOPMDataDepend & OOPMDataDependList::Dep (OOPObjectId & Id)	{
	deque< OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		if (i->Id() == Id)
			return *(i);
	}
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Wrong Dependency information";
  LOG4CXX_WARN(logger,sout.str());
	return * new OOPMDataDepend;
}
