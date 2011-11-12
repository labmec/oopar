/*
 *  OOPComputeDiagTask.cpp
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPComputeMatricesTask.h"
#include "OOPContributeDiagonalTask.h"
#include "tpzdohrassembly.h"
#include "tpzdohrsubstruct.h"
#include "ooptaskmanager.h"



OOPComputeMatricesTask::OOPComputeMatricesTask()
{
}
OOPComputeMatricesTask::OOPComputeMatricesTask(int processor) : OOPTask(processor)
{
}
void OOPComputeMatricesTask::SetMainDiagId(OOPObjectId & Id)
{
	m_MainDiagId = Id;
}
OOPMReturnType OOPComputeMatricesTask::Execute()
{
	OOPContributeDiagonalTask * contribDiagTask = new OOPContributeDiagonalTask(0);
	//TPZDohrAssembly * assembly = dynamic_cast<TPZDohrAssembly *> (fDependRequest.ObjectPtr(0));
	//TPZDohrSubstruct * sub = dynamic_cast<TPZDohrSubstruct *> (fDependRequest.ObjectPtr(1));
	OOPDataVersion version;
	version.SetLevelVersion(0, -1);
	contribDiagTask->AddDependentData(OOPAccessTag(m_MainDiagId, EWriteAccess, version,0));
	TM()->Submit(contribDiagTask);
	return ESuccess;
}

void OOPComputeMatricesTask::Write(TPZStream & buf, int withclassid){
	OOPTask::Write(buf, withclassid);
	int clsid = ClassId();
	buf.Write(&clsid);
	m_MainDiagId.Write(buf);

}
void OOPComputeMatricesTask::Read(TPZStream & buf, void * context){
	OOPTask::Read(buf, context);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
	m_MainDiagId.Read(buf);
}
template class TPZRestoreClass<OOPComputeMatricesTask, OOPCOMPUTEMATRICESTASK_ID>;
