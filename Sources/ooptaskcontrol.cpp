#include "ooptaskcontrol.h"
#include "ooptask.h"

#include <sstream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));

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
  static int numthreads = 0;
//  cout << __FUNCTION__ << " creating trhead number " << numthreads++ << " max threads " << PTHREAD_THREADS_MAX << endl;
  if(pthread_create(&fExecutor, NULL, ThreadExec, this)){
    stringstream sout;
    sout << "Fail to create service thread\n";
    sout << "Going out\n";
    LOG4CXX_DEBUG(logger,sout);
  }
}
extern ofstream TaskManLog;

void *OOPTaskControl::ThreadExec(void *threadobj)
{
  stringstream sout;
  OOPTaskControl *tc = (OOPTaskControl *) threadobj;
  /*TaskManLog*/ sout << "Task " << tc->fTask->Id() << " started\n";
  
  tc->fExecStarted = 1;
  tc->fTask->Execute();
  tc->fTask->SetExecuting(false);
  pthread_mutex_lock(&(tc->fStateMutex));
  tc->fExecFinished =1;
  pthread_mutex_unlock(&(tc->fStateMutex));
  /*TaskManLog*/ sout << "Task " << tc->fTask->Id() << " finished\n";
  LOG4CXX_DEBUG(logger,sout);
  //TaskManLog.flush();
  return 0;
}

void OOPTaskControl::Join()
{
  if(fExecutor == pthread_self())
  {
    stringstream sout;
    sout << __FUNCTION__ << " called by the taskcontrol object itself\n";
    LOG4CXX_DEBUG(logger,sout);
    return;
  }
  void *execptr;
  void **executorresultptr = &execptr;
  int result = pthread_join(fExecutor,executorresultptr);
  if(result)
  {
    stringstream sout;
    sout << __FUNCTION__ << __LINE__ << " join operation failed with result " << result << endl;
    LOG4CXX_DEBUG(logger,sout);
  }
}