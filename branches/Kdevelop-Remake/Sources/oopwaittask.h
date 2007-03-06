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

#include <semaphore.h>

#include <ooptask.h>
class OOPStorageBuffer;

/**
This task creates an entry point into the OOPAR system. The Task contains two central methods. Wait() will cause the calling thread to sleep till the data dependency of the task is satisfied. Finish() will release the datadependency from the OOPAR system.
 * Uses semaphore for synchronization.

@author Philippe R. B. Devloo
*/
class OOPWaitTask : public OOPTask
{
  /**
   * Semaphore for the Main Thread Execution
   */
  sem_t fMainSemaphore;
  /**
   * Semaphore for the WaitTask execution
   */
  sem_t fExecSemaphore;
public:
  OOPWaitTask()
  {
    sem_init(&fMainSemaphore, 0, 0); 
    sem_init(&fExecSemaphore, 0, 0); 
  }
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
