
#include "ooptaskmanager.h"
#include "oopcommmanager.h"
#include "tmultidata.h"
#include "tmultitask.h"

void OOPTaskManager::ComputeFlux(){
        // message #1.1.1 to pc:TParCompute
        TParCompute * pc = new TParCompute(Procid);

        // message #1.1.2 to rhs:TParRhs
        TParRhs * rhs = new TParRhs(ProcId, Trace);

        // message #1.1.3 to state:TParState
        TParState * state = new TParState(ProcId, Trace);

        // message #1.1.4 to mesh:TParMesh
        TParMesh * mesh = new TParMesh(ProcId, Trace);

        // message #1.1.5 to ver:OOPDataVersion
        OOPDataVersion * ver = new OOPDataVersion();

        // message #1.1.6 to ver:OOPDataVersion
        ver->SetLevelVersion(level, version);

        // message #1.1.7 to DM:OOPDataManager
        OOPDataManager * DM = null;
        OOPObjectId rhsId = DM->SubmitObject(rhs, 1);

        // message #1.1.8 to pc:TParCompute
        pc->SetRhsId(rhsId);

        // message #1.1.9 to DM:OOPDataManager
        OOPObjectId stateId = DM->SubmitObject(state);

        // message #1.1.10 to pc:TParCompute
        pc->SetStateId(stateId);

        // message #1.1.11 to DM:OOPDataManager
        OOPObjectId meshId = DM->SubmitObject(mesh);

        // message #1.1.12 to pc:TParCompute
        pc->SetMeshId(meshId);

        // message #1.1.13 to pc:TParCompute
        pc->AddDependentData(meshId, st, version);

        // message #1.1.14 to pc:TParCompute
        pc->AddDependentData(stateId, st, version);

        // message #1.1.15 to pc:TParCompute
        pc->AddDependentData(rhsId, st, version);

        // message #1.1.16 to pc:TParCompute
        OOPObjectId oOPObjectId = pc->Submit();

        // message #1.1.1 to pc:TParCompute
        TParCompute * pc = new TParCompute(Procid);

        // message #1.1.2 to rhs:TParRhs
        TParRhs * rhs = new TParRhs(ProcId, Trace);

        // message #1.1.3 to state:TParState
        TParState * state = new TParState(ProcId, Trace);

        // message #1.1.4 to mesh:TParMesh
        TParMesh * mesh = new TParMesh(ProcId, Trace);

        // message #1.1.5 to ver:OOPDataVersion
        OOPDataVersion * ver = new OOPDataVersion();

        // message #1.1.6 to ver:OOPDataVersion
        ver->SetLevelVersion(level, version);

        // message #1.1.7 to DM:OOPDataManager
        OOPDataManager * DM = null;
        OOPObjectId rhsId = DM->SubmitObject(rhs, 1);

        // message #1.1.8 to pc:TParCompute
        pc->SetRhsId(rhsId);

        // message #1.1.9 to DM:OOPDataManager
        OOPObjectId stateId = DM->SubmitObject(state);

        // message #1.1.10 to pc:TParCompute
        pc->SetStateId(stateId);

        // message #1.1.11 to DM:OOPDataManager
        OOPObjectId meshId = DM->SubmitObject(mesh);

        // message #1.1.12 to pc:TParCompute
        pc->SetMeshId(meshId);

        // message #1.1.13 to pc:TParCompute
        pc->AddDependentData(meshId, st, version);

        // message #1.1.14 to pc:TParCompute
        pc->AddDependentData(stateId, st, version);

        // message #1.1.15 to pc:TParCompute
        pc->AddDependentData(rhsId, st, version);

        // message #1.1.16 to pc:TParCompute
        OOPObjectId oOPObjectId = pc->Submit();

        // message #1.1.1 to pc:TParCompute
        TParCompute * pc = new TParCompute(Procid);

        // message #1.1.2 to rhs:TParRhs
        TParRhs * rhs = new TParRhs(ProcId, Trace);

        // message #1.1.3 to state:TParState
        TParState * state = new TParState(ProcId, Trace);

        // message #1.1.4 to mesh:TParMesh
        TParMesh * mesh = new TParMesh(ProcId, Trace);

        // message #1.1.5 to ver:OOPDataVersion
        OOPDataVersion * ver = new OOPDataVersion();

        // message #1.1.6 to ver:OOPDataVersion
        ver->SetLevelVersion(level, version);

        // message #1.1.7 to DM:OOPDataManager
        OOPDataManager * DM = null;
        OOPObjectId rhsId = DM->SubmitObject(rhs, 1);

        // message #1.1.8 to pc:TParCompute
        pc->SetRhsId(rhsId);

        // message #1.1.9 to DM:OOPDataManager
        OOPObjectId stateId = DM->SubmitObject(state);

        // message #1.1.10 to pc:TParCompute
        pc->SetStateId(stateId);

        // message #1.1.11 to DM:OOPDataManager
        OOPObjectId meshId = DM->SubmitObject(mesh);

        // message #1.1.12 to pc:TParCompute
        pc->SetMeshId(meshId);

        // message #1.1.13 to pc:TParCompute
        pc->AddDependentData(meshId, st, version);

        // message #1.1.14 to pc:TParCompute
        pc->AddDependentData(stateId, st, version);

        // message #1.1.15 to pc:TParCompute
        pc->AddDependentData(rhsId, st, version);

        // message #1.1.16 to pc:TParCompute
        OOPObjectId oOPObjectId = pc->Submit();
    }
class OOPTask;
class OOPDataVersion;

class OOPSaveable;


class TMultiTask;
class TMultiData;
class OOPObjectId;

void OOPTaskManager::main(){
	
	OOPObjectId Rhs_id[4], State_id[4], Mesh_id[4], task_id;
	TMultiTask * task_0 = new TMultiTask(0);
	TMultiTask * task_1 = new TMultiTask(0);
	TMultiTask * task_2 = new TMultiTask(0);
	TMultiTask * task_3 = new TMultiTask(0);
	
	TMultiData * Rhs = new TMultiData[4];
	TMultiData * State = new TMultiData[4];
	TMultiData * Mesh = new TMultiData[4];
	
	int i=0;
	int ntask = 4;
	
	for(i=0;i<ntask;i++){
		Rhs_id[i] = DM->SubmitObject(&Rhs[i], 0);
		State_id[i] = DM->SubmitObject(&State[i], 0);
		Mesh_id[i] = DM->SubmitObject(&Mesh[i], 0);
	}
	
	
	
	OOPMDataState st = EWriteAccess;
	OOPDataVersion ver;
	ver.SetLevelVersion(0,-1);

	task_0->RhsId = Rhs_id[0];
	
	task_1->RhsId = Rhs_id[1];
	task_2->RhsId = Rhs_id[2];
	task_3->RhsId = Rhs_id[3];

	task_0->AddDependentData(Rhs_id[0], st, ver);
	task_1->AddDependentData(Rhs_id[1], st, ver);
	task_2->AddDependentData(Rhs_id[2], st, ver);
	task_3->AddDependentData(Rhs_id[3], st, ver);
	
	st = EReadAccess;
	ver.SetLevelVersion(0,-1);
	
	task_0->AddDependentData(State_id[0], st, ver);
	task_1->AddDependentData(State_id[1], st, ver);
	task_2->AddDependentData(State_id[2], st, ver);
	task_3->AddDependentData(State_id[3], st, ver);

	task_0->AddDependentData(Mesh_id[0], st, ver);
	task_1->AddDependentData(Mesh_id[1], st, ver);
	task_2->AddDependentData(Mesh_id[2], st, ver);
	task_3->AddDependentData(Mesh_id[3], st, ver);
	
	task_0->Submit();
	task_1->Submit();
	task_2->Submit();
	task_3->Submit();
	TM->Print(cout);
	
	/*

	OOPMDataState st = EWriteAccess;
	OOPDataVersion ver;
	
	//ver.SetLevelVersion(0,1);
	ver.Print(cout);
	task->AddDependentData(data_id, st, ver);
	ver.SetLevelVersion(0,1);
	ver.Print(cout);
	task_2->AddDependentData(data_id, st, ver);
		
	task->Submit();
	task_2->Submit();
	
	//DM->CheckAccessRequests();
	task->Print(cout);
	task_2->Print(cout);
	*/
	TM->Execute();
}

OOPTaskManager::OOPTaskManager(int proc){
	fProc = proc;
	fLastCreated = NUMOBJECTS * fProc;
	fMaxId = fLastCreated + NUMOBJECTS;
}

OOPTaskManager::~OOPTaskManager() {
	deque<OOPTask *>::iterator i;
	for(i = fTimeConsuming.begin(); i!=fTimeConsuming.end(); i++)
		delete *i;
	for(i = fDaemon.begin(); i!=fDaemon.end(); i++) delete *i;
	fTimeConsuming.clear();
	fDaemon.clear();
}

//Passar o ID do task e do dado.
void OOPTaskManager::NotifyAccessGranted(OOPObjectId & TaskId, OOPObjectId & DataId, OOPMDataState st,
						OOPDataVersion &Version, OOPSaveable * objptr)
{
	deque<OOPTask*>::iterator i;
	bool found = false;
	for(i=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){
		OOPTask * t = (OOPTask*) (*i);
		if(t->Id() == TaskId){
			found = true;
			t->GrantAccess( DataId , st, objptr);
			#ifdef DEBUG
			cout << "Access Granted to taskId";
			TaskId.Print(cout);
			cout << " on data ";
			DataId.Print(cout);
			#endif
			//Notify someone else ?
		}
	}
	if (!found) {
		cerr << "Task not found on current TM: File:" << __FILE__ << " Line:" << __LINE__ << endl;
	}
}


OOPObjectId OOPTaskManager::Submit(OOPTask *task) {
	
	if(task->ExecTime() == 0) {
		if(task->GetProcID() == fProc) {
			OOPMReturnType r = task->Execute();
			if(r == EContinue) {
				fDaemon.push_back(task);//append(task);
			} else {
				delete task;
			}
		}
		//return 0;
	}
	
	//Check whether all the data dependency is already set,
	//if not, issue an error message !
	
	
	OOPObjectId id;
	id = GenerateId();
	task->SetTaskId(id);
	
	
	if(task->GetProcID() == fProc) {
		fTimeConsuming.push_back(task);
	}

	return id;
}


int OOPTaskManager::NumberOfTasks() {
	return fTimeConsuming.size();
}


int OOPTaskManager::ChangePriority(OOPObjectId taskid, int newpriority) {
	//if(!ExistsTask(taskid)) return 0;
	//int proc = fTaskExist[taskid];
	int proc = taskid.GetProcId();
	OOPTask *t = FindTask(taskid);
	if(t) {
		t->ChangePriority(newpriority);
		Reschedule();
	}
	return 0;
}

int OOPTaskManager::CancelTask(OOPObjectId taskid){
	//if(!ExistsTask(taskid)) return 0;
	//int proc = fTaskExist[taskid]; //How to get that information ?
	OOPTask * task;
	task = FindTask(taskid);
	int proc = task->GetProcID();
	deque<OOPTask * >::iterator i;//, iprev, atual;
	i = fTimeConsuming.begin();
//	iprev = 0, atual = 0;
	
	for(i=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){
		OOPTask * t =  *i;
		if (t->Id() == taskid){
			cout << "Task erased" << endl;
			delete *i;
			fTimeConsuming.erase(i);
			//returns 1 if task was canceled
			return 1;
		}
	}
			
	return 0;
}
	
void OOPTaskManager::NotifyCancel(OOPObjectId taskid){
	/*if(!ExistsTask(taskid)) return;
	int proc = fTaskExist[taskid];
	if(proc == fProc) return;*/
	OOPTask * task = FindTask(taskid);
	int proc = task->GetProcID();
	if(proc == fProc) return;
	/*deque<OOPTask *>::iterator i;	
	i = fTimeConsuming.begin();
	for(i;i!=fTimeConsuming.end();i++) {
		//OOPTask *t =  fTimeConsuming(i);
		
		fTimeConsuming.next(i);
		if(t->HasTaskDependence(taskid)) {
			CancelTask(t->Id());
			i = fTimeConsuming.first();
		}
	}*/
}


/*int TTaskManager::ExistsTask(OOPObjectId taskid){
	//return (fTaskExist.contains(taskid));
}*/
/*
int TTaskManager::TryTask(OOPObjectId TaskId){
//Phil
	//if(!ExistsTask(TaskId) || fTaskExist[TaskId] != fProc) return 0;
	deque<OOPTask *>::iterator i;
	OOPTask *t = 0;
	for(i=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){
		t =  (*i);
		if(!t){
			//O que fazer ?
		}
	}
	/*Pix iprev = 0,atual = 0,inext = 0;
	atual = fTimeConsuming.first();
	OOPTask *t = 0;
	while(atual) {
		t =  fTimeConsuming(atual);
		inext = atual;
		fTimeConsuming.next(inext);
		if(!t) {
			if(iprev) 	fTimeConsuming.del_after(iprev);
			else 		fTimeConsuming.remove_front();
			return 0;
		}
		if(t->Id() == TaskId) {
			return TryTask(iprev,atual);
		}

	// PERIGO, pode ser que inext nao pertence mais a fila!
		atual = inext;
	}
	return 0;
}
*/
/*int TTaskManager::TryTask(Pix iprev, Pix atual) {

	OOPTask *t =  fTimeConsuming(atual);
	if(!t) {
		if(iprev) 	fTimeConsuming.del_after(iprev);
		else 		fTimeConsuming.remove_front();
		return 0;
	}
	TTMMessageTask *cm = 0;
	if(t->CanExecute()) {
		OOPMReturnType res = t->Execute();
		cm = 0;
		if(res == ESuccess) {
			if(iprev)	fTimeConsuming.del_after(iprev);
			else		fTimeConsuming.remove_front();
			cm = new TTMMessageTask(-1);
			cm->fTaskId = t->Id();
			cm->fMessageOrigin = fProc;
			cm->fMessageDestination = -1;
			cm->fProcDestination = -1;
			cm->fProcOrigin = fProc;
			cm->fMessage = ETaskAborted;
			cm->fTask = 0;
			fTaskExist.del(t->Id());
			delete t;
		}
		switch(res) {
			case ESuccess :
				cm->fMessage = ETaskFinished;
				break;
			case EFail :
				CancelTask(t->Id());
				break;
			case EContinue :
				break;
		}
		if(cm) {
//			fMessageList.append(cm);
			CM->SendTask(cm);
			delete cm;
		}
		return 1;
	}
	return 0;
}
*/
/*
void TTaskManager::AddTaskDependence(long dependent, long execfirst){
	if(!ExistsTask(execfirst) || !ExistsTask(dependent)) return;
	int proc = fTaskExist[dependent];
	if(proc == fProc) {
		OOPTask *t = FindTask(dependent);
		if(!t) return;
		t->ExecAfter(execfirst);
		return;
	}
	TTMMessageTask *cm;
	cm = new TTMMessageTask(proc);
	cm->fTaskId = dependent;
	cm->fMessageOrigin = fProc;
	cm->fMessageDestination = proc;
	cm->fProcDestination = proc;
	cm->fProcOrigin = fProc;
	cm->fMessage = EAddDependence;
	cm->fTaskDependence = execfirst;
	cm->fTask = 0;
	CM->SendTask(cm);
	delete cm; 
//	fMessageList.append(cm);
}
*/

void OOPTaskManager::CleanUpTasks(){
	deque<OOPTask *>::iterator i;
	for(i=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){
		OOPTask *t =  (*i);
//		cout << "Is Task Compatible " << (!t->AmICompatible()) << endl;
		if(!t->AmICompatible()){
			
			TM->CancelTask(t->Id());
			i--;
		}

	}
}
void OOPTaskManager::Execute(){
	CM->ReceiveMessages();
	deque<OOPTask *>::iterator i;
	i=fTimeConsuming.begin();
	cout << "TTaskManager.Execute Queued task ids proc = " << fProc << "\n";
	cout << "Entering task list loop" << endl;
	bool continua = true;
	while(continua){
		for(i=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){//while(!fTimeConsuming.empty()){//=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){
			OOPTask *t =  (*i);
			if (t->CanExecute()){
				cout << "Executing task on processor " << fProc << endl;
				t->Execute();
				cout << (*i) << ":";
				OOPObjectId id;
				id = t->Id();
				id.Print(cout);
				//t->ReleaseAccessRequests();
				//Apagar tarefas apenas com a condição das condições de acesso aos dados.
				//fTimeConsuming.erase(i);
				//delete (*i);
			}
			TM->Print(cout);
			DM->CheckAccessRequests();

		//	i++;
		//	if(i==fTimeConsuming.end()) i=fTimeConsuming.begin();
		}
		TM->CleanUpTasks();
		if(fTimeConsuming.empty()){
			continua=false;
		}
	}
	cout << endl;


	CM->SendMessages();
	
}
/*
void TTaskManager::ExecDaemon(){
	//Pix iprev = 0, i = 0, inext = 0;
	deque<OOPTask *>::iterator iprev, i, inext;
	i = fDaemon.begin();
	while(i!=fDaemon.end()) {
		inext = i;
		inext ++;//fDaemon.next(inext);
		OOPTask *t =  fDaemon(i);
		if(t) {
			OOPMReturnType res = t->Execute();
			switch(res) {
				case ESuccess :
				case EFail :
					if(iprev!=fDaemon.end()) fDaemon.del_after(iprev);
					else fDaemon.remove_front();
					delete t;
					break;
				case EContinue :
					iprev = i;
					break;
			}
		} else {
				if(iprev) fDaemon.del_after(iprev);
				else fDaemon.remove_front();
		}
		i = inext;
	}
}
*/


OOPObjectId OOPTaskManager::GenerateId(){		// Generate a unique id number
	fLastCreated++;
	if(fLastCreated >= fMaxId) exit(-1);// the program ceases to function
    OOPObjectId tmp(fProc, fLastCreated);
	return tmp;
}


OOPTask *OOPTaskManager::FindTask(OOPObjectId taskid){	// find the task with the given id
	deque<OOPTask * >::iterator i;
	for(i=fTimeConsuming.begin(); i!=fTimeConsuming.end(); i++){
		OOPTask *t = (OOPTask*) (*i);
		if (t->Id() == taskid) return t;
	}
	return 0;
}

/**
 * reorder the tasks according to their priority
 * No local implementation found
 */
void OOPTaskManager::Reschedule(){
}

void OOPTaskManager::Print(ostream & out){
	out << "Task Manager data structure \t" << endl;
	out << "TM Processor " << fProc << endl;
	out << "Id of Last Created Task \t" << fLastCreated << endl;
	out << "Maximum number of tasks available \t" << fMaxId << endl;
	out << "Queued Daemon tasks ---------\t" << endl;
	out << "Number of Daemon tasks \t" << fDaemon.size() << endl;
	deque<OOPTask *>::iterator i;
	for(i=fDaemon.begin();i!=fDaemon.end();i++){
		(*i)->Print(out);
	}
	out << "Queued Time Consuming tasks ---------" << endl;
	out << "Number of Time Consuming tasks \t" << fTimeConsuming.size() << endl;
	for(i=fTimeConsuming.begin();i!=fTimeConsuming.end();i++){
		(*i)->Print(out);
	}
}

