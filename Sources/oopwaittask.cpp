//
// C++ Implementation: oopwaittask
//
// Description: 
//
//
// Author: Philippe R. B. Devloo <phil@fec.unicamp.br>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopwaittask.h"
class OOPStorageBuffer;
extern ofstream TaskManLog;

OOPWaitTask::OOPWaitTask(int Procid): OOPTask(Procid)
{
  pthread_cond_init(&fExecCond, NULL);
  pthread_cond_init(&fExtCond, NULL);
  pthread_mutex_init(&fExtMutex, NULL);
  pthread_mutex_init(&fExecMutex, NULL);
  LockExternal();
}


OOPWaitTask::~OOPWaitTask()
{
}


void OOPWaitTask::Write(TPZStream & buf, int withclassid)
{
  OOPTask::Write(buf);
  cout << "OOPWaitTask should never be packed\n";
  
}

void OOPWaitTask::Read(TPZStream & buf, void * context)
{
    OOPTask::Read(buf);
}

int OOPWaitTask::ClassId() const
{
    return -1;
}

OOPMReturnType OOPWaitTask::Execute()
{
  pthread_mutex_lock(&fExecMutex);
  pthread_mutex_lock(&fExtMutex);
  pthread_cond_signal(&fExtCond);
  pthread_mutex_unlock(&fExtMutex);
  //sleep(10);
  pthread_cond_wait(&fExecCond, &fExecMutex);
  TaskManLog << "Wait task is leaving execute id " << Id() << endl;
  return ESuccess;
}



/*!
    \fn OOPWaitTask::Finish()
 */
void OOPWaitTask::Finish()
{
  cout << "Entering finish\n";
  pthread_mutex_lock(&fExecMutex);
  cout << "Acquired lock \n";
  TaskManLog << "Wait task signaling ExecCond " << endl;
  pthread_cond_signal(&fExecCond);
  pthread_mutex_unlock(&fExecMutex);
  cout << "Signaled the ExecCond and unlocked the ExecMutex\n";
//  sleep(20);
}


/*!
    \fn OOPWaitTask::Wait()
 */
void OOPWaitTask::Wait()
{
//  pthread_mutex_lock(&fExtMutex);
  pthread_cond_wait(&fExtCond,&fExtMutex);
}


/*!
    \fn OOPWaitTask::LockExternal()
 */
void OOPWaitTask::LockExternal()
{
    pthread_mutex_lock(&fExtMutex);
}
