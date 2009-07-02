/*
 *  OOPContributeDiagonalTask.cpp
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#include "OOPContributeDiagonalTask.h"
#include "pzfmatrix.h"

template class TPZRestoreClass<OOPContributeDiagonalTask, OOPCONTRIBUTEDIAGONALTASK_ID>;

OOPContributeDiagonalTask::OOPContributeDiagonalTask(int processor)
: OOPTask(processor)
{
}

OOPMReturnType OOPContributeDiagonalTask::Execute()
{
	TPZFMatrix * matDiag = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	int i;
	for(i = 0; i < m_Indices.NElements(); i++)
	{
		matDiag->PutVal(m_Indices[i], 0, m_Values[i]);
	}
	return ESuccess;
}

void OOPContributeDiagonalTask::Write(TPZStream & buf, int withclassid)
{
	OOPTask::Write(buf, withclassid);
	int clsid = ClassId();
	buf.Write(&clsid);
	TPZSaveable::WriteObjects(buf, m_Indices);
	TPZSaveable::WriteObjects(buf, m_Values);
}
void OOPContributeDiagonalTask::Read(TPZStream & buf, void * context)
{
	OOPTask::Read(buf, context);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
	TPZSaveable::ReadObjects(buf, m_Indices);
	TPZSaveable::ReadObjects(buf, m_Values);
	
}

