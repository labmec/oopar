#include "ooptaskmanager.h"
#include "oopcommmanager.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif
#include "oopdatamanager.h"
#include "ooptaskcontrol.h"

#include "oopwaittask.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>

#include <sys/time.h>
class OOPStorageBuffer;
class   OOPTask;
class   OOPDataVersion;
class   OOPSaveable;
class   OOPObjectId;
class	OOPTerminationTask;
//#include <sys/types.h>
//#include <unistd.h>

#include <sstream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskManager"));
static LoggerPtr tasklogger(Logger::getLogger("OOPAR.OOPTaskManager.task"));

void OOPTaskManager::main ()
{
}
void OOPTaskManager::TransferExecutingTasks(){
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	list < OOPTaskControl * >::iterator sub;
//	int listsize = fExecuting.size();
	sub = fExecuting.begin();
	OOPTaskControl * auxtc=0;
	while (sub != fExecuting.end()){
		bool isfinished=false;
    auxtc = (*sub);
//		pthread_mutex_lock(&fExecutingMutex);
		if(auxtc) 
    {
      isfinished = auxtc->TaskFinished();
    } else
    {
      stringstream sout;
      sout << __FUNCTION__ << " I dont understand \n";
      LOG4CXX_DEBUG (logger,sout.str());
    }
  
//		pthread_mutex_unlock(&fExecutingMutex);
		if (isfinished){
      auxtc->Join();
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << "Task finished " << auxtc->Task()->Id() << " classid " << auxtc->Task()->ClassId();
      LOG4CXX_DEBUG (logger,sout.str());
			OOPObjectId id;
			id = auxtc->Task ()->Id ();
			auxtc->Depend ().SetExecuting (id, false);
			auxtc->Depend ().ReleaseAccessRequests (id);
      auxtc->Task()->Depend().ClearPointers();
			fFinished.push_back(auxtc);
    	list < OOPTaskControl * >::iterator keep;
      keep = sub;
      sub++;
			fExecuting.erase(keep);
		} else
    {
      sub++;
    }
	}

}

#ifdef MPI
#define MT

/*
void * OOPTaskManager::TriggerTask(void * data){
	OOPTaskControl * tc = static_cast<OOPTaskControl *> (data);
	OOPTaskManager * lTM = dynamic_cast<OOPTaskManager *> (TM);
	tc->Task ()->Execute ();
	pthread_mutex_lock(&lTM->fExecutingMutex);
	tc->Task()->SetExecuting(false);
	pthread_mutex_unlock(&lTM->fExecutingMutex);
	return NULL;
}
*/

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
  {
    std::stringstream sout;
    sout << "Entering task list loop";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	lTM->fKeepGoing=true;
	lTM->ExecuteDaemons();
	while (lTM->fKeepGoing) {
		CM->ReceiveMessages();
		lTM->ExecuteDaemons();
		while (lTM->fExecutable.size () && lTM->fExecuting.size() < 5) {
			i = lTM->fExecutable.begin ();
			OOPTaskControl *tc = (*i);
			lTM->fExecutable.erase(i);
			lTM->fExecuting.push_back(tc);
			tc->Task()->SetExecuting(true);
      {
        stringstream sout;
        sout << "Entering taskcontrol execute for task " << tc->Task()->Id() << " classid " << tc->Task()->ClassId();
        LOG4CXX_DEBUG (tasklogger,sout.str());
      }
      tc->Execute();
			lTM->TransferExecutingTasks();
			DM->SubmitAllObjects();
		}
		lTM->TransferExecutingTasks();
		lTM->TransferFinishedTasks ();
//    if(!CM->GetProcID()) cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << " before receive " << CM->GetProcID() <<   "\n";
		CM->ReceiveMessages ();
//    if(!CM->GetProcID()) cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << " after receive " << CM->GetProcID() <<   "\n";
		lTM->TransferSubmittedTasks ();
		CM->SendMessages ();
		lTM->ExecuteDaemons();
		//wait
//		pthread_mutex_lock(&fExecuteMutex);
		if(!lTM->HasWorkTodo () && lTM->fKeepGoing){
//			cout << "PID" << getpid() << endl;
      pthread_mutex_lock(&lTM->fSubmittedMutex);
      if(lTM->fSubmittedList.size())
      {
        pthread_mutex_unlock(&lTM->fSubmittedMutex);
      }
      else
      {
        timeval now;
        gettimeofday(&now,0);
        now.tv_usec += 1;
        now.tv_sec += now.tv_usec/1000000;
        now.tv_usec %= 1000000;
        timespec next;
        next.tv_sec = now.tv_sec;
        next.tv_nsec = now.tv_usec*1000;
//        cout << __PRETTY_FUNCTION__ << " TaskManager going to sleep\n";
        cout.flush();
        pthread_cond_timedwait(&lTM->fExecuteCondition, &lTM->fSubmittedMutex,&next);
        pthread_mutex_unlock(&lTM->fSubmittedMutex);
      }
		}
	}
	CM->SendMessages ();
	
	return NULL;
}
#endif
OOPTaskManager::OOPTaskManager (int proc)
{
	fProc = proc;
	fLastCreated = 0;//NUMOBJECTS * fProc;
//	fMaxId = fLastCreated + NUMOBJECTS;

//	pthread_mutex_init(&fExecutingMutex, NULL);
//	pthread_mutex_init(&fFinishedMutex, NULL);
	pthread_mutex_init(&fSubmittedMutex, NULL);
  pthread_cond_init(&fExecuteCondition, NULL);
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
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_ERROR (logger,sout.str());
  }
	list < OOPTaskControl * >::iterator i;
	bool found = false;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == TaskId) {
			found = true;
			tc->Depend ().GrantAccess (depend, objptr);
      {
        stringstream sout;
        sout << "Access Granted to taskId " << TaskId  << " classid " << tc->Task()->ClassId();
        sout << " on data " << depend.Id();
        LOG4CXX_DEBUG (tasklogger,sout.str());
      }
			if (tc->Depend ().CanExecute ()) {
				TransfertoExecutable (tc->Task ()->Id ());
        {
          stringstream sout;
          sout << "OOPTaskManager task is executable " << TaskId << " classid " << tc->Task()->ClassId();
          LOG4CXX_DEBUG (tasklogger,sout.str());
        }
			}
			break;
		}
	}
	if (!found) {
    stringstream sout;
		sout << "Task not found on current TM: File:" << __FILE__ <<
			" Line:" << __LINE__ << endl;
		sout << " Task ";
		TaskId.Print (sout);
    LOG4CXX_ERROR (tasklogger,sout.str());
	}
}
void OOPTaskManager::RevokeAccess (const OOPObjectId & TaskId,
				   const OOPMDataDepend & depend)
{
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_ERROR (logger,sout.str());
  }
	list < OOPTaskControl * >::iterator i;
	bool found = false;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == TaskId) {
			found = true;
			tc->Depend ().RevokeAccess (depend);
      stringstream sout;
			sout << "Access Revoked to taskId " << TaskId << " classid " << tc->Task()->ClassId();
			sout << " on data " << depend.Id();
      LOG4CXX_DEBUG (logger,sout.str());
			break;
		}
	}
	if (!found) {
    stringstream sout;
		sout << "OOPTaskManager::RevokeAccess Task not found on current TM: File:" << __FILE__ << " Line:" << __LINE__ << endl;
		sout << "Task ";
		TaskId.Print (sout);
    LOG4CXX_DEBUG (logger,sout.str());
	}
}
void OOPTaskManager::SubmitDaemon (OOPDaemonTask * task) {
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
  if(task->GetProcID() != this->fProc) 
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Sending a daemon task to proc " << task->GetProcID() << " classid " << task->ClassId();
    CM->SendTask(task);
    LOG4CXX_DEBUG (logger,sout.str());
  }
  else
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Submitting a daemon task " << " classid " << task->ClassId();
    LOG4CXX_DEBUG (logger,sout.str());
    fDaemon.push_back(task);
  }
}
OOPObjectId OOPTaskManager::Submit (OOPTask * task)
{
  {
    stringstream sout;
    sout << "Calling Submit on OOPTaskManager ";
    LOG4CXX_DEBUG (logger,sout.str());
  }

	OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(task);
        OOPObjectId id;
	if(dmt) {
    stringstream sout;
    sout << "Task Submitted is a daemon";
    LOG4CXX_DEBUG (logger,sout.str());
	} else 
  {
    id = task->Id();
    if(id.IsZeroOOP()) id = GenerateId ();
    task->SetTaskId (id);
  }
  { 
    stringstream sout;     
    sout << __PRETTY_FUNCTION__ << " Task with id " << task->Id() << " submitted for processor" << task->GetProcID() << " classid " << task->ClassId();
    LOG4CXX_DEBUG (tasklogger,sout.str());
  }
#ifdef DEBUG
      OOPWaitTask *wait = dynamic_cast<OOPWaitTask *> (task);
  if(!wait && !dmt && !CM->GetProcID())
  {
      std::ostringstream FileName, FileName2,command,subdir1,subdir2,subdir3;
      subdir1 << "taskman" << CM->GetProcID();
      subdir2 << "taskman" << CM->GetProcID() << "/orig";
      subdir3 << "taskman" << CM->GetProcID() << "/copy";
      mkdir(subdir1.str().c_str() , S_IRWXU | S_IXGRP | S_IRGRP| S_IXOTH | S_IROTH );
      mkdir(subdir2.str().c_str(), S_IRWXU | S_IXGRP | S_IRGRP| S_IXOTH | S_IROTH );
      mkdir(subdir3.str().c_str(), S_IRWXU | S_IXGRP | S_IRGRP| S_IXOTH | S_IROTH );
      FileName << subdir2.str() << "/" << task->ClassId() << ".sav";
      FileName2 << subdir3.str() << "/" << task->ClassId() << ".sav";
     {
      TPZFileStream PZFS;
      PZFS.OpenWrite(FileName.str());
      task->Write(PZFS,1);
    }
     {
      TPZFileStream PZFS;
      PZFS.OpenRead(FileName.str());
      OOPTask *test = (OOPTask *) TPZSaveable::Restore(PZFS,0);
      TPZFileStream PZFS2;
      PZFS2.OpenWrite(FileName2.str());
      test->Write(PZFS2,1);
      delete test;
    }
    command << "diff --brief " << FileName.str() << " " << FileName2.str() << endl;
    FILE *pipe = popen(command.str().c_str(),"r");
#ifdef DEBUGALL
    cout << "Command executed " << command.str() << endl;
#endif
    char *compare = new char[256];
    compare[0] = '\0';
    char **compptr = &compare;
    size_t size = 256;
    getline(compptr,&size,pipe);
//    fscanf(pipe,"%s",compare);
    pclose(pipe);
    if(strlen(compare))
    {
      cout << __PRETTY_FUNCTION__ << " The writing process produced an error for class " << task->ClassId() << compare << endl;
    }
    delete []compare;    
  }
#endif
	pthread_mutex_lock(&fSubmittedMutex);
	fSubmittedList.push_back (task);
  pthread_cond_signal(&fExecuteCondition);
	pthread_mutex_unlock(&fSubmittedMutex);
	return id;
}
int OOPTaskManager::NumberOfTasks ()
{
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	pthread_mutex_lock(&fSubmittedMutex);
//	pthread_mutex_lock(&fFinishedMutex);
	int numtasks = fExecutable.size () + fFinished.size () +
		fSubmittedList.size () + fTaskList.size () + fDaemon.size();
//	pthread_mutex_unlock(&fFinishedMutex);
	pthread_mutex_unlock(&fSubmittedMutex);
	return numtasks;
}
bool OOPTaskManager::HasWorkTodo ()
{
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	pthread_mutex_lock(&fSubmittedMutex);
//	pthread_mutex_lock(&fFinishedMutex);
	int numtasks = fExecutable.size () + fFinished.size () +
		fSubmittedList.size () + fDaemon.size();
	pthread_mutex_unlock(&fSubmittedMutex);
//	pthread_mutex_unlock(&fFinishedMutex);
	return numtasks != 0;
}


int OOPTaskManager::ChangePriority (OOPObjectId & taskid, int newpriority)
{
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	OOPTask *t = FindTask (taskid);
	if (t) {
		t->ChangePriority (newpriority);
		Reschedule ();
	}
	return 0;
}
int OOPTaskManager::CancelTask (OOPObjectId taskid)
{
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_ERROR (logger,sout.str());
  }
  
	list < OOPTaskControl * >::iterator i;	// , iprev, atual;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = *i;
		if (tc->Task ()->Id () == taskid) {
      stringstream sout;
      sout << "Task erased ";
      sout << "Task ID " << tc->Task()->Id() << " classid " << tc->Task()->ClassId();
      LOG4CXX_DEBUG (logger,sout.str());
			tc->Depend ().ReleaseAccessRequests (tc->Task ()->
							     Id ());
			delete tc;
			fTaskList.erase (i);
			return 1;
		}
	}
	return 0;
}
void OOPTaskManager::ExecuteDaemons() {
  
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
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
  stringstream sout;
#ifndef MT	
	//Qual é o service thread ?
	// O service thread e a linha de execucao do programa principal
	DM->SubmitAllObjects();
	CM->ReceiveMessages ();
	TransferSubmittedTasks ();
	list < OOPTaskControl * >::iterator i;
	fKeepGoing=true;
	ExecuteDaemons();
	while (fKeepGoing) {
		//pthread_mutex_lock(&fExecuteMutex);
		DM->SubmitAllObjects();
		CM->ReceiveMessages();
		ExecuteDaemons();
		while (fExecutable.size ()) {
			//pthread_mutex_unlock(&fExecuteMutex);
			i = fExecutable.begin ();
			OOPTaskControl *tc = (*i);
			tc->Task ()->Execute ();
			OOPObjectId id;
			id = tc->Task ()->Id ();
			tc->Depend ().SetExecuting (tc->Task ()->Id (),
						      false);
			tc->Depend ().ReleaseAccessRequests (tc->Task ()->
							       Id ());
			DM->SubmitAllObjects();

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
      stringstream sout;
			sout << "Leaving blocking receive PID " << getpid();
      LOG4CXX_DEBUG (logger,sout.str());
			DM->SubmitAllObjects();
		}
//		pthread_mutex_unlock(&fExecuteMutex)
	}
	CM->SendMessages ();
#else
//	pthread_t execute_thread;
  {
    stringstream sout;
	  sout << "Creating service thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	if(pthread_create(&fExecuteThread, NULL, ExecuteMT, this)){
    stringstream sout;
    sout << "Fail to create service thread\n";
    sout << "Going out";
    LOG4CXX_ERROR (logger,sout.str());
	} 
 {
   stringstream sout;
   sout << "Created succesfuly";
   LOG4CXX_DEBUG (logger,sout.str());
 }
#endif
}
void OOPTaskManager::Wait(){
	pthread_join(fExecuteThread,NULL);
}
void OOPTaskManager::SetKeepGoing(bool go){
	fKeepGoing = go;
}
OOPObjectId OOPTaskManager::GenerateId ()
{      // Generate a unique id number
	fLastCreated++;
//	if (fLastCreated >= fMaxId)
// {
//   cout << __PRETTY_FUNCTION__ << " available Ids exhausted bye!\n";
//    exit (-1); // the program ceases to function
//  }
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
void OOPTaskManager::Print (std::ostream & out)
{
	out << "Task Manager data structure \t" << endl;
	out << "TM Processor " << fProc << endl;
	out << "Id of Last Created Task \t" << fLastCreated << endl;
//	out << "Maximum number of tasks available \t" << fMaxId << endl;
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
  DM->SubmitAllObjects();

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
               	OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(aux);
		if (aux->GetProcID () != fProc) {
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << "Transferring task " << aux->Id() << " from " << fProc <<
           " to proc " <<  aux->GetProcID();
      LOG4CXX_DEBUG(tasklogger,sout.str())
			CM->SendTask (aux);
		}
                else if(dmt)
                {
                  SubmitDaemon(dmt);
                }
		else 
                {
			OOPTaskControl *tc = new OOPTaskControl (aux);
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
    DM->SubmitAllObjects();
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
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	list < OOPTaskControl * >::iterator sub;
	int listsize = fFinished.size();
  if(!listsize) return;
  DM->SubmitAllObjects();
	sub = fFinished.begin();
	OOPTaskControl * auxtc=0;
	if(listsize){
		auxtc = (*sub);
		fFinished.erase(sub);
	}
	while (auxtc){
   {
     stringstream sout;
     sout << __PRETTY_FUNCTION__ << " task " << auxtc->Task()->Id() << " classid " <<
         auxtc->Task()->ClassId() << " finished";
     LOG4CXX_DEBUG(tasklogger,sout.str());
   }
		if (auxtc->Task ()->IsRecurrent () && auxtc->Task ()->GetProcID () != fProc) {
      {
        stringstream sout;
        sout << __PRETTY_FUNCTION__ << " task " << auxtc->Task()->Id() << " classid " <<
            auxtc->Task()->ClassId() << " transferred from " << fProc << " to " << auxtc->Task ()->GetProcID();
        LOG4CXX_DEBUG(tasklogger,sout.str());
      }
			CM->SendTask (auxtc->Task ());
			auxtc->ZeroTask ();
			delete auxtc;
		}else if(auxtc->Task ()->IsRecurrent ()) {
      {
        stringstream sout;
        sout << __PRETTY_FUNCTION__ << " task " << auxtc->Task()->Id() << " classid " <<
            auxtc->Task()->ClassId() << " resubmitted";
        LOG4CXX_DEBUG(tasklogger,sout.str());
      }
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
//		pthread_mutex_lock(&fFinishedMutex);
		listsize = fFinished.size();
		auxtc=0;
		sub = fFinished.begin ();
		if(listsize) {
			auxtc = (*sub);
			fFinished.erase (sub);
		}
// pthread_mutex_unlock(&fFinishedMutex);
	}
}
void OOPTaskManager::TransfertoExecutable (const OOPObjectId & taskid)
{
  if(!pthread_equal(fExecuteThread,pthread_self()))
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOG4CXX_DEBUG (logger,sout.str());
  }
	list < OOPTaskControl * >::iterator i;
	for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
		OOPTaskControl *tc = (*i);
		if (tc->Task ()->Id () == taskid) {
			tc->Task ()->SetDependencyList (tc->Depend ());
			tc->Depend ().SetExecuting (taskid, true);
			OOPDaemonTask *dmt =
				dynamic_cast < OOPDaemonTask * >(tc->Task ());
			if (dmt) {
        LOG4CXX_ERROR (logger,"TM::TransfertoExecutable inconsistent datastructure\nThere is daemontask in the fTaskList\n");
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
void OOPTaskManager::PrintTaskQueues(char * msg, std::ostream & out){
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
OOPTMTask::OOPTMTask(): OOPDaemonTask() {
  LOG4CXX_DEBUG (logger,"Should Never be Called\n");
}
OOPTMTask::~OOPTMTask(){}
OOPMReturnType OOPTMTask::Execute (){
  LOG4CXX_DEBUG (logger,"Should Never be Called\n");
  return EContinue;
}


OOPTerminationTask::~OOPTerminationTask (){}
OOPTerminationTask::OOPTerminationTask (int ProcId) : OOPTask(ProcId){}
OOPTerminationTask::OOPTerminationTask (const OOPTerminationTask & term): OOPTask(term)
{
}

OOPMReturnType OOPTerminationTask::Execute (){
  TM->SetKeepGoing(false);
  LOG4CXX_DEBUG (logger,"----------------------------------------------TM Finished\n");
	return ESuccess;
}

void OOPTerminationTask::Write(TPZStream & buf, int withclassid){
	OOPTask::Write(buf, withclassid);

}
void OOPTerminationTask::Read(TPZStream & buf, void * context){
	OOPTask::Read(buf, context);

}

long int OOPTerminationTask::ExecTime(){
	return -1;
}
TPZSaveable *OOPTerminationTask::Restore (TPZStream & buf, void * context){
	OOPTerminationTask*v = new OOPTerminationTask(0);
	v->Read (buf);
	return v;
}
extern OOPTaskManager *TM;
