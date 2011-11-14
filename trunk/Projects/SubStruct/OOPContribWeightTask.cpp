/*
 *  OOPContribWeightTask.cpp
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPContribWeightTask.h"

#include "tpzdohrsubstruct.h"
#include "tpzdohrassembly.h"

template class TPZRestoreClass<OOPContribWeightTask, OOPCONTRIBWEIGHTTASK_ID>;

OOPContribWeightTask::OOPContribWeightTask(int processor) : OOPTask(processor)
{
}
OOPContribWeightTask::OOPContribWeightTask()
{
}
void OOPContribWeightTask::Write(TPZStream & buf, int withclassid)
{
	OOPTask::Write(buf, withclassid);
	int clsid = ClassId();
	buf.Write(&clsid);
	TPZSaveable::WriteObjects(buf, m_Values);
}
void OOPContribWeightTask::Read(TPZStream & buf, void * context)
{
	OOPTask::Read(buf, context);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
	TPZSaveable::ReadObjects(buf, m_Values);
}

OOPMReturnType OOPContribWeightTask::Execute()
{
	//TPZDohrAssembly * assembly = dynamic_cast<TPZDohrAssembly *> (fDependRequest.ObjectPtr(0));
	//TPZDohrSubstruct * subStr = dynamic_cast<TPZDohrSubstruct * > (fDependRequest.ObjectPtr(1));
	// variables are not referenced ... ???
	
	return ESuccess;
}

