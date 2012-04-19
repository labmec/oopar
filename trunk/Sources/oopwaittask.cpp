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
#include <errno.h>

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
static LoggerPtr logger(Logger::getLogger("OOPar.WaitTask"));
#endif

#ifdef OOP_MPE
#include "oopevtid.h"
#endif

class OOPStorageBuffer;

OOPWaitTask::OOPWaitTask(int Procid): OOPTask(Procid)
{
    fMainSemaphore = new TPZSemaphore;
    fExecSemaphore = new TPZSemaphore;
//	fMainSemaphore = new boost::interprocess::interprocess_semaphore(0);
//	fExecSemaphore = new boost::interprocess::interprocess_semaphore(0);
}

OOPWaitTask::~OOPWaitTask()
{
	//sem_destroy(&fMainSemaphore);
	delete fMainSemaphore;
	delete fExecSemaphore;
	//sem_destroy(&fExecSemaphore);
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
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "Inside Execute of WaitTask ID " << Id() << " Posting fMainSemaphore";
		LOGPZ_DEBUG(logger, sout.str());
	}
#endif
	//this->IncrementWriteDependentData();
	//sem_post(&fMainSemaphore);
	fMainSemaphore->Post();
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "WaitTask ID " << Id() << " waiting post on fExecSemaphore";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	//sem_wait(&fExecSemaphore);
	fExecSemaphore->Wait();
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "WaitTask ID " <<  Id() << " Leaving execute";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	return ESuccess;
}

/*!
 \fn OOPWaitTask::Finish()
 */
void OOPWaitTask::Finish()
{
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << __PRETTY_FUNCTION__ << " WaitTask ID " << Id() << " Finished ! Posting fExecSemaphore";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	//sem_post(&fExecSemaphore);
	fExecSemaphore->Post();
}


/*!
 \fn OOPWaitTask::Wait()
 */
void OOPWaitTask::Wait()
{
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "WaitTask ID " << Id() << " Waiting for Post in fMainSemaphore";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	int retval = 0;
	//sem_init(&fMainSemaphore, 0, 1);
	//retval = sem_wait(&fMainSemaphore);
	fMainSemaphore->Wait();
	if(retval == -1)
	{
#ifdef LOG4CXX
		{
			stringstream sout;
			sout << "WaitTask ID " << Id() << " sem_wai failed ! killing application ... bye bye\nError number = " << errno;
			//GetLastEroor()
			if(errno == EAGAIN)
				sout << "The semaphore was already locked, so it cannot be immediately locked by the sem_trywait() operation ( sem_trywait only).";
			if(errno == EINVAL)
				sout << "The sem argument does not refer to a valid semaphore.";
			if(errno == ENOSYS)
				sout << "The functions sem_wait() and sem_trywait() are not supported by this implementation.";
			if(errno == EDEADLK)
				sout << "A deadlock condition was detected.";
			if(errno ==	EINTR)
				sout << "A signal interrupted this function.";
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif
		exit(-1);
	}
	
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "WaitTask ID " << Id() << " fMainSemaphore Posted ! Leaving Wait";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
}
