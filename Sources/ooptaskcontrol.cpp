#include "ooptaskcontrol.h"
#include "ooptask.h"
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
    cerr << "Fail to create service thread\n";
    cerr << "Going out\n";
    cerr.flush();
  }
}
extern ofstream TaskManLog;

void *OOPTaskControl::ThreadExec(void *threadobj)
{
  OOPTaskControl *tc = (OOPTaskControl *) threadobj;
  TaskManLog << "Task " << tc->fTask->Id() << " started\n";
  tc->fExecStarted = 1;
  tc->fTask->Execute();
  tc->fTask->SetExecuting(false);
  pthread_mutex_lock(&(tc->fStateMutex));
  tc->fExecFinished =1;
  pthread_mutex_unlock(&(tc->fStateMutex));
  TaskManLog << "Task " << tc->fTask->Id() << " finished\n";
  TaskManLog.flush();
  return 0;
}

void OOPTaskControl::Join()
{
  if(fExecutor == pthread_self())
  {
    cerr << __FUNCTION__ << " called by the taskcontrol object itself\n";
    return;
  }
  void *execptr;
  void **executorresultptr = &execptr;
  int result = pthread_join(fExecutor,executorresultptr);
  if(result)
  {
    cerr << __FUNCTION__ << __LINE__ << " join operation failed with result " << result << endl;
  }
}