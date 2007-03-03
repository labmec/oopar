#include "ooptaskcontrol.h"
#include "ooptask.h"
#include "ooperror.h"
#include "ooptaskmanager.h"
#include "ooptmlock.h"
#include <sstream>
#include <pzlog.h>
#ifdef LOGPZ
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));
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
#ifdef LOGPZ  
    stringstream sout;
    sout << "Fail to create service thread -- ";
    sout << "Going out";
    LOGPZ_ERROR(logger,sout.str());
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
#ifdef LOGPZ
      stringstream sout;
      sout << "Submitting new Versions from Task " << fTaskId 
      << " On ObjectId " << TaskDepend().Dep(i).Id()
      << " : Old Version " << TaskDepend().Dep(i).Version()
      << " New Version " << fTask->GetDependencyData().Version(i);
      LOGPZ_DEBUG(logger, sout.str());
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
#ifdef OOP_MPE
  stringstream sout;
  sout << "T:" << tc->fTask->Id().GetId()
    << ":" << tc->fTask->Id().GetProcId() << ":C:" << tc->fTask->ClassId() << ":D:";
  tc->Task()->PrintDependency(sout);
  OOPStateEvent evt("taskexec",sout.str());
#endif
#ifdef LOGPZ  
  {
    stringstream sout;
    sout << "Task " << tc->fTask->Id() << " started";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif  
  tc->fExecStarted = 1;
  tc->fTask->Execute();
  OOPObjectId id = tc->fTask->Id();
  int lClassId = tc->fTask->ClassId();
  //Guardar versoes dos dados
  //Associando TaskDependList com DataDependList
  //Objetos com WriteAccess sao atualizados com as novas versoes
  //VerifyAccessRequest feito dentro do submit do metadata. 
#ifdef LOGPZ
  {
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << "Task finished, calling updateversions";
  LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  tc->UpdateVersions();
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Task " << id << " CId:" << lClassId << " finished before lock";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif

  OOPTMLock lock;
  tc->fExecFinished =1;
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Task " << id << " CId:" << lClassId << " finished";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  TM->WakeUpCall();
  return 0;
}

void OOPTaskControl::Join()
{
  if(fExecutor == pthread_self())
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << __FUNCTION__ << " called by the taskcontrol object itself";
    LOGPZ_ERROR(logger,sout.str());
#endif    
    return;
  }
  void *execptr;
  void **executorresultptr = &execptr;
  int result = pthread_join(fExecutor,executorresultptr);
  if(result)
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << __FUNCTION__ << __LINE__ << " join operation failed with result " << result;
    LOGPZ_ERROR(logger,sout.str());
#endif
  }
}

