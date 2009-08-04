#include "ooptaskcontrol.h"
#include "ooptask.h"
#include "ooptaskmanager.h"
#include "ooplock.h"
#include "oopwaittask.h"
#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
extern OOPGenericLockService gMutex;
#endif

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));
static LoggerPtr tasklogger (Logger::getLogger ("OOPar.OOPTaskManager.OOPTask"));
#endif

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif

#include "oopcommmanager.h"
#include "oopmetadata.h"

OOPTaskControl::OOPTaskControl (OOPTask * task):fTask (task)
{
  fExecStarted = 0;
  fExecFinished = 0;
  if (task) {
    fTaskId = task->Id();
    fClassId = task->ClassId();
  }
}

OOPTaskControl::~OOPTaskControl ()
{
  if (fTask) delete  fTask;
}

void OOPTaskControl::Execute()
{
  fExecFinished =0;
  if(pthread_create(&fExecutor, NULL, ThreadExec, this)){
#ifdef LOG4CXX
    stringstream sout;
    sout << "Fail to create service thread -- ";
    sout << "Going out";
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
  }
}

void OOPTaskControl::UpdateVersions()
{
  fTask->ReleaseDepObjPtr();
}
/*
  int i = 0;
  int size = TaskDepend().NElements();
  for(i=0;i<size;i++)
  {
    if(TaskDepend().Dep(i).State() == EWriteAccess)
    {
#ifdef LOG4CXX
      stringstream sout;
      sout << "Submitting new Versions from Task " << fTaskId
      << " On ObjectId " << TaskDepend().Dep(i).Id()
      << " : Old Version " << TaskDepend().Dep(i).Version()
      << " New Version " << fTask->GetDependencyData().Version(i);
      LOGPZ_DEBUG(logger, sout.str().c_str());
#endif
      OOPDataVersion nextver = fTask->GetDependencyData().Version(i);
      TPZAutoPointer<TPZSaveable> objptr = fTask->GetDependencyData().ObjPtr(i);
      TaskDepend().Dep(i).ObjPtr()->SubmitVersion(nextver, objptr);
    }
  }

*/
void OOPTaskControl::Print(std::ostream & out)
{
  out << "TaskId:" << fTask->Id().GetId()
    << ":Proc:" << fTask->Id().GetProcId() << ":ClassId:" << fTask->ClassId() << ":Dependency:";
  Task()->PrintDependency(out);
}
void *OOPTaskControl::ThreadExec(void *threadobj)
{
  OOPTaskControl *tc = (OOPTaskControl *) threadobj;
#ifdef STEP_MUTEX
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "waiting for lock");
#endif
		OOPLock<OOPGenericLockService> lock(&gMutex);
#endif

#ifdef OOP_MPE
  stringstream sout;
  sout << "T:" << tc->fTask->Id().GetId()
    << ":" << tc->fTask->Id().GetProcId() << ":C:" << tc->fTask->ClassId() << ":D:";
  tc->Task()->PrintDependency(sout);
  OOPStateEvent evt("taskexec",sout.str().c_str());
#endif
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << "Task T:" << tc->fTask->Id() << " Started";
    LOGPZ_DEBUG(tasklogger,sout.str().c_str());
  }
#endif
  tc->fExecStarted = 1;
  TPZAutoPointer<OOPTaskManager> TM = tc->fTask->TM();
#ifdef STEP_MUTEX
  // a wait task depends on other tasks to execute
  // it cannot run in a step lock fashion
  if(dynamic_cast<OOPWaitTask*> (tc->fTask))
  {
	  lock.Unlock();
  }
#endif
  tc->fTask->Execute();
#ifdef STEP_MUTEX
  if(dynamic_cast<OOPWaitTask *>(tc->fTask))
  {
	  lock.Lock();
  }
#endif
  OOPObjectId id = tc->fTask->Id();
  int lClassId = tc->fTask->ClassId();
#ifdef LOG4CXX
  {
  stringstream sout;
  sout << "Task T:" << tc->fTask->Id() << " ClassId CId:" << lClassId << " Finished";
  LOGPZ_DEBUG(tasklogger,sout.str().c_str());
  }
#endif
	tc->fTask->IncrementWriteDependentData();
  tc->UpdateVersions();

  {
    OOPLock<OOPTaskManager> lock(TM);
    tc->fExecFinished =1;
  }

  TM->WakeUpCall();
  return 0;
}

void OOPTaskControl::Join()
{
  if(fExecutor == pthread_self())
  {
#ifdef LOG4CXX
    stringstream sout;
    sout << __FUNCTION__ << " called by the taskcontrol object itself";
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    return;
  }
  void *execptr;
  void **executorresultptr = &execptr;
  int result = pthread_join(fExecutor,executorresultptr);
  if(result)
  {
#ifdef LOG4CXX
    stringstream sout;
    sout << __FUNCTION__ << __LINE__ << " join operation failed with result " << result;
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
  }
}

