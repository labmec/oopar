
#include "ooptaskmanager.h"
#include "oopcommmanager.h"
#include "oopdatamanager.h"
#include "ooptaskcontrol.h"
//#include "tmultidata.h"
//#include "tmultitask.h"

class OOPTask;
class OOPDataVersion;

class OOPSaveable;


//class TMultiTask;
//class TMultiData;
class OOPObjectId;

void OOPTaskManager::main(){

  /*
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



    //	OOPMDataState st = EWriteAccess;
    //	OOPDataVersion ver;

    //ver.SetLevelVersion(0,1);
    //	ver.Print(cout);
    //	task->AddDependentData(data_id, st, ver);
    //	ver.SetLevelVersion(0,1);
    //	ver.Print(cout);
    //	task_2->AddDependentData(data_id, st, ver);

    //	task->Submit();
    //	task_2->Submit();

    //DM->CheckAccessRequests();
    //	task->Print(cout);
    //	task_2->Print(cout);

    TM->Execute();
  */
}

OOPTaskManager::OOPTaskManager(int proc){
  fProc = proc;
  fLastCreated = NUMOBJECTS * fProc;
  fMaxId = fLastCreated + NUMOBJECTS;
}

OOPTaskManager::~OOPTaskManager() {
  deque<OOPTask *>::iterator i;
  for(i = fSubmittedList.begin(); i!=fSubmittedList.end(); i++)
    delete *i;
  deque<OOPTaskControl *>::iterator itc;
  for(itc = fExecutable.begin(); itc!=fExecutable.end(); itc++) delete *itc;
  for(itc = fFinished.begin(); itc!=fFinished.end(); itc++) delete *itc;
  for(itc = fTaskList.begin(); itc!=fTaskList.end(); itc++) delete *itc;
}

//Passar o ID do task e do dado.
void OOPTaskManager::NotifyAccessGranted(const OOPObjectId & TaskId, const OOPMDataDepend & depend
										 , OOPMetaData * objptr)
{
  deque<OOPTaskControl*>::iterator i;
  bool found = false;

  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    OOPTaskControl * tc = (*i);
    if(tc->Task()->Id() == TaskId){
      found = true;
      tc->Depend().GrantAccess( depend, objptr);
#ifdef DEBUG
      cout << "Access Granted to taskId";
      TaskId.Print(cout);
      cout << " on data ";
      depend.Id().Print(cout);
#endif
	  if(tc->Depend().CanExecute()) {
		  TransfertoExecutable(tc->Task()->Id());
		  cout << "OOPTaskManager task is executable ";
		  TaskId.Print(cout);

	  }
	  break;
	}
  }

  if (!found) {
    cerr << "Task not found on current TM: File:" << __FILE__ << " Line:" << __LINE__ << endl;
	cerr << "Task \n";
	TaskId.Print(cerr);
  }
}


void OOPTaskManager::RevokeAccess(const OOPObjectId & TaskId, const OOPMDataDepend & depend)
{
  deque<OOPTaskControl*>::iterator i;
  bool found = false;

  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    OOPTaskControl * tc = (*i);
    if(tc->Task()->Id() == TaskId){
      found = true;
      tc->Depend().RevokeAccess( depend);
#ifdef DEBUG
      cout << "Access Revoked to taskId";
      TaskId.Print(cout);
      cout << " on data ";
      depend.Id().Print(cout);
#endif
	  break;
	}
  }

  if (!found) {
    cerr << "OOPTaskManager::RevokeAccess Task not found on current TM: File:" << __FILE__ << " Line:" << __LINE__ << endl;
	cerr << "Task \n";
	TaskId.Print(cerr);
  }
}


OOPObjectId OOPTaskManager::Submit(OOPTask *task) {
		
  OOPObjectId id;
	//mutex lock 
  id = GenerateId();
  task->SetTaskId(id);
	
  fSubmittedList.push_back(task);
	//mutex unlock
  return id;
}


int OOPTaskManager::NumberOfTasks() {
  return fExecutable.size()+fFinished.size()+fSubmittedList.size()+fTaskList.size();
}


int OOPTaskManager::ChangePriority(OOPObjectId &taskid, int newpriority) {
  //if(!ExistsTask(taskid)) return 0;
  //int proc = fTaskExist[taskid];
  //	int proc = taskid.GetProcId();
  OOPTask *t = FindTask(taskid);
  if(t) {
    t->ChangePriority(newpriority);
    Reschedule();
  }
  return 0;
}

int OOPTaskManager::CancelTask(OOPObjectId taskid){
  deque<OOPTaskControl * >::iterator i;//, iprev, atual;
	
  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    OOPTaskControl * tc =  *i;
    if (tc->Task()->Id() == taskid){
      cout << "Task erased" << endl;
      cout << "Task ID "; tc->Task()->Id().Print(cout);
	  tc->Depend().ReleaseAccessRequests(tc->Task()->Id());
      delete tc;
      fTaskList.erase(i);
      return 1;
    }
  }
  return 0;
}
	
void OOPTaskManager::CleanUpTasks(){
#ifndef WIN32
#warning "CleanUpTasks is empty"
#warning "Actuating on the TaskLists, multithreading sychronization required"
#warning "CleanUpTasks must always delete tasks only from the fFinished List ????"
#endif
	//lock over the mutex for the fTaskList object.
	deque <OOPTaskControl *>::iterator i = fTaskList.begin();
	while(i!=fTaskList.end()){
		if(!(*i)->Depend().AmIConsistent()){
			//Inconsistency detected on the fTaskList deque.
			cerr << "Task which was never executed contains inconsistent data depency\n";
			cerr << "Task deletion request will be submitted\n";
			TM->CancelTask((*i)->Task()->Id());
		}
		i++;
			
	}
	//unlock over mutex.
	//lock over mutex for the fExecutable object;
	deque< OOPTaskControl *>::iterator j = fExecutable.begin();
	while(j!=fExecutable.end()){
		if(!(*j)->Depend().AmIConsistent()){
			//Inconsistency detected on the fExecutable deque.
			cerr << "Inconsistent task dependency on fExecutable task list.\n";
			cerr << "Task deletion request will be submitted\n";
			cerr << "File " << __FILE__ << ": Line " << __LINE__ << endl;
			TM->CancelTask((*j)->Task()->Id());
		}
		j++;
			
	}
	//unlock mutex
	//lock over mutex for the fFinished object
	deque <OOPTaskControl *>::iterator k = fFinished.begin();
	while(k!=fFinished.end()){
		if(!(*k)->Depend().AmIConsistent()){
			//Inconsistency detected on the fFinished deque.
			cout << "Inconsistent task dependency on fFinished task list.\n";
			cout << "Task deletion request will be submitted\n";
			cout << "File " << __FILE__ << ": Line " << __LINE__ << endl;
			TM->CancelTask((*k)->Task()->Id());
		}
		k++;
			
	}
	//unlock mutex
	//lock over mutex for the fSubmittedList object
	deque <OOPTask *>::iterator l = fSubmittedList.begin();
	while(l!=fSubmittedList.end()){
		if(!(*l)->Depend().AmIConsistent()){
			//Inconsistency detected on the fFinished deque.
			cerr << "Inconsistent task dependency on fSubmittedList task list.\n";
			cerr << "Propably some mal functioning\n";
			cerr << "Task deletion request will be submitted\n";
			cerr << "File " << __FILE__ << ": Line " << __LINE__ << endl;
			TM->CancelTask((*l)->Id());
		}
		l++;
			
	}
	//unlock mutex.

  
	
}
void OOPTaskManager::Execute(){
  CM->ReceiveMessages();
  TransferSubmittedTasks();
  deque<OOPTaskControl *>::iterator i;
  //  cout << "TTaskManager.Execute Queued task ids proc = " << fProc << "\n";
  //  cout << "Entering task list loop" << endl;

  while(fExecutable.size()) {
	  while(fExecutable.size()){
		i=fExecutable.begin();
		(*i)->Task()->Execute();
//		cout << (*i)->Task() << ":";
		OOPObjectId id;
		id = (*i)->Task()->Id();
		(*i)->Depend().SetExecuting((*i)->Task()->Id(),false);
 		(*i)->Depend().ReleaseAccessRequests((*i)->Task()->Id());
#ifdef DEBUG		  
		//		cout << "Executing task on processor " << fProc << endl;
		//		id.Print(cout);
		//		cout.flush();
#endif		  
		fFinished.push_back(*i);
		fExecutable.erase(i);
	  }
	  TransferFinishedTasks();
	  CM->ReceiveMessages();
	  TransferSubmittedTasks();
	  CM->SendMessages();
	}


	
}

OOPObjectId OOPTaskManager::GenerateId(){		// Generate a unique id number
  fLastCreated++;
  if(fLastCreated >= fMaxId) exit(-1);// the program ceases to function
  OOPObjectId tmp(fProc, fLastCreated);
  return tmp;
}


OOPTask *OOPTaskManager::FindTask(OOPObjectId taskid){	// find the task with the given id
  deque<OOPTaskControl * >::iterator i;
  for(i=fTaskList.begin(); i!=fTaskList.end(); i++){
    OOPTask *t = (*i)->Task();
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
  out << "Queued Time Consuming tasks ---------" << endl;
  out << "Number of Time Consuming tasks \t" << fTaskList.size() << endl;
  deque<OOPTaskControl *>::iterator i;
  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    (*i)->Task()->Print(out);
  }
}

void OOPTaskManager::TransferSubmittedTasks() {
	deque<OOPTask *>::iterator sub;
	while(fSubmittedList.begin() != fSubmittedList.end()) {
		sub = fSubmittedList.begin();
		if((*sub)->GetProcID() != fProc) {
			CM->SendTask(*sub);
			fSubmittedList.erase(sub);
		} else {
			OOPTaskControl *tc = new OOPTaskControl(*sub);
			fSubmittedList.erase(sub);
			fTaskList.push_back(tc);
			if(tc->Depend().SubmitDependencyList(tc->Task()->Id())) {
				// their is no incompatibility between versions
			} else {
				// there is an incompatibility of versions
				CancelTask(tc->Task()->Id());
			}
		}
	}
}

void OOPTaskManager::TransferFinishedTasks() {
	deque<OOPTaskControl *>::iterator sub;
	while(fFinished.size()) {
		sub = fFinished.begin();
		if((*sub)->Task()->GetProcID() != fProc) {
			CM->SendTask((*sub)->Task());
			(*sub)->ZeroTask();
			delete (*sub);
			fFinished.erase(sub);
		} else if((*sub)->Task()->IsRecurrent()) {
			(*sub)->Depend() = (*sub)->Task()->GetDependencyList();
			(*sub)->Depend().ClearPointers();
			fTaskList.push_back(*sub);

			if((*sub)->Depend().SubmitDependencyList((*sub)->Task()->Id())) {
				// their is no incompatibility between versions
			} else {
				// there is an incompatibility of versions
				CancelTask((*sub)->Task()->Id());
			}
			fFinished.erase(sub);
		} else {
			delete (*sub);
			fFinished.erase(sub);
		}
	}
}

void OOPTaskManager::TransfertoExecutable(const OOPObjectId &taskid){

  deque<OOPTaskControl*>::iterator i;
  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    OOPTaskControl * tc = (*i);
    if(tc->Task()->Id() == taskid){
		tc->Task()->SetDependencyList(tc->Depend());
		tc->Depend().SetExecuting(taskid,true);
		OOPDaemonTask *dmt = dynamic_cast<OOPDaemonTask *> (tc->Task());
		if(dmt) {
			fExecutable.push_front(tc);
		} else {
			fExecutable.push_back(tc);
		}
		fTaskList.erase(i);
		break;
	}
  }
}

