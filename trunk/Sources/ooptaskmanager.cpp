
#include "ooptaskmanager.h"
#include "oopcommmanager.h"
#include "oopdatamanager.h"
#include "ooptaskcontrol.h"
//#include "tmultidata.h"
//#include "tmultitask.h"

class   OOPTask;
class   OOPDataVersion;

class   OOPSaveable;


//class TMultiTask;
//class TMultiData;
class   OOPObjectId;

void OOPTaskManager::main ()
{

	/* 
	 * OOPObjectId Rhs_id[4], State_id[4], Mesh_id[4], task_id;
	 * TMultiTask * task_0 = new TMultiTask(0); TMultiTask * task_1 = new 
	 * TMultiTask(0); TMultiTask * task_2 = new TMultiTask(0); TMultiTask 
	 * * task_3 = new TMultiTask(0);
	 * 
	 * TMultiData * Rhs = new TMultiData[4]; TMultiData * State = new
	 * TMultiData[4]; TMultiData * Mesh = new TMultiData[4];
	 * 
	 * int i=0; int ntask = 4;
	 * 
	 * for(i=0;i<ntask;i++){ Rhs_id[i] = DM->SubmitObject(&Rhs[i], 0);
	 * State_id[i] = DM->SubmitObject(&State[i], 0); Mesh_id[i] =
	 * DM->SubmitObject(&Mesh[i], 0); }
	 * 
	 * 
	 * 
	 * OOPMDataState st = EWriteAccess; OOPDataVersion ver;
	 * ver.SetLevelVersion(0,-1);
	 * 
	 * task_0->RhsId = Rhs_id[0];
	 * 
	 * task_1->RhsId = Rhs_id[1]; task_2->RhsId = Rhs_id[2];
	 * task_3->RhsId = Rhs_id[3];
	 * 
	 * task_0->AddDependentData(Rhs_id[0], st, ver);
	 * task_1->AddDependentData(Rhs_id[1], st, ver);
	 * task_2->AddDependentData(Rhs_id[2], st, ver);
	 * task_3->AddDependentData(Rhs_id[3], st, ver);
	 * 
	 * st = EReadAccess; ver.SetLevelVersion(0,-1);
	 * 
	 * task_0->AddDependentData(State_id[0], st, ver);
	 * task_1->AddDependentData(State_id[1], st, ver);
	 * task_2->AddDependentData(State_id[2], st, ver);
	 * task_3->AddDependentData(State_id[3], st, ver);
	 * 
	 * task_0->AddDependentData(Mesh_id[0], st, ver);
	 * task_1->AddDependentData(Mesh_id[1], st, ver);
	 * task_2->AddDependentData(Mesh_id[2], st, ver);
	 * task_3->AddDependentData(Mesh_id[3], st, ver);
	 * 
	 * task_0->Submit(); task_1->Submit(); task_2->Submit();
	 * task_3->Submit(); TM->Print(TaskManLog);
	 * 
	 * 
	 * 
	 * // OOPMDataState st = EWriteAccess; // OOPDataVersion ver;
	 * 
	 * //ver.SetLevelVersion(0,1); // ver.Print(TaskManLog); //
	 * task->AddDependentData(data_id, st, ver); //
	 * ver.SetLevelVersion(0,1); // ver.Print(TaskManLog); //
	 * task_2->AddDependentData(data_id, st, ver);
	 * 
	 * // task->Submit(); // task_2->Submit();
	 * 
	 * //DM->CheckAccessRequests(); // task->Print(TaskManLog); //
	 * task_2->Print(TaskManLog);
	 * 
	 * TM->Execute(); */
}

OOPTaskManager::OOPTaskManager (int proc)
{
	fProc = proc;
	fLastCreated = 0;//NUMOBJECTS * fProc;
	fMaxId = fLastCreated + NUMOBJECTS;
}

OOPTaskManager::~OOPTaskManager ()
{
	deque < OOPTask * >::iterator i;
	for (i = fSubmittedList.begin (); i != fSubmittedList.end (); i++)
		delete *i;
	deque < OOPTaskControl * >::iterator itc;
	for (itc = fExecutable.begin (); itc != fExecutable.end (); itc++)
		delete *itc;
	for (itc = fFinished.begin (); itc != fFinished.end (); itc++)
		delete *itc;
	for (itc = fTaskList.begin (); itc != fTaskList.end (); itc++)
		delete *itc;
}

//Passar o ID do task e do dado.
void OOPTaskManager::NotifyAccessGranted (const OOPObjectId & TaskId,
					  const OOPMDataDepend & depend,
					  OOPMetaData * objptr)
{
	TaskManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	deque < OOPTaskControl * >::iterator i;
	bool found = false;

	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == TaskId) {
			found = true;
			tc->Depend ().GrantAccess (depend, objptr);
#ifdef DEBUG
			TaskManLog << "Access Granted to taskId " << TaskId ;
//			TaskId.Print (TaskManLog);
			TaskManLog << " on data " << depend.Id() << endl;
//			depend.Id ().Print (TaskManLog);
#endif
			if (tc->Depend ().CanExecute ()) {
				TransfertoExecutable (tc->Task ()->Id ());
				TaskManLog << "OOPTaskManager task is executable " << TaskId << endl;
//				TaskId.Print (TaskManLog);

			}
			break;
		}
	}

	if (!found) {
		cerr << "Task not found on current TM: File:" << __FILE__ <<
			" Line:" << __LINE__ << endl;
		cerr << "Task \n";
		TaskId.Print (cerr);
	}
}


void OOPTaskManager::RevokeAccess (const OOPObjectId & TaskId,
				   const OOPMDataDepend & depend)
{
	TaskManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	deque < OOPTaskControl * >::iterator i;
	bool found = false;

	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == TaskId) {
			found = true;
			tc->Depend ().RevokeAccess (depend);
#ifdef DEBUG
			TaskManLog << "Access Revoked to taskId " << TaskId;
//			TaskId.Print (TaskManLog);
			TaskManLog << " on data " << depend.Id() << endl;
//			depend.Id ().Print (TaskManLog);
#endif
			break;
		}
	}

	if (!found) {
		cerr << "OOPTaskManager::RevokeAccess Task not found on current TM: File:" << __FILE__ << " Line:" << __LINE__ << endl;
		cerr << "Task \n";
		TaskId.Print (cerr);
	}
}


OOPObjectId OOPTaskManager::Submit (OOPTask * task)
{
	TaskManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	TaskManLog << "Calling Submit on OOPTaskManager ";
	OOPObjectId id;
	// mutex lock 
	id = GenerateId ();
	task->SetTaskId (id);
	TaskManLog << id << endl;
	TaskManLog.flush();
//	id.ShortPrint(TaskManLog);
	fSubmittedList.push_back (task);
	// mutex unlock
	return id;
}

OOPObjectId OOPTaskManager::ReSubmit (OOPTask * task)
{
	// mutex lock 
	OOPObjectId auxId = task->Id ();
	int intId = auxId.GetId ();
	if (intId == -1) {
		cerr << "ReSubmite called on task with no Id\n";
		cerr << "File " << __FILE__ << endl;
		exit (-1);
	}
	fSubmittedList.push_back (task);
	// mutex unlock
	return task->Id ();
}


int OOPTaskManager::NumberOfTasks ()
{
	return fExecutable.size () + fFinished.size () +
		fSubmittedList.size () + fTaskList.size ();
}


int OOPTaskManager::ChangePriority (OOPObjectId & taskid, int newpriority)
{
	// if(!ExistsTask(taskid)) return 0;
	// int proc = fTaskExist[taskid];
	// int proc = taskid.GetProcId();
	OOPTask *t = FindTask (taskid);
	if (t) {
		t->ChangePriority (newpriority);
		Reschedule ();
	}
	return 0;
}

int OOPTaskManager::CancelTask (OOPObjectId taskid)
{
	TaskManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	deque < OOPTaskControl * >::iterator i;	// , iprev, atual;

	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = *i;
		if (tc->Task ()->Id () == taskid) {
			TaskManLog << "Task erased ";
			TaskManLog << "Task ID " << tc->Task()->Id() << endl;
//			tc->Task ()->Id ().Print (TaskManLog);
			tc->Depend ().ReleaseAccessRequests (tc->Task ()->
							     Id ());
			delete tc;
			fTaskList.erase (i);
			return 1;
		}
	}
	return 0;
}

void OOPTaskManager::CleanUpTasks ()
{
#ifndef WIN32
#warning "CleanUpTasks is empty"
#endif
}
void OOPTaskManager::Execute ()
{
	CM->ReceiveMessages ();
	TransferSubmittedTasks ();
	deque < OOPTaskControl * >::iterator i;
	// TaskManLog << "TTaskManager.Execute Queued task ids proc = " << fProc << 
	// "\n";
	// TaskManLog << "Entering task list loop" << endl;
	PrintTaskQueues("Antes", TaskQueueLog);
	while (fExecutable.size ()) {
		while (fExecutable.size ()) {
			DM->PrintDataQueues("Dentro do Loop ----------------------------------------",DataQueueLog);
			i = fExecutable.begin ();
			OOPTaskControl *tc = (*i);
			tc->Task ()->Execute ();
//              TaskManLog << (*i)->Task() << ":";
			OOPObjectId id;
			id = tc->Task ()->Id ();
			tc->Depend ().SetExecuting (tc->Task ()->Id (),
						      false);
			tc->Depend ().ReleaseAccessRequests (tc->Task ()->
							       Id ());
#ifdef DEBUG
			// TaskManLog << "Executing task on processor " << fProc << 
			// endl;
			// id.Print(TaskManLog);
			// TaskManLog.flush();
#endif
			fFinished.push_back (tc);
			fExecutable.erase (i);
		}
		TransferFinishedTasks ();
		CM->ReceiveMessages ();
		TransferSubmittedTasks ();
		CM->SendMessages ();
	}
	PrintTaskQueues("Depois", TaskQueueLog);
	CM->SendMessages ();


}

OOPObjectId OOPTaskManager::GenerateId ()
{      // Generate a unique id number
	fLastCreated++;
	if (fLastCreated >= fMaxId)
		exit (-1);	// the program ceases to function
	OOPObjectId tmp (fProc, fLastCreated);
	return tmp;
}


OOPTask *OOPTaskManager::FindTask (OOPObjectId taskid)
{      // find the task with the given id
	deque < OOPTaskControl * >::iterator i;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTask *t = (*i)->Task ();
		if (t->Id () == taskid)
			return t;
	}
	return 0;
}

/**
 * reorder the tasks according to their priority
 * No local implementation found
 */
void OOPTaskManager::Reschedule ()
{
}

void OOPTaskManager::Print (ostream & out)
{
	out << "Task Manager data structure \t" << endl;
	out << "TM Processor " << fProc << endl;
	out << "Id of Last Created Task \t" << fLastCreated << endl;
	out << "Maximum number of tasks available \t" << fMaxId << endl;
	out << "Queued Daemon tasks ---------\t" << endl;
	out << "Queued Time Consuming tasks ---------" << endl;
	out << "Number of Time Consuming tasks \t" << fTaskList.
		size () << endl;
	deque < OOPTaskControl * >::iterator i;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		(*i)->Task ()->Print (out);
	}
}

void OOPTaskManager::TransferSubmittedTasks ()
{
	deque < OOPTask * >::iterator sub;
	while (fSubmittedList.begin () != fSubmittedList.end ()) {
		sub = fSubmittedList.begin ();
		OOPTask * aux = (*sub);
		if (aux->GetProcID () != fProc) {
			CM->SendTask (aux);
			fSubmittedList.erase (sub);
		}
		else {
			OOPTaskControl *tc = new OOPTaskControl (*sub);
			fSubmittedList.erase (sub);
			fTaskList.push_back (tc);
			if (tc->Depend ().
			    SubmitDependencyList (tc->Task ()->Id ())) {
				// their is no incompatibility between
				// versions
			}
			else {
				// there is an incompatibility of versions
				CancelTask (tc->Task ()->Id ());
			}
		}
	}
}

void OOPTaskManager::TransferFinishedTasks ()
{
	deque < OOPTaskControl * >::iterator sub;
	while (fFinished.size ()) {
		sub = fFinished.begin ();
		OOPTaskControl * auxtc=(*sub);
		if (auxtc->Task ()->GetProcID () != fProc) {
			CM->SendTask (auxtc->Task ());
			auxtc->ZeroTask ();
			delete auxtc;
			fFinished.erase (sub);
		}
		else if (auxtc->Task ()->IsRecurrent ()) {
			auxtc->Depend () =
				auxtc->Task ()->GetDependencyList ();
			auxtc->Depend ().ClearPointers ();
			fTaskList.push_back (auxtc);

			if (auxtc->Depend ().
			    SubmitDependencyList (auxtc->Task ()->Id ())) {
				// their is no incompatibility between
				// versions
			}
			else {
				// there is an incompatibility of versions
				CancelTask (auxtc->Task ()->Id ());
			}
			fFinished.erase (sub);
		}
		else {
			delete auxtc;
			fFinished.erase (sub);
		}
	}
}

void OOPTaskManager::TransfertoExecutable (const OOPObjectId & taskid)
{

	deque < OOPTaskControl * >::iterator i;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == taskid) {
			tc->Task ()->SetDependencyList (tc->Depend ());
			tc->Depend ().SetExecuting (taskid, true);
			OOPDaemonTask *dmt =
				dynamic_cast < OOPDaemonTask * >(tc->Task ());
			if (dmt) {
				fExecutable.push_front (tc);
			}
			else {
				fExecutable.push_back (tc);
			}
			fTaskList.erase (i);
			break;
		}
	}
}
void OOPTaskManager::PrintTaskQueues(char * msg, ostream & out){
	out << msg << endl;
	out << "Printing TaskManager Queues on TM:" << fProc << endl;
	out << "Print fSubmittedList\n";
	out << "Number of tasks :" << fSubmittedList.size() << endl;
	deque < OOPTask * >::iterator i;
	for(i=fSubmittedList.begin();i!=fSubmittedList.end();i++)
		out << (*i)->Id() << endl;
	out << "Print fTaskList\n";
	out << "Number of tasks :" << fTaskList.size() << endl;
	deque < OOPTaskControl * >::iterator j;
	for(j=fTaskList.begin();j!=fTaskList.end();j++)
		out << (*j)->Task()->Id() << endl;
	out << "Print fExecutable\n";
	out << "Number of tasks :" << fExecutable.size() << endl;
	for(j=fExecutable.begin();j!=fExecutable.end();j++)
		out << (*j)->Task()->Id() << endl;
	out << "Print fFinished\n";
	out << "Number of tasks :" << fFinished.size() << endl;
	for(j=fFinished.begin();j!=fFinished.end();j++)
		out << (*j)->Task()->Id() << endl;

	
}
