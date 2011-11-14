//
// C++ Implementation: oopdelobjecttask
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopdelobjecttask.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"

//extern OOPDataManager *DM;

#ifdef LOG4CXX
#include <pzlog.h>
static LoggerPtr logger(Logger::getLogger("OOPar.OOPDataManager"));
#endif


template class TPZRestoreClass<OOPDelObjectTask, TDELOBJECTTASK_ID>;


OOPDelObjectTask::OOPDelObjectTask()
: OOPTask()
{
}
OOPDelObjectTask::OOPDelObjectTask(OOPObjectId & Id, int procid)
: OOPTask(procid)
{
	fTargetObject = Id;
}

OOPDelObjectTask::~OOPDelObjectTask()
{
}


void OOPDelObjectTask::Write(TPZStream & buf, int withclassid)
{
	OOPTask::Write(buf, withclassid);
	fTargetObject.Write( buf, 0);
}

void OOPDelObjectTask::Read(TPZStream & buf, void * context)
{
	OOPTask::Read(buf, context);
	fTargetObject.Read( buf, context);
}

int OOPDelObjectTask::ClassId() const
{
	return TDELOBJECTTASK_ID;
}

OOPMReturnType OOPDelObjectTask::Execute()
{
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "OOPDelObjectTask Executing";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	fTM->DM()->RequestDelete(fTargetObject);
	OOPTask::Execute();
	return ESuccess;
}
