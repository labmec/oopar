#include "ooptaskcontrol.h"
#include "ooptask.h"
#include "ooperror.h"

#include <sstream>

#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));
#endif

OOPTaskControl::OOPTaskControl (OOPTask * task):fTask (task)
{
  fExecStarted = 0;
  fExecFinished = 0;
	if (task) {
		fDepend = task->GetDependencyList ();
		fDepend.ClearPointers ();
	}
	pthread_mutex_init(&fStateMutex, NULL);

}
OOPTaskControl::~OOPTaskControl ()
{
	delete  fTask;
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
#ifdef LOGPZ  
  {
    sout << "Task " << tc->fTask->Id() << " started";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif  
  tc->fExecStarted = 1;
  tc->fTask->Execute();
  tc->fTask->SetExecuting(false);
  pthread_mutex_lock(&(tc->fStateMutex));
  tc->fExecFinished =1;
  pthread_mutex_unlock(&(tc->fStateMutex));
#ifdef LOGPZ  
  {
    sout << "Task " << tc->fTask->Id() << " finished";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif  
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
