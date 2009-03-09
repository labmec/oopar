/*
 *  OOPMergeMatrix.cpp
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPMergeMatrix.h"
#include "OOPParMatIndexation.h"

#include <sstream>

#ifdef LOGPZ
#include "pzlog.h"
static LoggerPtr logger(Logger::getLogger("OOPar.root.log"));
#endif



OOPMReturnType OOPMergeMatrix::Execute()
{

  cout << "Executing Task " << Id() << " For Dispersed vector aggregation ";
	TPZMatrix * GlobalVector = dynamic_cast<TPZMatrix * > (fDependRequest.ObjectPtr(0));
	OOPParMatIndexation * Indices = NULL;
	Indices = dynamic_cast<OOPParMatIndexation * >  (fDependRequest.ObjectPtr(1));
	
#ifdef LOGPZ
	{
		if(!Indices)
		{
			std::stringstream sout;
			sout << "Indices Object not available";
			LOGPZ_ERROR(logger, sout.str().c_str());
		}
	}
#endif
	
	std::vector<int> rows = Indices->GetRowVector(m_SubId);
	
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Rows.size on MergeTask = " << rows.size();
		LOGPZ_ERROR(logger, sout.str().c_str());
	}
#endif
	int i;
#ifdef LOGPZ
	std::stringstream sout;
	for(i = 0; i < rows.size(); i++)
	{
		sout << "Row[" << i << "] = " << rows[i] << endl;
	}
	GlobalVector->Print("Global Vector Original", sout, EFormatted);
#endif
	for(i = 0; i < rows.size(); i++)
	{
		GlobalVector->Put(rows[i], 0, GlobalVector->Get(rows[i], 0) + m_Vector.Get(i, 0));
	}
	
  //IncrementWriteDependentData();
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Leaving Merge Task Id " << Id();
		sout << "\nResulting Vector\n";
		m_Vector.Print("Local Vector", sout, EFormatted);
		GlobalVector->Print("Global Vector After Contribution", sout, EFormatted);
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
  cout << "Leaving Merge Task execution " << endl;
  return ESuccess;
}

void OOPMergeMatrix::Write(TPZStream & buf, int withclassid){
	OOPTask::Write(buf, withclassid);
	int clsid = ClassId();
	buf.Write(&clsid);
	buf.Write(&m_SubId, 1);
	cout << "m_SubId " << m_SubId << endl;
	cout.flush();
	m_Vector.Write(buf, 0);
	m_Vector.Print("Na tarefa Merge", cout, EFormatted);
	cout.flush();
}
void OOPMergeMatrix::Read(TPZStream & buf, void * context){
	OOPTask::Read(buf, context);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
	buf.Read(&m_SubId, 1);
	cout << "m_SubId " << m_SubId << endl;
	cout.flush();
	m_Vector.Read(buf, 0);
	m_Vector.Print("Depois de lido" , cout, EFormatted);
	cout.flush();
}
template class TPZRestoreClass<OOPMergeMatrix, OOPMERGETASK_ID>;

