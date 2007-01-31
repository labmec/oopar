#ifdef OOP_MPI
#include "oopmpicomm.h"
#endif


#include <errno.h>

#include "ooptaskmanager.h"
#include "oopcommmanager.h"
#include "oopdatamanager.h"
#include "ooptaskcontrol.h"

#include "oopwaittask.h"

#include "ooptmlock.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>

#include <sys/time.h>

#ifdef OOP_MPE
class OOPSoloEvent;
#endif
class OOPStorageBuffer;
class OOPTask;
class OOPDataVersion;
class OOPSaveable;
class OOPObjectId;
class OOPTerminationTask;

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
logger (Logger::getLogger ("OOPAR.OOPTaskManager"));
static LoggerPtr
tasklogger (Logger::getLogger ("OOPAR.OOPTaskManager.task"));
#endif

static ofstream
tlog ("TM_time_log.txt");


void OOPTaskManager::SnapShotMe()
{
  stringstream filename;
  filename << "TMSnapShot" << fProc << ".txt";
  ofstream out(filename.str().c_str(), ios::app);
  std::stringstream sout;
  sout << "TaskManager SnapShot\n";
  sout << "Processor" << fProc << endl;
  sout << "Daemon size " << fDaemon.size() << endl;
  sout << "Executable size " << fExecutable.size() << endl;
  sout << "Finished size " << fFinished.size() << endl;
  sout << "Finished size " << fFinished.size() << endl;
  sout << "TaskList size " << fTaskList.size() << endl;
  
  sout << "---DaemonTasks---" << endl;
  std::list<OOPDaemonTask * >::iterator lit;
  for(lit=fDaemon.begin();lit!=fDaemon.end();lit++)
  {
    (*lit)->Print(sout);
  }
  {
    sout << "---Executable---" << endl;
    std::list<OOPTaskControl * >::iterator lit;
    for(lit=fExecutable.begin();lit!=fExecutable.end();lit++)
    {
      (*lit)->Print(sout);
    }
  }
  {
    sout << "---Executing---" << endl;
    std::list<OOPTaskControl * >::iterator lit;
    for(lit=fExecuting.begin();lit!=fExecuting.end();lit++)
    {
      (*lit)->Print(sout);
    }
  }
  {
    sout << "---Finished---" << endl;
    std::list<OOPTaskControl * >::iterator lit;
    for(lit=fFinished.begin();lit!=fFinished.end();lit++)
    {
      (*lit)->Print(sout);
    }
  }
  {
    sout << "---SubmittedList---" << endl;
    std::list<OOPTask * >::iterator lit;
    for(lit=fSubmittedList.begin();lit!=fSubmittedList.end();lit++)
    {
      (*lit)->Print(sout);
    }
  }
  {
    sout << "---TaskList---" << endl;
    std::list<OOPTaskControl * >::iterator lit;
    for(lit=fTaskList.begin();lit!=fTaskList.end();lit++)
    {
      (*lit)->Print(sout);
    }
  }
  
  sout << "---------Finished STL Data Components------------" << endl;
  out << sout.str().c_str();
}


void OOPTaskManager::GrantAccess(OOPAccessTag & tag)
{
  std::pair< TMMessageType, OOPAccessTag> item(ETMAccessGranted, tag);
  OOPTMLock lock;
  fMessages.push_back(item);
}
void
OOPTaskManager::SetNumberOfThreads (const int n)
{
  OOPTaskManager::fNumberOfThreads = n;
}

int
OOPTaskManager::NumberOfThreads ()
{
  return OOPTaskManager::fNumberOfThreads;
}

void OOPTaskManager::TransferExecutingTasks ()
{
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    //cout << __PRETTY_FUNCTION__ << " called by foreign thread\n";
    LOGPZ_ERROR (logger, sout.str ());

#endif
    return;
  }
  list < OOPTaskControl * >::iterator sub;
  //int listsize = fExecuting.size();
  sub = fExecuting.begin ();
  OOPTaskControl *auxtc = 0;
  while (sub != fExecuting.end ()) {
    bool isfinished = false;
    auxtc = (*sub);
    {
      OOPTMLock lock;
      //pthread_mutex_lock(&fExecutingMutex);
      if (auxtc) {
  //      cout << __PRETTY_FUNCTION__ << " AUX TC VALID " << __LINE__ << endl;
        isfinished = auxtc->TaskFinished ();
      } else {
  #ifdef LOGPZ
        stringstream sout;
        sout << __FUNCTION__ << " I dont understand \n";
        //cout << __FUNCTION__ << " I dont understand \n";
        LOGPZ_ERROR (logger, sout.str ());
  #endif
      }
      // cout << __PRETTY_FUNCTION__ << " and line " << __LINE__ << endl;
      WakeServiceThread();
      //pthread_mutex_unlock(&fExecutingMutex);
    }
    
    if (isfinished) {
      //cout << __PRETTY_FUNCTION__ << " IS FINISHED and line " << __LINE__ <<
      //" taskid " << auxtc->Id() <<  endl;
      auxtc->Join ();
#ifdef LOGPZ
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << "Task finished " << auxtc->
	Id () << " classid " << auxtc->ClassId ();
      LOGPZ_DEBUG (logger, sout.str ());
#endif
      OOPObjectId id;
      id = auxtc->Id ();
      
      fFinished.push_back (auxtc);
      list < OOPTaskControl * >::iterator keep;
      keep = sub;
      fExecuting.erase (keep);
      // a finished task may have sneeked in during the unlock/lock sequence
      sub = fExecuting.begin ();
    } else {
      sub++;
    }
  }

}

#ifdef OOP_MPI
/**
	disparar o thread de execu�o da tarefa.
*/

void *
OOPTaskManager::ExecuteMT (void *data)
{
#ifdef LOGTIME
  tlog <<
    "time\tsubmitted\twaiting\texecutable\texecuting\tfinished\tdaemon\n";
#endif
  OOPTaskManager *lTM = static_cast < OOPTaskManager * >(data);
  sem_init(&lTM->fServiceSemaphore, 0,0);
  DM->SubmitAllObjects ();
  CM->ReceiveMessages ();
  lTM->TransferSubmittedTasks ();
  list < OOPTaskControl * >::iterator i;

  SszQueues curSz;
  curSz.set (lTM->fTaskList.size (), lTM->fExecutable.size (),
	     lTM->fExecuting.size ());

#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << "Entering task list loop";
    LOGPZ_DEBUG (logger, sout.str ());
  }
#endif
  lTM->fKeepGoing = true;
  lTM->ExecuteDaemons ();
  while (lTM->fKeepGoing || lTM->fExecuting.size())
  {
    CM->ReceiveMessages ();
    lTM->ExecuteDaemons ();
    while (lTM->fExecutable.size ()  && (int) lTM->fExecuting.size () < lTM->fNumberOfThreads) 
    {
      i = lTM->fExecutable.begin ();
      OOPTaskControl *tc = (*i);
      lTM->fExecutable.erase (i);
      lTM->fExecuting.push_back (tc);
#ifdef LOGPZ
      {
	stringstream sout;
	sout << "Entering taskcontrol execute for task " << tc->
	  Id () << " classid " << tc->ClassId ();
	LOGPZ_DEBUG (tasklogger, sout.str ());
      }
#endif
      tc->Execute ();
      lTM->TransferExecutingTasks ();
      DM->SubmitAllObjects ();
    }
    lTM->TransferExecutingTasks ();
    lTM->TransferFinishedTasks ();
    CM->ReceiveMessages ();

    lTM->TransferSubmittedTasks ();
    //cout << __PRETTY_FUNCTION__ << " and line " << __LINE__ << endl;
    lTM->ExecuteDaemons ();

#ifdef LOGTIME
    if (!curSz.
	IsEqual (lTM->fTaskList.size (), lTM->fExecutable.size (),
		 lTM->fExecuting.size ()))
    {
      timeval curtime;
      gettimeofday (&curtime, 0);

      tlog << curtime.tv_sec << ":" << curtime.tv_usec << "\t"
	<< lTM->fSubmittedList.size () << "\t"
	<< lTM->fTaskList.size () << "\t"
	<< lTM->fExecutable.size () << "\t"
	<< lTM->fExecuting.size () << "\t"
	<< lTM->fFinished.size () << "\t"
	<< lTM->fDaemon.size () << std::endl;
      curSz.set (lTM->fTaskList.size (), lTM->fExecutable.size (),
		 lTM->fExecuting.size ());
    }
#endif


    if (!lTM->HasWorkTodo ())
    {
      if (CM->NumProcessors () > 1)
      {
	timeval now;
	gettimeofday (&now, 0);
	now.tv_usec += 1000*10;
	now.tv_sec += now.tv_usec / 1000000;
	now.tv_usec %= 1000000;
	timespec next;
	next.tv_sec = now.tv_sec;
	next.tv_nsec = now.tv_usec * 1000;
	
#ifdef LOGPZ
        {
          std::stringstream sout;
          sout << __PRETTY_FUNCTION__ << " going to sleep";
          //LOGPZ_DEBUG(tasklogger,sout.str().c_str());
        }
#endif
        int retval = 0;
	retval = sem_timedwait(&lTM->fServiceSemaphore, &next);

        if(retval == ETIMEDOUT){
          //LOGPZ_DEBUG(tasklogger,"TimedWait TimedOut");
          //*(lTM->fMainLog) << "Thread Id " << pthread_self() << "TimedCondWait Expired \n";
          cout << "Thread Id " << pthread_self() << "sem_timedwait Expired \n";
        }else{
          //cout << "Thread Id " << pthread_self() << "sem_timedwait Posted\n";
          LOGPZ_DEBUG(tasklogger,"TimedWait Signaled ");
        }
      } else {
#ifdef LOGPZ
        {
          std::stringstream sout;
          sout << __PRETTY_FUNCTION__ << " going to sleep";
          //LOGPZ_DEBUG(tasklogger,sout.str().c_str());
        }
#endif
	sem_wait(&lTM->fServiceSemaphore);
      }
    }
  }
#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << __PRETTY_FUNCTION__ << " falling through " << " keep going = " << lTM->fKeepGoing << " number of executing tasks " << lTM->fExecuting.size();
    LOGPZ_DEBUG(tasklogger,sout.str().c_str());
  }
#endif
  
  CM->SendMessages ();
  return NULL;
}

void *
OOPTaskManager::ExecuteMTBlocking (void *data)
{
#ifdef LOGTIME
  tlog <<
    "time\tsubmitted\twaiting\texecutable\texecuting\tfinished\tdaemon\n";
#endif
  OOPTaskManager *lTM = static_cast < OOPTaskManager * >(data);

  OOPTMLock lock;//pthread_mutex_lock (&lTM->fServiceMutex);
  DM->SubmitAllObjects ();

  sem_init(&lTM->fServiceSemaphore, 0,0);
  ((OOPMPICommManager *)CM)->ReceiveMessagesBlocking ();
  // this method needs to grab the lock
  lTM->TransferSubmittedTasks ();
  list < OOPTaskControl * >::iterator i;

  SszQueues curSz;
  curSz.set (lTM->fTaskList.size (), lTM->fExecutable.size (),
	     lTM->fExecuting.size ());

#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << "Entering task list loop";
    LOGPZ_DEBUG (logger, sout.str ());
  }
#endif
  lTM->fKeepGoing = true;
  lTM->ExecuteDaemons ();
  while (lTM->fKeepGoing) {
    lTM->ExecuteDaemons ();
    while (lTM->fExecutable.size ()
	   && (int) lTM->fExecuting.size () < lTM->fNumberOfThreads) {
      i = lTM->fExecutable.begin ();
      OOPTaskControl *tc = (*i);
      lTM->fExecutable.erase (i);
      lTM->fExecuting.push_back (tc);
#ifdef LOGPZ
      {
	stringstream sout;
	sout << "Entering taskcontrol execute for task " << tc->
	  Id () << " classid " << tc->ClassId ();
	LOGPZ_DEBUG (tasklogger, sout.str ());
      }
#endif
      tc->Execute ();
      lTM->TransferExecutingTasks ();
      DM->SubmitAllObjects ();
    }
    lTM->TransferExecutingTasks ();
    lTM->TransferFinishedTasks ();
    lTM->TransferSubmittedTasks ();
    lTM->ExecuteDaemons ();

#ifdef LOGTIME
    if (!curSz.
	IsEqual (lTM->fTaskList.size (), lTM->fExecutable.size (),
		 lTM->fExecuting.size ())) {
      timeval curtime;
      gettimeofday (&curtime, 0);

      tlog << curtime.tv_sec << ":" << curtime.tv_usec << "\t"
	<< lTM->fSubmittedList.size () << "\t"
	<< lTM->fTaskList.size () << "\t"
	<< lTM->fExecutable.size () << "\t"
	<< lTM->fExecuting.size () << "\t"
	<< lTM->fFinished.size () << "\t"
	<< lTM->fDaemon.size () << std::endl;
      curSz.set (lTM->fTaskList.size (), lTM->fExecutable.size (),
		 lTM->fExecuting.size ());
    }
#endif


    if (!lTM->HasWorkTodo () && lTM->fKeepGoing) {
      if (CM->NumProcessors () > 1) {
	timeval now;
	gettimeofday (&now, 0);
	now.tv_usec += 1;
	now.tv_sec += now.tv_usec / 1000000;
	now.tv_usec %= 1000000;
	timespec next;
	next.tv_sec = now.tv_sec;
	next.tv_nsec = now.tv_usec * 1000;
#ifdef LOGPZ
      {
        std::stringstream sout;
        sout << __PRETTY_FUNCTION__ << " going to sleep";
//        LOGPZ_DEBUG(tasklogger,sout.str().c_str());
      }
#endif
	//pthread_cond_timedwait (&lTM->fExecuteCondition,
	//			&lTM->fSubmittedMutex, &next);
        lock.Unlock();//pthread_mutex_unlock (&lTM->fServiceMutex);
	sem_wait(&lTM->fServiceSemaphore);//, &next);
        lock.Lock();//pthread_mutex_lock (&lTM->fServiceMutex);
      } else {
#ifdef LOGPZ
      {
        std::stringstream sout;
        sout << __PRETTY_FUNCTION__ << " going to sleep";
//        LOGPZ_DEBUG(tasklogger,sout.str().c_str());
      }
#endif
	//pthread_cond_wait (&lTM->fExecuteCondition, &lTM->fSubmittedMutex);
        lock.Unlock();//pthread_mutex_unlock (&lTM->fServiceMutex);
	sem_wait(&lTM->fServiceSemaphore);
        lock.Lock();//pthread_mutex_lock (&lTM->fServiceMutex);
      }
    }
  }
  
  
  //pthread_mutex_unlock (&lTM->fServiceMutex);
  //Unlock not necessary since lock local object leaves context and calls its destructor.

  return NULL;
}


#endif
OOPTaskManager::OOPTaskManager (int proc):fNumberOfThreads (10)
{
  fProc = proc;
  fLastCreated = 0;		//NUMOBJECTS * fProc;
  fExecuteThread = 0;
  stringstream filename;
  filename << "TaskMainLog" << fProc << ".dat";
  fMainLog = 0;
}

OOPTaskManager::~OOPTaskManager ()
{
#ifdef LOGPZ
	stringstream sout;
	sout << "submitted.size : " << fSubmittedList.size()  << 
	" executable.size " << fExecutable.size() << 
	" finished.size " << fFinished.size() << 
	" tasklist.size " << fTaskList.size();
	LOGPZ_DEBUG (tasklogger, sout.str ());
#endif
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
/*

void
OOPTaskManager::NotifyAccessGranted (const OOPAccessTag & depend)
{
#warning "Apenas colocar numa pilha"
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_ERROR (logger, sout.str ());
#endif
  }
  list < OOPTaskControl * >::iterator i;
  bool found = false;
  for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
    OOPTaskControl *tc = (*i);
    if (tc->Task ()->Id () == TaskId) {
      found = true;
      tc->Task()->GrantAccess (depend);
      {
#ifdef OOP_MPE
        stringstream auxsout;
        auxsout << "T:" << TaskId << " " << depend;
        OOPSoloEvent solo("grantaccess", auxsout.str());
#endif
#ifdef LOGPZ
	stringstream sout;
	sout << "Access Granted to taskId " << TaskId << " classid " << tc->
	  ClassId ();
	sout << " on data " << depend.Id ();
	LOGPZ_DEBUG (tasklogger, sout.str ());
#endif
      }
      if (tc->Task()->CanExecute ()) {
#ifdef LOGPZ
	stringstream sout;
	sout << "Task " << TaskId << " classid " << tc->
	  ClassId ();
	sout << " can execute";
	LOGPZ_DEBUG (tasklogger, sout.str ());
#endif
	TransfertoExecutable (tc->Task ()->Id ());
	{
#ifdef LOGPZ
	  stringstream sout;
	  sout << "OOPTaskManager task is executable " << TaskId <<
	    " classid " << tc->ClassId ();
	  LOGPZ_DEBUG (tasklogger, sout.str ());
#endif
	}
      }
      break;
    }
  }
  if (!found) {
#ifdef LOGPZ
    stringstream sout;
    sout << "Task not found on current TM: File:" << __FILE__ <<
      " Line:" << __LINE__ << endl;
    sout << " Task ";
    TaskId.Print (sout);
    LOGPZ_ERROR (tasklogger, sout.str ());
#endif
  }
}
*/
void
OOPTaskManager::SubmitDaemon (OOPDaemonTask * task)
{
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
  if (task->GetProcID () != this->fProc) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Sending a daemon task to proc " << task->
      GetProcID () << " classid " << task->ClassId ();
#endif
    CM->SendTask (task);
#ifdef LOGPZ
    LOGPZ_DEBUG (logger, sout.str ());
#endif

  } else {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Submitting a daemon task " << " classid "
      << task->ClassId ();
    LOGPZ_DEBUG (logger, sout.str ());
#endif
    fDaemon.push_back (task);
  }
}

OOPObjectId OOPTaskManager::Submit (OOPTask * task)
{
  cout << "Calling Submit on OOPTaskManager ";
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Calling Submit on OOPTaskManager ";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }

  OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(task);
  OOPObjectId id;
  if (dmt) {
#ifdef LOGPZ
    stringstream sout;
    sout << "Task Submitted is a daemon";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  } else {
    id = task->Id ();
    if (id.IsZeroOOP ())
      id = GenerateId ();
    task->SetTaskId (id);
  }
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Task with id " << task->
      Id () << " submitted for processor" << task->
      GetProcID () << " classid " << task->ClassId ();
    LOGPZ_DEBUG (tasklogger, sout.str ());
#endif
  }
#ifdef DEBUG
  OOPWaitTask *wait = dynamic_cast < OOPWaitTask * >(task);
  if (!wait && !dmt && !CM->GetProcID ()) {
    std::ostringstream FileName, FileName2, command, subdir1, subdir2,
      subdir3;
    subdir1 << "taskman" << CM->GetProcID ();
    subdir2 << "taskman" << CM->GetProcID () << "/orig";
    subdir3 << "taskman" << CM->GetProcID () << "/copy";
    mkdir (subdir1.str ().c_str (),
	   S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
    mkdir (subdir2.str ().c_str (),
	   S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
    mkdir (subdir3.str ().c_str (),
	   S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
    FileName << subdir2.str () << "/" << task->ClassId () << ".sav";
    FileName2 << subdir3.str () << "/" << task->ClassId () << ".sav";
    {
      TPZFileStream PZFS;
      PZFS.OpenWrite (FileName.str ());
      task->Write (PZFS, 1);
    }
    {
      TPZFileStream PZFS;
      PZFS.OpenRead (FileName.str ());
      OOPTask *test = (OOPTask *) TPZSaveable::Restore (PZFS, 0);
      TPZFileStream PZFS2;
      PZFS2.OpenWrite (FileName2.str ());
      test->Write (PZFS2, 1);
      delete test;
    }
    command << "diff --brief " << FileName.str () << " " << FileName2.
      str () << endl;
    FILE *pipe = popen (command.str ().c_str (), "r");
#ifdef DEBUGALL
    cout << "Command executed " << command.str () << endl;
#endif
    char *compare = new char[256];
    compare[0] = '\0';
    char **compptr = &compare;
    size_t size = 256;
    getline (compptr, &size, pipe);
    pclose (pipe);
    if (strlen (compare)) {
      cout << __PRETTY_FUNCTION__ <<
	" The writing process produced an error for class " << task->
	ClassId () << compare << endl;
    }
    delete [] compare;
  }
#endif
  // I dont need to lock if I am the service thread
  // (in that case I already have the lock)
//  if (!pthread_equal (fExecuteThread, pthread_self ())) {
    //LOGPZ_DEBUG(logger,"Lock within Submit")
#warning "Call to discontinued fSubmittedMutex, Verify accesses on the Mutexed lists"
  //pthread_mutex_lock (&fSubmittedMutex);
  //}
  fSubmittedList.push_back (task);
  //if (!pthread_equal (fExecuteThread, pthread_self ())) {
    //LOGPZ_DEBUG(logger,"Signal within Submit")
    //pthread_cond_signal (&fExecuteCondition);
  sem_post(&fServiceSemaphore);
    //LOGPZ_DEBUG(logger,"Unlock within Submit")
  //pthread_mutex_unlock (&fSubmittedMutex);
  //}
  return id;
}
/*
OOPObjectId
OOPTaskManager::SubmitOriginal (OOPTask * task)
{
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Calling Submit on OOPTaskManager ";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }

  OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(task);
  OOPObjectId id;
  if (dmt) {
#ifdef LOGPZ
    stringstream sout;
    sout << "Task Submitted is a daemon";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  } else {
    id = task->Id ();
    if (id.IsZeroOOP ())
      id = GenerateId ();
    task->SetTaskId (id);
  }
  {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Task with id " << task->
      Id () << " submitted for processor" << task->
      GetProcID () << " classid " << task->ClassId ();
    LOGPZ_DEBUG (tasklogger, sout.str ());
#endif
  }
#ifdef DEBUG
  OOPWaitTask *wait = dynamic_cast < OOPWaitTask * >(task);
  if (!wait && !dmt && !CM->GetProcID ()) {
    std::ostringstream FileName, FileName2, command, subdir1, subdir2,
      subdir3;
    subdir1 << "taskman" << CM->GetProcID ();
    subdir2 << "taskman" << CM->GetProcID () << "/orig";
    subdir3 << "taskman" << CM->GetProcID () << "/copy";
    mkdir (subdir1.str ().c_str (),
	   S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
    mkdir (subdir2.str ().c_str (),
	   S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
    mkdir (subdir3.str ().c_str (),
	   S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
    FileName << subdir2.str () << "/" << task->ClassId () << ".sav";
    FileName2 << subdir3.str () << "/" << task->ClassId () << ".sav";
    {
      TPZFileStream PZFS;
      PZFS.OpenWrite (FileName.str ());
      task->Write (PZFS, 1);
    }
    {
      TPZFileStream PZFS;
      PZFS.OpenRead (FileName.str ());
      OOPTask *test = (OOPTask *) TPZSaveable::Restore (PZFS, 0);
      TPZFileStream PZFS2;
      PZFS2.OpenWrite (FileName2.str ());
      test->Write (PZFS2, 1);
      delete test;
    }
    command << "diff --brief " << FileName.str () << " " << FileName2.
      str () << endl;
    FILE *pipe = popen (command.str ().c_str (), "r");
#ifdef DEBUGALL
    cout << "Command executed " << command.str () << endl;
#endif
    char *compare = new char[256];
    compare[0] = '\0';
    char **compptr = &compare;
    size_t size = 256;
    getline (compptr, &size, pipe);
    pclose (pipe);
    if (strlen (compare)) {
      cout << __PRETTY_FUNCTION__ <<
	" The writing process produced an error for class " << task->
	ClassId () << compare << endl;
    }
    delete [] compare;
  }
#endif
  // I dont need to lock if I am the service thread
  // (in that case I already have the lock)
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
    //LOGPZ_DEBUG(logger,"Lock within Submit")
//    pthread_mutex_lock (&fSubmittedMutex);
  }
  fSubmittedList.push_back (task);
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
    //LOGPZ_DEBUG(logger,"Signal within Submit")
    //pthread_cond_signal (&fExecuteCondition);
    //sem_post(&fServiceSemaphore);
    //LOGPZ_DEBUG(logger,"Unlock within Submit")
//    pthread_mutex_unlock (&fSubmittedMutex);
  }
  return id;
}
*/
int
OOPTaskManager::NumberOfTasks ()
{
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
  int numtasks = fExecutable.size () + fFinished.size () +
    fSubmittedList.size () + fTaskList.size () + fDaemon.size ();
  return numtasks;
}

bool
OOPTaskManager::HasWorkTodo ()
{
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
    return false;
  }
//   cout << __PRETTY_FUNCTION__ <<  "\n\t" <<fExecutable.size()
//       << "\t" << fFinished.size () << "\t" << fSubmittedList.size () << "\t" << fDaemon.size() << endl;
  int numtasks = fFinished.size () + fSubmittedList.size () + fDaemon.size ();
  if ((int)fExecuting.size () != fNumberOfThreads && fExecutable.size ())
  {
    numtasks++;
  }
//   cout << "numtasks = " << numtasks << " fExecuting.size " << fExecuting.size() <<
//   " fExecutable.size() " << fExecutable.size() << endl;
  return numtasks != 0;
}



void OOPTaskManager::CancelTask (OOPAccessTag & tag)
{
  std::pair<TMMessageType, OOPAccessTag> item(ETMCancelTask, tag);
  OOPTMLock lock;
  fMessages.push_back(item);
}
/**
  // I assume that I already have the lock!!
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_ERROR (logger, sout.str ());
#endif
  }

  list < OOPTaskControl * >::iterator i;	// , iprev, atual;
  for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
    OOPTaskControl *tc = *i;
    if (tc->Task ()->Id () == taskid) {
#ifdef LOGPZ
      stringstream sout;
      sout << "Task erased ";
      sout << "Task ID " << tc->Id () << " classid " << tc->ClassId ();
      LOGPZ_DEBUG (logger, sout.str ());
#endif
      tc->Task()->ClearDependentData();
      delete tc;
      fTaskList.erase (i);
      return 1;
    }
  }
  return 0;

*/
void
OOPTaskManager::ExecuteDaemons ()
{

  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread fExecuteThread "
      << fExecuteThread << " thread_self " << pthread_self ();
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
  list < OOPDaemonTask * >::iterator i;
  while (fDaemon.size ()) {
    i = fDaemon.begin ();
    if ((*i)->GetProcID () != DM->GetProcID ()) {
      CM->SendTask ((*i));
    } else { 
      (*i)->Execute ();
      delete (*i);
    }
    fDaemon.erase (i);
  }
}
void
OOPTaskManager::Execute ()
{
//      pthread_t execute_thread;
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Creating service thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
#ifdef BLOCKING
   if (pthread_create (&fExecuteThread, NULL, ExecuteMTBlocking, this)) {
#else
   if (pthread_create (&fExecuteThread, NULL, ExecuteMT, this)) {
#endif
#ifdef LOGPZ
    stringstream sout;
    sout << "Fail to create service thread\n";
    sout << "Going out";
    LOGPZ_ERROR (logger, sout.str ());
#endif
  }
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Created succesfuly";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
}

void
OOPTaskManager::Wait ()
{

  pthread_join (fExecuteThread, NULL);
  MPI_Barrier(MPI_COMM_WORLD);
#ifdef BLOCKING
  cout << " Unlocking BlockingReceive Thread hopefully going down in a few seconds " << endl;
  ((OOPMPICommManager *)CM)->UnlockReceiveBlocking(); 
#endif
}

void
OOPTaskManager::SetKeepGoing (bool go)
{
  fKeepGoing = go;
}

OOPObjectId
OOPTaskManager::GenerateId ()
{
  fLastCreated++;
  OOPObjectId tmp (fProc, fLastCreated);
  return tmp;
}

OOPTask *
OOPTaskManager::FindTask (OOPObjectId taskid)
{				// find the task with the given id
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
void
OOPTaskManager::Print (std::ostream & out)
{
  out << "Task Manager data structure \t" << endl;
  out << "TM Processor " << fProc << endl;
  out << "Id of Last Created Task \t" << fLastCreated << endl;

  out << "Queued Daemon tasks ---------\t" << endl;
  out << "Queued Time Consuming tasks ---------" << endl;
  out << "Number of Time Consuming tasks \t" << fTaskList.size () << endl;
  list < OOPTaskControl * >::iterator i;
  for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
    (*i)->Task ()->Print (out);
  }
}

void OOPTaskManager::TransferSubmittedTasks ()
{

  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
  //cout << __PRETTY_FUNCTION__ << " Locking on fSubmittedMutex \n";
  //pthread_mutex_lock(&fSubmittedMutex);
#warning "pthread_mutex_lock(&fSubmittedMutex);"
  
  //cout << __PRETTY_FUNCTION__ << " Locking on fSubmittedMutex Succeded \n";
  {
    OOPTMLock lock;
    list < OOPTask * >::iterator sub;
    DM->SubmitAllObjects ();
  
    int listsize = fSubmittedList.size ();
    sub = fSubmittedList.begin ();
    OOPTask *aux = 0;
    if (listsize) {
      aux = (*sub);
      fSubmittedList.erase (sub);
    }
  
    while (aux)
    {
      //aux could be a DaemonTask
      OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(aux);
      if (aux->GetProcID () != fProc) {
  #ifdef LOGPZ
        stringstream sout;
        sout << __PRETTY_FUNCTION__ << "Transferring task " << aux->
          Id () << " from " << fProc << " to proc " << aux->GetProcID ();
        LOGPZ_DEBUG (tasklogger, sout.str ())
  #endif
          CM->SendTask (aux);
      } else if (dmt) { //Checks if dmt is valid. aux was a DaemonTask
        SubmitDaemon (dmt);
      } else {//Ordinary task to be executed in this processor
  #ifdef LOGPZ
        stringstream sout;
        sout << __PRETTY_FUNCTION__ << "Creating the task control ojbect for task " << aux->Id () ;
        LOGPZ_DEBUG (tasklogger, sout.str ())
  #endif
  
        OOPTaskControl *tc = new OOPTaskControl (aux);
        fTaskList.push_back (tc);
        tc->Task()->SubmitDependencyList();
      }
      DM->SubmitAllObjects ();
      listsize = fSubmittedList.size ();
      sub = fSubmittedList.begin ();
      aux = 0;
      if (listsize) {
        aux = (*sub);
        fSubmittedList.erase (sub);
      }
    }
    WakeServiceThread();
  }
  //cout << __PRETTY_FUNCTION__ << " UnLocking on fSubmittedMutex \n";
#warning "pthread_mutex_unlock(&fSubmittedMutex);"
}
void OOPTaskManager::TransferFinishedTasks ()
{
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
  list < OOPTaskControl * >::iterator sub;
#warning "Not sure if this is necessary, Locking anyway"
  int listsize = fFinished.size ();
  if (!listsize)
    return;
  DM->SubmitAllObjects ();
  sub = fFinished.begin ();
  OOPTaskControl *auxtc = 0;
  if (listsize) {
    auxtc = (*sub);
    fFinished.erase (sub);
  }
  while (auxtc) {
#ifdef LOGPZ
    {
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << " task " << auxtc->
	Id () << " classid " << auxtc->ClassId () << " finished";
      LOGPZ_DEBUG (tasklogger, sout.str ());
    }
#endif
    if (auxtc->Task () && auxtc->Task ()->IsRecurrent ()
	&& auxtc->Task ()->GetProcID () != fProc) {
#ifdef LOGPZ
      {
	stringstream sout;
	sout << __PRETTY_FUNCTION__ << " task " << auxtc->
	  Id () << " classid " << auxtc->
	  ClassId () << " transferred from " << fProc << " to " << auxtc->
	  Task ()->GetProcID ();
	LOGPZ_DEBUG (tasklogger, sout.str ());
      }
#endif
      CM->SendTask (auxtc->Task ());
      auxtc->ZeroTask ();
      delete auxtc;
    } else if (auxtc->Task () && auxtc->Task ()->IsRecurrent ()) {
#ifdef LOGPZ
      {
	stringstream sout;
	sout << __PRETTY_FUNCTION__ << " task " << auxtc->
	  Id () << " classid " << auxtc->ClassId () << " resubmitted";
	LOGPZ_DEBUG (tasklogger, sout.str ());
      }
#endif
      auxtc->Task ()->ClearDependentData();
      fTaskList.push_back (auxtc);
      
      auxtc->Task()->SubmitDependencyList ();
    } else {
      delete auxtc;
    }
    listsize = fFinished.size ();
    auxtc = 0;
    sub = fFinished.begin ();
    if (listsize) {
      auxtc = (*sub);
      fFinished.erase (sub);
    }
  }
}

void
OOPTaskManager::TransfertoExecutable (const OOPObjectId & taskid)
{
  if (!pthread_equal (fExecuteThread, pthread_self ())) {
#ifdef LOGPZ
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " called by foreign thread";
    LOGPZ_DEBUG (logger, sout.str ());
#endif
  }
  list < OOPTaskControl * >::iterator i;
  {
    OOPTMLock lock;
    for (i = fTaskList.begin (); i != fTaskList.end (); i++) {
      OOPTaskControl *tc = (*i);
      if (tc->Task ()->Id () == taskid) {
        OOPDaemonTask *dmt = dynamic_cast < OOPDaemonTask * >(tc->Task ());
        if (dmt) {
  #ifdef LOGPZ
          LOGPZ_ERROR (logger,
                       "TM::TransfertoExecutable inconsistent datastructure\nThere is daemontask in the fTaskList\n");
  #endif
          SubmitDaemon (dmt);
          tc->ZeroTask ();
          delete tc;
        } else {
          fExecutable.push_back (tc);
        }
        fTaskList.erase (i);
        break;
      }
    }
    WakeServiceThread();
  }
}

void
OOPTaskManager::PrintTaskQueues (char *msg, std::ostream & out)
{
  out << msg << endl;
  out << "Printing TaskManager Queues on TM:" << fProc << endl;
  out << "Print fSubmittedList\n";
  out << "Number of tasks :" << fSubmittedList.size () << endl;
  list < OOPTask * >::iterator i;
  for (i = fSubmittedList.begin (); i != fSubmittedList.end (); i++)
    out << (*i)->Id () << endl;
  out << "Print fTaskList\n";
  out << "Number of tasks :" << fTaskList.size () << endl;
  list < OOPTaskControl * >::iterator j;
  for (j = fTaskList.begin (); j != fTaskList.end (); j++)
    out << (*j)->Id () << endl;
  out << "Print fExecutable\n";
  out << "Number of tasks :" << fExecutable.size () << endl;
  for (j = fExecutable.begin (); j != fExecutable.end (); j++)
    out << (*j)->Id () << endl;
  out << "Print fFinished\n";
  out << "Number of tasks :" << fFinished.size () << endl;
  for (j = fFinished.begin (); j != fFinished.end (); j++)
    out << (*j)->Id () << endl;

}



OOPTerminationTask::~OOPTerminationTask ()
{
}

OOPTerminationTask::OOPTerminationTask (int ProcId):OOPTask (ProcId)
{
}
OOPTerminationTask::OOPTerminationTask (const OOPTerminationTask & term):
OOPTask (term)
{
}

OOPMReturnType
OOPTerminationTask::Execute ()
{
  //LOGPZ_DEBUG (logger, "Antes do Lock\n");
  {
  OOPTMLock lock;
  TM->SetKeepGoing (false);
  }
  //LOGPZ_DEBUG (logger, "Depois do Lock\n");
  sleep(1);
  return ESuccess;
}

void
OOPTerminationTask::Write (TPZStream & buf, int withclassid)
{
  OOPTask::Write (buf, withclassid);

}

void
OOPTerminationTask::Read (TPZStream & buf, void *context)
{
  OOPTask::Read (buf, context);

}

long int
OOPTerminationTask::ExecTime ()
{
  return -1;
}

TPZSaveable *
OOPTerminationTask::Restore (TPZStream & buf, void *context)
{
  OOPTerminationTask *v = new OOPTerminationTask (0);
  v->Read (buf);
  return v;
}


extern OOPTaskManager *TM;
