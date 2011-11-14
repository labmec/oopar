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
//#include "oopmdatadepend.h"
#include "oopmetadata.h"

#include <unistd.h>

TSmallTask::TSmallTask(int Procid): OOPTask(Procid)
{
}


TSmallTask::~TSmallTask()
{
}


TPZSaveable* TSmallTask::Restore(TPZStream & buf, void * context)
{
	TSmallTask *loc = new TSmallTask(-1);
	loc->Read(buf,context);
	return loc;
}

void TSmallTask::Write(TPZStream & buf, int withclassid)
{
    OOPTask::Write(buf,withclassid);
}

void TSmallTask::Read(TPZStream & buf, void * context)
{
    OOPTask::Read(buf,context);
}

int TSmallTask::ClassId () const{
	return TSMALLTASKID;
}

OOPMReturnType TSmallTask::Execute()
{
	//  OOPMDataDependList &deplist = this->GetDependencyList();
	OOPAccessTagList &deplist = this->GetDependencyList();
	int i,nel = deplist.Count();
	for(i=0; i<nel; i++) 
	{
		//    OOPMDataDepend &dep = deplist.Dep(i);
		OOPAccessTag dep = deplist.GetTag(i);
		if(dep.AccessMode() == EWriteAccess || dep.AccessMode() == EVersionAccess) {
			OOPDataVersion ver = dep.Version();
			ver.Increment();
			dep.SetVersion(ver);
			dep.SetTaskId(Id());
		}
	}
	sleep(2);
	
	return ESuccess;
}

template class TPZRestoreClass<TSmallTask,TSMALLTASKID>;
