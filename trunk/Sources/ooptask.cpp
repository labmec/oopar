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
class OOPStorageBuffer;
class OOPStorageBuffer;
class OOPObjectId;
class OOPDataVersion;
class OOPDaemonTask;
class OOPMDataDepend;
class OOPSaveable;
using namespace std;
void OOPTask::Print (ostream & out)
{
	out << "OOPTask Id" << fTaskId << endl;
	out << fLabel << endl;
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
	fLabel="non initialized";
}
OOPTask::OOPTask (const OOPTask & task):fProc (task.fProc),
fTaskId (), fDataDepend (task.fDataDepend),
fPriority (task.fPriority), fIsRecurrent (task.fIsRecurrent),
fLabel(task.fLabel)
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
	cout << "OOPTask::Execute should never be called!\n";
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
TPZSaveable *OOPTask::Restore (TPZStream * buf)
{
	OOPTask *v = new OOPTask (0);
	v->Read (buf);
	return v;
}
int OOPTask::Write (TPZStream * buf, int nel)
{
	TPZSaveable::Write (*buf, 1);
	// ObjectId packing and unpacking
	fTaskId.Write (buf);
	buf->Write (&fProc);	// Processor where the task should be
	// executed
	buf->Write (&fPriority);
	buf->Write (&fIsRecurrent);
	fDataDepend.Write (buf);
	return 0;
}
int OOPTask::Read (TPZStream * buf, int nel)
{
	TPZSaveable::Read(*buf);
	fTaskId.Read (buf);
	// Finished OOPObjectId unpacking
	buf->Read (&fProc);
	buf->Read (&fPriority);
	buf->Read (&fIsRecurrent);
	fDataDepend.Read(buf);
	return 0;
}
TPZSaveable *OOPDaemonTask::Restore (TPZStream * buf)
{
	OOPDaemonTask *v = new OOPDaemonTask (0);
	v->Read (buf);
	return v;
}
