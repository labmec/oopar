#include "ooptaskmanager.h"
#include "oopcommmanager.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif
#include "oopdatamanager.h"
#include "ooptaskcontrol.h"
class   OOPTask;
class   OOPDataVersion;
class   OOPSaveable;
class   OOPObjectId;
class	OOPTerminationTask;
//#include <sys/types.h>
//#include <unistd.h>

int OOPTaskManager::main ()
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
void OOPTaskManager::TransferExecutingTasks(){
		
	list < OOPTaskControl * >::iterator sub;
	int listsize = fExecuting.size();
	sub = fExecuting.begin();
	OOPTaskControl * auxtc=0;
	if(listsize){
		auxtc = (*sub);
	}
	while (auxtc){
		bool isexec=false;
		pthread_mutex_lock(&fExecutingMutex);
		isexec = auxtc->Task ()->IsExecuting();
		pthread_mutex_unlock(&fExecutingMutex);
		if (!isexec){
			OOPObjectId id;
			id = auxtc->Task ()->Id ();
			auxtc->Depend ().SetExecuting (id, false);
			auxtc->Depend ().ReleaseAccessRequests (id);
			fFinished.push_back(auxtc);
			fExecuting.erase(sub);
		}
		listsize = fExecuting.size();
		sub = fExecuting.begin ();
		auxtc = 0;
		if(listsize) {
			auxtc = (*sub);
		}
		
		
	}

}

#ifdef MPI
#define MT


void * OOPTaskManager::TriggerTask(void * data){
	OOPTaskControl * tc = static_cast<OOPTaskControl *> (data);
	OOPTaskManager * lTM = dynamic_cast<OOPTaskManager *> (TM);
	tc->Task ()->Execute ();
	pthread_mutex_lock(&lTM->fExecutingMutex);
	tc->Task()->SetExecuting(false);
	pthread_mutex_unlock(&lTM->fExecutingMutex);
	
}
/**
	disparar o thread de execução da tarefa.
*/
void * OOPTaskManager::ExecuteMT(void * data){
	OOPTaskManager * lTM = static_cast<OOPTaskManager *>(data);
	//Qual é o service thread ?
	// O service thread e a linha de execucao do programa principal
	DM->SubmitAllObjects();
	CM->ReceiveMessages ();
	lTM->TransferSubmittedTasks ();
	list < OOPTaskControl * >::iterator i;
	// TaskManLog << "TTaskManager.Execute Queued task ids proc = " << fProc << 
	// "\n";
        TaskManLog << "Entering task list loop" << endl;
	//PrintTaskQueues("Antes", TaskQueueLog);
	lTM->fKeepGoing=true;
	lTM->ExecuteDaemons();
	while (lTM->fKeepGoing) {
		//pthread_mutex_lock(&fExecuteMutex);
		DM->SubmitAllObjects();
		CM->ReceiveMessages();
		lTM->ExecuteDaemons();
		while (lTM->fExecutable.size () && lTM->fExecuting.size() < 5) {
			i = lTM->fExecutable.begin ();
			OOPTaskControl *tc = (*i);
			lTM->fExecutable.erase(i);
			lTM->fExecuting.push_back(tc);
			tc->Task()->SetExecuting(true);
                        TaskManLog << "Entering taskcontrol execute for task " << tc->Task()->Id() << endl;
                        TaskManLog.flush();
                        tc->Execute();
			lTM->TransferExecutingTasks();
			DM->SubmitAllObjects();
		}
		DM->SubmitAllObjects();
		lTM->TransferExecutingTasks();
		DM->SubmitAllObjects();
		lTM->TransferFinishedTasks ();
		CM->ReceiveMessages ();
		lTM->TransferSubmittedTasks ();
		CM->SendMessages ();
		lTM->ExecuteDaemons();
		//wait
//		pthread_mutex_lock(&fExecuteMutex);
		if(!lTM->HasWorkTodo () && lTM->fKeepGoing){
//			cout << "Going into Blocking receive on TM->Execute() ";
//			cout << "PID" << getpid() << endl;
			cout.flush();
			#ifdef MPI
			OOPMPICommManager *MPICM = dynamic_cast<OOPMPICommManager *> (CM);
			if(MPICM) MPICM->ReceiveBlocking();
			#endif
//			pthread_cond_wait(&fExecuteCondition, &fExecuteMutex);
//			cout << "Leaving blocking receive PID " << getpid() << endl;
//			cout.flush();
			DM->SubmitAllObjects();
		}
//		pthread_mutex_unlock(&fExecuteMutex)
	}
	//PrintTaskQueues("Depois", TaskQueueLog);
	CM->SendMessages ();
	

}
#endif
OOPTaskManager::OOPTaskManager (int proc)
{
	fProc = proc;
	fLastCreated = 0;//NUMOBJECTS * fProc;
	fMaxId = fLastCreated + NUMOBJECTS;
	pthread_cond_init(&fExecuteCondition, NULL);
	pthread_cond_init(&fExecuteTaskCondition, NULL);
	pthread_mutex_init(&fExecutingMutex, NULL);
	pthread_mutex_init(&fFinishedMutex, NULL);
	pthread_mutex_init(&fSubmittedMutex, NULL);
}
OOPTaskManager::~OOPTaskManager ()
{
	list < OOPTask * >::iterator i;
	for (i = fSubmittedList.begin (); i != fSubmittedList.end (); i++)
		delete *i;
	list < OOPTaskControl * >::iterator itc;
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
	list < OOPTaskControl * >::iterator i;
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
	list < OOPTaskControl * >::iterator i;
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
void OOPTaskManager::SubmitDaemon (OOPDaemonTask * task) {
	fDaemon.push_back(task);
}
OOPObjectId OOPTaskManager::Submit (OOPTask * task)
{
	TaskManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	TaskManLog << "Calling Submit on OOPTaskManager ";
        TaskManLog.flush();
	OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(task);
	if(dmt) {
		// lock
		SubmitDaemon(dmt);
		TaskManLog << "Task Submitted is a daemon\n";
		return OOPObjectId();
	}
	OOPObjectId id = task->Id();
	if(id.IsZero()) id = GenerateId ();
	task->SetTaskId (id);
	TaskManLog << "Task with id " << id << " submitted " << endl;
	TaskManLog.flush();
	pthread_mutex_lock(&fSubmittedMutex);
	fSubmittedList.push_back (task);
	pthread_mutex_unlock(&fSubmittedMutex);
	return id;
}
int OOPTaskManager::NumberOfTasks ()
{
	pthread_mutex_lock(&fSubmittedMutex);
	pthread_mutex_lock(&fFinishedMutex);
	int numtasks = fExecutable.size () + fFinished.size () +
		fSubmittedList.size () + fTaskList.size () + fDaemon.size();
	pthread_mutex_unlock(&fFinishedMutex);
	pthread_mutex_unlock(&fSubmittedMutex);
	return numtasks;
}
bool OOPTaskManager::HasWorkTodo ()
{
	pthread_mutex_lock(&fSubmittedMutex);
	pthread_mutex_lock(&fFinishedMutex);
	int numtasks = fExecutable.size () + fFinished.size () +
		fSubmittedList.size () + fDaemon.size();
	pthread_mutex_unlock(&fSubmittedMutex);
	pthread_mutex_unlock(&fFinishedMutex);
	return numtasks != 0;
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
	list < OOPTaskControl * >::iterator i;	// , iprev, atual;
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
void OOPTaskManager::ExecuteDaemons() {
	list < OOPDaemonTask * >::iterator i;
	while(fDaemon.size()) {
		i = fDaemon.begin();
		if((*i)->GetProcID() != DM->GetProcID()) {
			CM->SendTask((*i));
		} else {
			(*i)->Execute();
			delete (*i);
		}
		fDaemon.erase(i);
	}
}
void OOPTaskManager::Execute ()
{
#ifndef MT	
	//Qual é o service thread ?
	// O service thread e a linha de execucao do programa principal
	DM->SubmitAllObjects();
	CM->ReceiveMessages ();
	TransferSubmittedTasks ();
	list < OOPTaskControl * >::iterator i;
	// TaskManLog << "TTaskManager.Execute Queued task ids proc = " << fProc << 
	// "\n";
	// TaskManLog << "Entering task list loop" << endl;
	//PrintTaskQueues("Antes", TaskQueueLog);
	fKeepGoing=true;
	ExecuteDaemons();
	while (fKeepGoing) {
		//pthread_mutex_lock(&fExecuteMutex);
		DM->SubmitAllObjects();
		CM->ReceiveMessages();
		ExecuteDaemons();
		while (fExecutable.size ()) {
			//pthread_mutex_unlock(&fExecuteMutex);
			//DM->PrintDataQueues("Dentro do Loop ----------------",DataQueueLog);
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
			DM->SubmitAllObjects();

#ifdef DEBUG
			// TaskManLog << "Executing task on processor " << fProc << 
			// endl;
			// id.Print(TaskManLog);
			// TaskManLog.flush();
#endif
			fFinished.push_back (tc);
			fExecutable.erase (i);
		}
		DM->SubmitAllObjects();
		TransferFinishedTasks ();
		CM->ReceiveMessages ();
		TransferSubmittedTasks ();
		CM->SendMessages ();
		ExecuteDaemons();
		//wait
//		pthread_mutex_lock(&fExecuteMutex);
		if(!HasWorkTodo () && fKeepGoing){
//			cout << "Going into Blocking receive on TM->Execute()\n";
//			cout << "PID" << getpid() << endl;
			cout.flush();
			#ifdef MPI
			OOPMPICommManager *MPICM = dynamic_cast<OOPMPICommManager *> (CM);
			if(MPICM) MPICM->ReceiveBlocking();
			#endif
//			pthread_cond_wait(&fExecuteCondition, &fExecuteMutex);
			cout << "Leaving blocking receive PID " << getpid() << endl;
			cout.flush();
			DM->SubmitAllObjects();
		}
//		pthread_mutex_unlock(&fExecuteMutex)
	}
	//PrintTaskQueues("Depois", TaskQueueLog);
	CM->SendMessages ();
#else
//	pthread_t execute_thread;
	cout << "Creating service thread\n";
	cout.flush();
	if(pthread_create(&fexecute_thread, NULL, ExecuteMT, this)){
		cerr << "Fail to create service thread\n";
		cerr << "Going out\n";
		cerr.flush();
	}
	cout << "Created succesfuly\n";
	cout.flush();
		
//	pthread_join(execute_thread,NULL);
#endif
}
void OOPTaskManager::SetKeepGoing(bool go){
	fKeepGoing = go;
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
	list < OOPTaskControl * >::iterator i;
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
	list < OOPTaskControl * >::iterator i;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		(*i)->Task ()->Print (out);
	}
}
void OOPTaskManager::TransferSubmittedTasks ()
{
	list < OOPTask * >::iterator sub;
	pthread_mutex_lock(&fSubmittedMutex);
	int listsize = fSubmittedList.size();
	sub = fSubmittedList.begin ();
	OOPTask * aux = 0;
	if(listsize) {
		aux = (*sub);
		fSubmittedList.erase (sub);
	}
	pthread_mutex_unlock(&fSubmittedMutex);
	
	while (aux){//(fSubmittedList.begin () != fSubmittedList.end ()) {
		//sub = fSubmittedList.begin ();
		//OOPTask * aux = (*sub);
		if (aux->GetProcID () != fProc) {
			CM->SendTask (aux);
		}
		else {
			OOPTaskControl *tc = new OOPTaskControl (*sub);
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
		pthread_mutex_lock(&fSubmittedMutex);
		listsize = fSubmittedList.size();
		sub = fSubmittedList.begin ();
		aux = 0;
		if(listsize) {
			aux = (*sub);
			fSubmittedList.erase (sub);
		}
		pthread_mutex_unlock(&fSubmittedMutex);
		
	}
}
void OOPTaskManager::TransferFinishedTasks ()
{
	list < OOPTaskControl * >::iterator sub;
	pthread_mutex_lock(&fFinishedMutex);
	int listsize = fFinished.size();
	sub = fFinished.begin();
	OOPTaskControl * auxtc=0;
	if(listsize){
		auxtc = (*sub);
		fFinished.erase(sub);
	}
	pthread_mutex_unlock(&fFinishedMutex);
	while (auxtc){
		if (auxtc->Task ()->GetProcID () != fProc) {
			CM->SendTask (auxtc->Task ());
			auxtc->ZeroTask ();
			delete auxtc;
		}else if(auxtc->Task ()->IsRecurrent ()) {
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
		}
		else {
			delete auxtc;
		}
		pthread_mutex_lock(&fFinishedMutex);
		listsize = fFinished.size();
		auxtc=0;
		sub = fFinished.begin ();
		if(listsize) {
			auxtc = (*sub);
			fFinished.erase (sub);
		}
		pthread_mutex_unlock(&fFinishedMutex);
		
	}
}
void OOPTaskManager::TransfertoExecutable (const OOPObjectId & taskid)
{
	list < OOPTaskControl * >::iterator i;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == taskid) {
			tc->Task ()->SetDependencyList (tc->Depend ());
			tc->Depend ().SetExecuting (taskid, true);
			OOPDaemonTask *dmt =
				dynamic_cast < OOPDaemonTask * >(tc->Task ());
			if (dmt) {
				cout << "TM::TransfertoExecutable inconsistent datastructure\n"
					"There is daemontask in the fTaskList\n";
				SubmitDaemon(dmt);
				tc->ZeroTask();
				delete tc;
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
	list < OOPTask * >::iterator i;
	for(i=fSubmittedList.begin();i!=fSubmittedList.end();i++)
		out << (*i)->Id() << endl;
	out << "Print fTaskList\n";
	out << "Number of tasks :" << fTaskList.size() << endl;
	list < OOPTaskControl * >::iterator j;
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
OOPTerminationTask::~OOPTerminationTask (){}
OOPTerminationTask::OOPTerminationTask (int ProcId) : OOPTask(ProcId){}
OOPTerminationTask::OOPTerminationTask (const OOPTerminationTask & term): OOPTask(term)
{
}

OOPMReturnType OOPTerminationTask::Execute (){
	TM->SetKeepGoing(false);
	cout << "----------------------------------------------TM Finished\n";
	cout.flush();
	return ESuccess;
}
long OOPTerminationTask::GetClassID ()
{
	return TTERMINATIONTASK_ID;
}

int OOPTerminationTask::Pack(OOPSendStorage * buf){
	OOPTask::Pack(buf);
	return 0;
}
int OOPTerminationTask::Unpack(OOPReceiveStorage * buf){
	OOPTask::Unpack(buf);
	return 0;
}

long int OOPTerminationTask::ExecTime(){
	return -1;
}
OOPSaveable *OOPTerminationTask::Restore (OOPReceiveStorage * buf){
	OOPTerminationTask*v = new OOPTerminationTask(0);
	v->Unpack (buf);
	return v;
}
extern OOPTaskManager *TM;
