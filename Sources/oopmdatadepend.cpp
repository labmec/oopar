#include "oopmdatadepend.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopmetadata.h"
class OOPStorageBuffer;

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMDataDepend"));
static LoggerPtr tasklogger(Logger::getLogger("OOPAR.OOPTaskManager.task"));
#endif


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

   /**
    *  operator = to copy the content
    */
OOPMDataDepend &OOPMDataDepend::operator=(const OOPMDataDepend &copy)
{
	fDataId = copy.fDataId;
	fNeed = copy.fNeed;
	fVersion = copy.fVersion;
	fObjPtr = copy.fObjPtr;
	return *this;
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
	out << "Data Id" << std::endl;
	fDataId.Print (out);
	out << "Access State \t" << fNeed << std::endl;
	out << "Object Pointer \t" << fObjPtr << std::endl;
	out << "Required version " << std::endl;
	fVersion.Print (out);
}
std::ostream &OOPMDataDepend::ShortPrint(std::ostream &out) const {
	out << "obj " << fDataId << " acc " << fNeed << " vers " << fVersion;
	return out;
}
std::ostream &OOPMDataDepend::SuperShortPrint(std::ostream &out) const {
	out << fDataId << ":" << fNeed << ":" << fVersion;
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
    LOGPZ_ERROR(logger, "Pack the object pointer should be zero");
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
#ifdef LOGPZ     
     stringstream sout;
     sout << __PRETTY_FUNCTION__ << " task id " << taskid << " submitting " << *i;
     LOGPZ_DEBUG(tasklogger,sout.str());
#endif     
   }
		if (!DM->SubmitAccessRequest (taskid, *i)) {
#ifdef LOGPZ      
      stringstream sout;
      sout << "SubmitDependencyList failed for task id ";
      taskid.Print (sout);
      sout << "With dependency ";
      (*i).Print (sout);
      LOGPZ_ERROR(logger,sout.str());
#endif      
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
#ifdef LOGPZ    
          std::stringstream sout;
          sout << __PRETTY_FUNCTION__ << " taskid " << taskid << " releasing depend " << *i;
          LOGPZ_DEBUG(tasklogger,sout.str());
#endif          
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
		i->ShortPrint (out) << " status " << i->Status() << endl;
	}
}
void OOPMDataDependList::ShortPrint (std::ostream & out)
{
	deque < OOPMDataDepend >::iterator i;
	for (i = fDependList.begin (); i != fDependList.end (); i++) {
		i->SuperShortPrint (out) << " st " << i->Status() << ";";
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
#ifdef LOGPZ      
      stringstream sout;
      sout << "OOPMDataDependList::AppendDependency duplicate dependency ";
      LOGPZ_WARN(logger,sout.str());
#endif      
		}
	}
	fDependList.push_back (depend);
}
void OOPMDataDependList::GrantAccess (const OOPMDataDepend & depend, OOPMetaData * ObjPtr)
{
  deque < OOPMDataDepend >::iterator i;
  for (i = fDependList.begin (); i != fDependList.end (); i++)
  {
    if (*i == depend && !i->Status ())
    {
      i->SetObjPtr (ObjPtr);
      break;
    }
  }
  if (i == fDependList.end ())
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPMDataDepend::GrantAccess didn't find the corresponding dependency : ";
    depend.ShortPrint (sout);
    sout << std::endl << "My dependency list " << std::endl;
    Print(sout);
    LOGPZ_ERROR(logger,sout.str());
#endif    
  }
}
void OOPMDataDependList::RevokeAccess (const OOPMDataDepend & depend)
{
  deque < OOPMDataDepend >::iterator i;
  for (i = fDependList.begin (); i != fDependList.end (); i++)
  {
    if (*i == depend && i->Status ())
    {
      i->SetObjPtr (0);
      break;
    }
  }
  if (i == fDependList.end ())
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPMDataDepend::RevokeAccess didn't find the corresponding dependency, dependencies";
    for(i = fDependList.begin(); i!= fDependList.end(); i++)
    {
      i->ShortPrint(sout);
    }
    sout << " dependency not found : ";
    depend.ShortPrint (sout);
    LOGPZ_ERROR(logger,sout.str());
#endif    
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
#ifdef LOGPZ    
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Wrong Dependency information";
  LOGPZ_WARN(logger,sout.str());
#endif  
	return * new OOPMDataDepend;
}
