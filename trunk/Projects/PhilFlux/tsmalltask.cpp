//
// C++ Implementation: tsmalltask
//
// Description: 
//
//
// Author: Philippe R. B. Devloo <phil@fec.unicamp.br>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tsmalltask.h"
#include "oopmdatadepend.h"
#include "oopmetadata.h"

TSmallTask::TSmallTask(int Procid): OOPTask(Procid)
{
}


TSmallTask::~TSmallTask()
{
}


OOPSaveable* TSmallTask::Restore(OOPReceiveStorage* buf)
{
  TSmallTask *loc = new TSmallTask(-1);
  loc->Unpack(buf);
  return loc;
}

int TSmallTask::Pack(OOPSendStorage* buf)
{
    return OOPTask::Pack(buf);
}

int TSmallTask::Unpack(OOPReceiveStorage* buf)
{
    return OOPTask::Unpack(buf);
}

long TSmallTask::GetClassID()
{
    return TSMALLTASKID;
}

OOPMReturnType TSmallTask::Execute()
{
  OOPMDataDependList &deplist = this->GetDependencyList();
  int i,nel = deplist.NElements();
  for(i=0; i<nel; i++) 
  {
    OOPMDataDepend &dep = deplist.Dep(i);
    if(dep.State() == EWriteAccess || dep.State() == EVersionAccess) {
      OOPDataVersion ver = dep.ObjPtr()->Version();
      ver.Increment();
      dep.ObjPtr()->SetVersion(ver,Id());
    }
  }
  
  return ESuccess;
}

