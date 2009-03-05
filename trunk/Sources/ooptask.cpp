#include "ooptask.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"
#include "oopcommmanager.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>
//#include "../gnu/gnudefs.h"

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace
  log4cxx::helpers;
static LoggerPtr
logger (Logger::getLogger ("OOPAR.OOPTask"));
#endif



class OOPStorageBuffer;
class OOPStorageBuffer;
class OOPObjectId;
class OOPDataVersion;
class OOPDaemonTask;
class OOPMDataDepend;
class OOPSaveable;
using namespace std;

template class TPZRestoreClass<OOPTask, TTASK_ID>;

void
OOPTask::Print (std::ostream & out)
{
  out << "OOPTask Id" << fTaskId << endl;
  out << fLabel << endl;
  out << "Processor\t" << fProc << endl;
  out << "Data Dependence\t" << endl;
  fDependRequest.Print(out);
}

OOPTask::OOPTask (int proc)
{
  fProc = proc;
  fLabel = "non initialized";
}
OOPTask::OOPTask (const OOPTask & task):
  fProc (task.fProc),
  fTaskId (),
  fDependRequest (task.fDependRequest),
  fLabel (task.fLabel)
{
}
void
OOPTask::AddDependentData (const OOPAccessTag & depend)
{
  fDependRequest.AppendTag(depend);
}

void
OOPTask::PrintLog (std::ostream & out, char *message)
{
  out << "Task:" << fTaskId.GetProcId () << ":" << fTaskId.
    GetId () << ":" << message;
}

OOPObjectId
OOPTask::Submit ()
{
  OOPObjectId val = TM->Submit (this);
  return val;
}


template class TPZRestoreClass<OOPDaemonTask, TDAEMONTASK_ID>;


OOPDaemonTask::OOPDaemonTask (int Procid):OOPTask (Procid)
{
}

int
OOPDaemonTask::CanExecute ()
{
  return 1;
}

OOPMReturnType
OOPTask::Execute ()
{
  //IncrementWriteDependentData ();
/*  stringstream sout;
  sout << "OOPTask::Execute should never be called!\n";
  sout << "Called from ClassId " << ClassId() << endl;
  LOGPZ_ERROR(logger, sout.str().c_str());*/
  return ESuccess;		// execute the task, verifying that
}

int
OOPTask::GetProcID ()
{
  return fProc;
}


OOPObjectId
OOPTask::Id ()
{
  return fTaskId;
}

void
OOPTask::Write (TPZStream & buf, int withclassid)
{
  TPZSaveable::Write (buf, withclassid);
  // ObjectId packing and unpacking
  fTaskId.Write (buf);
  //Logging purpose only
  buf.Write (&fProc);		// Processor where the task should be
  // executed
  fDependRequest.Write(buf,0);//, withclassid);
#ifdef LOGPZ
  stringstream sout;
  sout << "Writing Task ClassId: " << ClassId() << " ObjID: " << Id() << " Depend list \n";
  fDependRequest.ShortPrint( sout);
  LOGPZ_DEBUG(logger, sout.str ());
#endif

}

void
OOPTask::Read (TPZStream & buf, void *context)
{
  TPZSaveable::Read (buf, context);
  fTaskId.Read (buf);
  // Finished OOPObjectId unpacking
  //Logging purpose only
  buf.Read (&fProc);
  fDependRequest.Read (buf, context);
#ifdef LOGPZ
  stringstream sout;
  sout << "Read Task ClassId: " << ClassId() << " ObjID:" << Id();
  fDependRequest.ShortPrint( sout);
  LOGPZ_DEBUG(logger, sout.str().c_str());
#endif

}

TPZSaveable *
OOPTask::GetDepObjPtr (int idepend)
{
  int numdep = fDependRequest.Count();
  if (idepend < 0 || idepend >= numdep) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " depend index is larger than numdep " <<
      idepend << " " << numdep;
    LOGPZ_WARN (logger, sout.str().c_str());
#endif
    return 0;
  }
  return fDependRequest.ObjectPtr(idepend);
}

void
OOPTask::IncrementWriteDependentData ()
{
  fDependRequest.IncrementWriteDependent();
  TM->WakeUpCall();
}

void OOPTask::SubmitDependencyList()
{
#ifdef LOGPZ
    stringstream sout;
    sout << "Submitting Access Requests for Task " << fTaskId;
    LOGPZ_DEBUG(logger, sout.str().c_str());
#endif
  fDependRequest.PostRequests(fTaskId);
}

OOPAccessTag OOPTask::GetTag(int i)
{
	return fDependRequest.GetTag(i);
}

