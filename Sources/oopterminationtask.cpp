/*
 * oopterminationtask.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: phil
 */

#include "oopterminationtask.h"
#include "ooplock.h"
#include "ooptaskmanager.h"
#include "oopcommmanager.h"

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
static LoggerPtr logger (Logger::getLogger ("OOPar.OOPTerminationTask"));
#endif


template class TPZRestoreClass < OOPTerminationTask, TTERMINATIONTASK_ID >;


OOPTerminationTask::~OOPTerminationTask ()
{
}

OOPTerminationTask::OOPTerminationTask (int ProcId):OOPTask (ProcId)
{
	fStatus = EStopSending;
}
OOPTerminationTask::OOPTerminationTask (const OOPTerminationTask & term):
OOPTask (term), fStatus(term.fStatus), fProcOrigin(term.fProcOrigin)
{
}

OOPMReturnType OOPTerminationTask::Execute ()
{
  //sleep(5)

	{
#ifdef LOG4CXX
    	std::stringstream sout;
    	sout << __PRETTY_FUNCTION__ << " fStatus " << fStatus;
    	LOGPZ_DEBUG(logger,sout.str())
#endif
    if(fStatus == EStopSending)
    {
#ifdef LOG4CXX
    	std::stringstream sout;
    	sout << "Received Stop Sending command from processor " << fProcOrigin << " sending stop confirmation";
    	LOGPZ_DEBUG(logger,sout.str())
#endif
    	TM()->StopSending();
    	OOPTerminationTask *task = new OOPTerminationTask(fProcOrigin);
    	task->SetStatus(EStopSendingConfirmation);
    	task->fProcOrigin = TM()->CM()->GetProcID();
    	TM()->Submit(task);
    }
    else if(fStatus == EStopSendingConfirmation)
    {
#ifdef LOG4CXX
    	std::stringstream sout;
    	sout << "Received Stop Sending Confirmation";
    	LOGPZ_DEBUG(logger,sout.str())
#endif
    	TM()->StopSendingConfirmation(fProcOrigin);
    }
    else if(fStatus == EShutdown)
    {
#ifdef LOG4CXX
    	std::stringstream sout;
    	sout << "Received Shutdown";
    	LOGPZ_DEBUG(logger,sout.str())
#endif
    	TM()->SetKeepGoing (false);
    }
  }
 /*
	{
    OOPDMLock lock;
    DM()->SetKeepGoing (false);
  }
 */

	//sleep(5);

 // IncrementWriteDependentData();
  /*
	TM->WakeUpCall();
  DM()->WakeUpCall();
	*/
  return ESuccess;
}

void OOPTerminationTask::Write (TPZStream & buf, int withclassid)
{
  OOPTask::Write (buf, withclassid);
  buf.Write(&fStatus);
  buf.Write(&fProcOrigin);
}

void OOPTerminationTask::Read (TPZStream & buf, void *context)
{
  OOPTask::Read (buf, context);
  buf.Read(&fStatus);
  buf.Read(&fProcOrigin);
}

long int OOPTerminationTask::ExecTime ()
{
  return -1;
}

TPZSaveable * OOPTerminationTask::Restore (TPZStream & buf, void *context)
{
  OOPTerminationTask *v = new OOPTerminationTask (0);
  v->Read (buf);
  return v;
}

void OOPTerminationTask::SetStatus(ETerminate status)
{
	fStatus = status;
}

