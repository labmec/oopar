#include "ooptaskcontrol.h"


#include "ooptask.h"
#include "ooperror.h"
#include "ooptaskmanager.h"

#include <sstream>
#include <pzlog.h>
#ifdef LOGPZ
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));
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
    fDataDepend = task->GetDependencyRequests();
#ifdef LOGPZ  
    stringstream sout;
    sout << "The data dependency of the task control for task id " << task->Id();
    fDataDepend.Print(sout);
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
}

OOPTaskControl::~OOPTaskControl ()
{
  if (fTask) delete  fTask;
}

void OOPTaskControl::Execute()
{
  fExecFinished =0;
  //  static int numthreads = 0;
  //  cout << __FUNCTION__ << " creating trhead number " << numthreads++ << " max threads " << PTHREAD_THREADS_MAX << endl;
  if(pthread_create(&fExecutor, NULL, ThreadExec, this)){
#ifdef LOGPZ  
    stringstream sout;
    sout << "Fail to create service thread -- ";
    sout << "Going out";
    LOGPZ_ERROR(logger,sout.str());
#endif    
  }
}

void OOPTaskControl::UpdateVersions(){
  int i = 0;
  int size = TaskDepend().NElements();
  for(i=0;i<size;i++)
  {
    if(TaskDepend().Dep(i).State() == EWriteAccess &&
      !(fTask->GetDependencyData().Version(i) == TaskDepend().Dep(i).Version()))
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
#ifdef LOGPZ
  stringstream sout;
  sout << "Leaving UpdateVersion";
  LOGPZ_DEBUG(logger, sout.str());
#endif      
}
void OOPTaskControl::Print(std::ostream & out)
{
  out << "TaskId:" << fTask->Id().GetId()
    << ":Proc:" << fTask->Id().GetProcId() << ":ClassId:" << fTask->ClassId() << ":Dependency:";
  fDataDepend.Print(out);
}
void *OOPTaskControl::ThreadExec(void *threadobj)
{
  OOPTaskControl *tc = (OOPTaskControl *) threadobj;
#ifdef OOP_MPE
  stringstream sout;
  sout << "T:" << tc->fTask->Id().GetId()
    << ":" << tc->fTask->Id().GetProcId() << ":C:" << tc->fTask->ClassId() << ":D:";
  tc->fDataDepend.ShortPrint(sout);
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
  // the task finished executing!!!!
  //cout << __PRETTY_FUNCTION__ << " before lock for task " << tc->fTask->Id() << endl;
  OOPObjectId id = tc->fTask->Id();
  //Guardar versoes dos dados
  //Associando TaskDependList com DataDependList
  //Objetos com WriteAccess sao atualizados com as novas versoes
  //VerifyAccessRequest feito dentro do submit do metadata. 
#ifdef LOGPZ
  {
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << "Before lock";
  LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  {
  TMLock lock;
  tc->UpdateVersions();
  }
#ifdef LOGPZ
  {
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << "After lock";
  LOGPZ_DEBUG(logger,sout.str());
  }
#endif
/*  if (!tc->fTask->IsRecurrent())
  {
    delete tc->fTask;
    tc->fTask=0;
  }*/
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Task " << id << " CId:" << lClassId << " finished before lock";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif

  TMLock lock;
  //cout << __PRETTY_FUNCTION__ << " after lock for task" << tc->fTask->Id() << endl;
  //tc->fTask->SetExecuting(false);
  tc->fExecFinished =1;
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Task " << id << " CId:" << lClassId << " finished";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  TM->Signal(lock);
  return 0;
}

void OOPTaskControl::Join()
{
  if(fExecutor == pthread_self())
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << __FUNCTION__ << " called by the taskcontrol object itself";
    LOGPZ_DEBUG(logger,sout.str());
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
    LOGPZ_DEBUG(logger,sout.str());
#endif
  }
}

