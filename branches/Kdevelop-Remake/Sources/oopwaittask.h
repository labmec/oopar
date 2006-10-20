//
// C++ Interface: oopwaittask
//
// Description: 
//
//
// Author: Philippe R. B. Devloo <phil@fec.unicamp.br>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPWAITTASK_H
#define OOPWAITTASK_H

#include <ooptask.h>
class OOPStorageBuffer;

/**
This task creates an entry point into the OOPAR system. The Task contains two central methods. Wait() will cause the calling thread to sleep till the data dependency of the task is satisfied. Finish() will release the datadependency from the OOPAR system

@author Philippe R. B. Devloo
*/
class OOPWaitTask : public OOPTask
{
  /**
   Mutex to control the execution of the external thread
   */
  pthread_mutex_t fExtMutex;
  /**
   Mutex to control the execution of the execution thread
   */
  pthread_mutex_t fExecMutex;
  
  /**
  Condition variable to put the external thread to sleep
  */
  pthread_cond_t fExtCond;
  /**
  Condition variable to put the execution thread to sleep
  */
  pthread_cond_t fExecCond;
  /**
  Some "idiot" may want to call a wait task while another wait task is executing
  */
  static int gCounter;

    void LockExternal();
public:
	OOPWaitTask(){}
    OOPWaitTask(int Procid);

    ~OOPWaitTask();

    virtual void Write(TPZStream & buf, int withclassid);
    virtual void Read(TPZStream & buf, void * context);
    virtual int ClassId() const;
    virtual OOPMReturnType Execute();
    void Finish();
    void Wait();
    using OOPTask::fProc;

};

#endif
