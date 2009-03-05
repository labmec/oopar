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

extern OOPDataManager *DM;

#ifdef LOG4CXX
#include <pzlog.h>
static LoggerPtr logger(Logger::getLogger("OOPar.OOPDataManager"));
#endif

OOPDelObjectTask::OOPDelObjectTask()
 : OOPTask(DM->GetProcID())
{
}
OOPDelObjectTask::OOPDelObjectTask(OOPObjectId & Id)
 : OOPTask(DM->GetProcID())
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
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "OOPDelObjectTask Executing";
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
  DM->RequestDelete(fTargetObject);
  OOPTask::Execute();
  return ESuccess;
}
