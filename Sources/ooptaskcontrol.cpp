
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

#ifdef OOP_MPI
#include "mpi.h"
#endif
#ifdef OOP_MPE
#include "mpe.h"
#endif

OOPTaskControl::OOPTaskControl (OOPTask * task):fTask (task)
{
  m_MPEEvtStart = MPE_Log_get_event_number();
  m_MPEEvtEnd = MPE_Log_get_event_number();
  fExecStarted = 0;
  fExecFinished = 0;
  if (task) {
    fDepend = task->GetDependencyList ();
    fDepend.ClearPointers ();
    fTaskId = task->Id();
    fClassId = task->ClassId();
    fDataDepend = task->Depend();
    char title [256];
    sprintf (title, "TaskId %d:%d : Class %d",fTaskId.GetId(), fTaskId.GetProcId(), fClassId);
    MPE_Describe_state(m_MPEEvtStart, m_MPEEvtEnd, title, "blue");

  }
}

OOPTaskControl::~OOPTaskControl ()
{
  if (fTask) delete  fTask;
}

void OOPTaskControl::Execute()
{
//  static int numthreads = 0;
//  cout << __FUNCTION__ << " creating trhead number " << numthreads++ << " max threads " << PTHREAD_THREADS_MAX << endl;
  if(pthread_create(&fExecutor, NULL, ThreadExec, this)){
#ifdef LOGPZ  
    stringstream sout;
    sout << "Fail to create service thread -- ";
    sout << "Going out";
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
}

void *OOPTaskControl::ThreadExec(void *threadobj)
{
#ifdef LOGPZ
  stringstream sout;
#endif  
  OOPTaskControl *tc = (OOPTaskControl *) threadobj;
  MPE_Log_event(tc->m_MPEEvtStart, 0, NULL);
#ifdef LOGPZ  
  {
    sout << "Task " << tc->fTask->Id() << " started";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif  
  tc->fExecStarted = 1;
  tc->fTask->Execute();
  // the task finished executing!!!!
//  cout << __PRETTY_FUNCTION__ << " before lock for task " << tc->fTask->Id() << endl;
  OOPObjectId id = tc->fTask->Id();
  if (!tc->fTask->IsRecurrent())
  {
    delete tc->fTask;
    tc->fTask=0;
  }

  TMLock lock;
  //cout << __PRETTY_FUNCTION__ << " after lock for task" << tc->fTask->Id() << endl;
  //tc->fTask->SetExecuting(false);
  tc->fExecFinished =1;
#ifdef LOGPZ
  {
    sout << "Task " << id << " finished";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  TM->Signal(lock);
  MPE_Log_event(tc->m_MPEEvtEnd, 0, NULL);

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
