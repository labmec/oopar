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
#include <sstream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("OOPAR.WaitTask"));

class OOPStorageBuffer;

int OOPWaitTask::gCounter = 0;

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
  OOPTask::Write(buf, withclassid);
  LOG4CXX_WARN(logger,"OOPWaitTask should never be packed\n");
}

void OOPWaitTask::Read(TPZStream & buf, void * context)
{
    OOPTask::Read(buf, context);
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
  stringstream sout;
  sout << "Wait task is leaving execute id " << Id();
  LOG4CXX_DEBUG(logger,sout.str());
  this->IncrementWriteDependentData();
  return ESuccess;
}

/*!
    \fn OOPWaitTask::Finish()
 */
void OOPWaitTask::Finish()
{
  gCounter--;
  pthread_mutex_lock(&fExecMutex);
  pthread_cond_signal(&fExecCond);
  pthread_mutex_unlock(&fExecMutex);
}


/*!
    \fn OOPWaitTask::Wait()
 */
void OOPWaitTask::Wait()
{
  if(gCounter) 
  {
    std::stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Recursive call of wait task " << Id();
    LOG4CXX_ERROR(logger,sout.str());
  }
  gCounter++;
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
