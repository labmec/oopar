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
	OOPObjectId val = TM->Submit(this);
//	fTaskId = TM->Submit (this);
	return val;
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
	cout << "Called from ClassId " << ClassId() << endl;
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
void OOPTask::Write (TPZStream & buf, int withclassid)
{
	TPZSaveable::Write (buf,withclassid);
	// ObjectId packing and unpacking
	fTaskId.Write (buf);
	buf.Write (&fProc);	// Processor where the task should be
	// executed
	buf.Write (&fPriority);
	buf.Write (&fIsRecurrent);
	fDataDepend.Write (buf);
	
}
void OOPTask::Read (TPZStream & buf, void * context)
{
	TPZSaveable::Read(buf, context);
	fTaskId.Read (buf);
	// Finished OOPObjectId unpacking
	buf.Read (&fProc);
	buf.Read (&fPriority);
	buf.Read (&fIsRecurrent);
	fDataDepend.Read(buf);
	
}

	/**
	 * Return the pointer to the ith object from which this task depends
	 */
TPZSaveable *OOPTask::GetDepObjPtr(int idepend)
{
	int numdep = fDataDepend.NElements();
	if(idepend < 0 || idepend >= numdep) return 0;
	OOPMDataDepend &dep = fDataDepend.Dep(idepend);
	return dep.ObjPtr()->Ptr();
}
	/**
	 * Increment the version of the ith object from which this task depends
	 */
void OOPTask::IncrementDepObjVersion(int idepend)
{
	int numdep = fDataDepend.NElements();
	if(idepend < 0 || idepend >= numdep) {
		cout << "Dpendency Id is bigger then number of objects\n";
		return;
	}
	OOPMDataDepend &dep = fDataDepend.Dep(idepend);
	dep.ObjPtr()->IncrementVersion(Id());
}
void OOPTask::IncrementWriteDependentData()
{
	int numdep = fDataDepend.NElements();
	int i;
	
	for(i=0;i<numdep;i++){
		if(fDataDepend.Dep(i).State()==EWriteAccess){
			fDataDepend.Dep(i).ObjPtr()->IncrementVersion(Id());
			cout << "Automatically Incrementing Write Dependent Data Versions\n";
			cout.flush();
		}
	}
}
