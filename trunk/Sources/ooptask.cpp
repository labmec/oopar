#include "ooptask.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"
#include "oopcommmanager.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>
//#include "../gnu/gnudefs.h"
void OOPTask::SetRecurrence (bool recurrence)
{
	fIsRecurrent = recurrence;
}
int OOPTask::IsRecurrent ()
{
	return fIsRecurrent;
}
class OOPSendStorage;
class OOPReceiveStorage;
class OOPObjectId;
class OOPDataVersion;
class OOPDaemonTask;
class OOPMDataDepend;
class OOPSaveable;
using namespace std;
void OOPTask::Print (ostream & out)
{
	out << "OOPTask Id" << fTaskId << endl;
//	fTaskId.Print (out);
	out << "Priority\t" << fPriority << endl;
	out << "Processor\t" << fProc << endl;
	out << "Data Dependence\t" << endl;
	fDataDepend.Print (out);
}
void OOPTask::TaskFinished ()
{
//  fDataDepend.ReleaseAccessRequests(fTaskId);
// Is taken care of by the task manager
}
OOPTask::OOPTask (int proc)
{
	fProc = proc;
	fPriority = 0;
	fIsRecurrent = 0;
}
OOPTask::OOPTask (const OOPTask & task):fProc (task.fProc),
fTaskId (), fDataDepend (task.fDataDepend),
fPriority (task.fPriority), fIsRecurrent (task.fIsRecurrent)
{
}
void OOPTask::AddDependentData (const OOPMDataDepend & depend)
{
	fDataDepend.AppendDependency (depend);
}
void OOPTask::PrintLog(ostream & out, char * message){
//	out << GLogMsgCounter << endl;
	out << "Task:" << fTaskId.GetProcId()<< ":" << fTaskId.GetId() << ":" << message << ":" << GLogMsgCounter << endl;
	GLogMsgCounter++;
	out.flush();
}
long OOPTask::ExecTime ()
{
	return -1;
}
OOPObjectId OOPTask::Submit ()
{
	fTaskId = TM->Submit (this);
	return fTaskId;
}
OOPDaemonTask::OOPDaemonTask (int Procid):OOPTask (Procid)
{
}
long OOPDaemonTask::ExecTime ()
{
	return 0;
}
int OOPDaemonTask::CanExecute ()
{
	return 1;
}
OOPMReturnType OOPTask::Execute ()
{
	return ESuccess;	// execute the task, verifying that
}
int OOPTask::GetProcID ()
{
	return fProc;
}
void OOPTask::ChangePriority (int newpriority)
{
	fPriority = newpriority;
}
int OOPTask::Priority ()
{
	return fPriority;
}
OOPObjectId OOPTask::Id ()
{
	return fTaskId;
}
OOPSaveable *OOPTask::Restore (OOPReceiveStorage * buf)
{
	OOPTask *v = new OOPTask (0);
	v->Unpack (buf);
	return v;
}
int OOPTask::Pack (OOPSendStorage * buf)
{
	OOPSaveable::Pack (buf);
	// ObjectId packing and unpacking
	fTaskId.Pack (buf);
	buf->PkInt (&fProc);	// Processor where the task should be
	// executed
	buf->PkInt (&fPriority);
	buf->PkInt (&fIsRecurrent);
	fDataDepend.Pack (buf);
	return 0;
}
int OOPTask::Unpack (OOPReceiveStorage * buf)
{
	OOPSaveable::Unpack (buf);
	fTaskId.Unpack (buf);
	// Finished OOPObjectId unpacking
	buf->UpkInt (&fProc);
	buf->UpkInt (&fPriority);
	buf->UpkInt (&fIsRecurrent);
	fDataDepend.Unpack (buf);
	return 0;
}
OOPSaveable *OOPDaemonTask::Restore (OOPReceiveStorage * buf)
{
	OOPDaemonTask *v = new OOPDaemonTask (0);
	v->Unpack (buf);
	return v;
}
