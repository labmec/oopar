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
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.WaitTask"));
#endif

#ifdef OOP_MPE
#include "oopevtid.h"
#endif

class OOPStorageBuffer;

OOPWaitTask::OOPWaitTask(int Procid): OOPTask(Procid)
{
  sem_init(&fMainSemaphore, 0, 0); 
  sem_init(&fExecSemaphore, 0, 0); 
}

OOPWaitTask::~OOPWaitTask()
{
  sem_destroy(&fMainSemaphore);
  sem_destroy(&fExecSemaphore);
}


void OOPWaitTask::Write(TPZStream & buf, int withclassid)
{
  OOPTask::Write(buf, withclassid);
  LOGPZ_WARN(logger,"OOPWaitTask should never be packed\n");
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
  sem_post(&fMainSemaphore);
  this->IncrementWriteDependentData();
  sem_wait(&fExecSemaphore);
  return ESuccess;
}

/*!
    \fn OOPWaitTask::Finish()
 */
void OOPWaitTask::Finish()
{
  sem_post(&fExecSemaphore);
}


/*!
    \fn OOPWaitTask::Wait()
 */
void OOPWaitTask::Wait()
{
  int res = sem_wait(&fMainSemaphore);
}
