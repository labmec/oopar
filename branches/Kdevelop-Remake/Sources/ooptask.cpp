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

void
OOPTask::SetRecurrence (bool recurrence)
{
  fIsRecurrent = recurrence;
}

int
OOPTask::IsRecurrent ()
{
  return fIsRecurrent;
}

class OOPStorageBuffer;
class OOPStorageBuffer;
class OOPObjectId;
class OOPDataVersion;
class OOPDaemonTask;
class OOPMDataDepend;
class OOPSaveable;
using namespace std;
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
  fIsRecurrent = 0;
  fLabel = "non initialized";
}
OOPTask::OOPTask (const OOPTask & task):
  fProc (task.fProc),
  fTaskId (),
  fDependRequest (task.fDependRequest),
  fIsRecurrent (task.fIsRecurrent),
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
  cout << "Inside OOPTask Submit ---------\n";
  OOPObjectId val = TM->Submit (this);
//      fTaskId = TM->Submit (this);
  return val;
}

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
  IncrementWriteDependentData ();
/*  stringstream sout;
  sout << "OOPTask::Execute should never be called!\n";
  sout << "Called from ClassId " << ClassId() << endl;
  LOGPZ_ERROR(logger, sout.str());*/
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
  buf.Write (&fIsRecurrent);
  fDependRequest.Write (buf);

}

void
OOPTask::Read (TPZStream & buf, void *context)
{
  TPZSaveable::Read (buf, context);
  fTaskId.Read (buf);
  // Finished OOPObjectId unpacking
  //Logging purpose only
  buf.Read (&fProc);
  buf.Read (&fIsRecurrent);
  fDependRequest.Read (buf, context);

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
    LOGPZ_WARN (logger, sout.str ());
#endif
    return 0;
  }
  return fDependRequest.ObjectPtr(idepend);
}

void
OOPTask::IncrementWriteDependentData ()
{
  fDependRequest.IncrementWriteDependent();
}

void OOPTask::SubmitDependencyList()
{
// DM->SubmitAccessRequest();
#warning "Submit something to DM - > Check what exactaly"
}
