/*
 *  OOPMatVecMultiply.cpp
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPMatVecMultiply.h"
#include "pzfmatrix.h"

#include "OOPMergeMatrix.h"
#include <sstream>

#ifdef LOGPZ
#include "pzlog.h"
#endif
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPar.root.log"));
/*static LoggerPtr taglogger(Logger::getLogger("OOPar.OOPDataManager.OOPAccessTag"));
static LoggerPtr HandleMsglogger(Logger::getLogger("OOPar.OOPDataManager.DMHandleMessages"));
static LoggerPtr AccessLogger(Logger::getLogger("OOPar.OOPDataManager.OOPAccessTag"));
static LoggerPtr ServiceLogger(Logger::getLogger("OOPar.OOPDataManager.ServiceLogger"));
static LoggerPtr DaemonLogger(Logger::getLogger("OOPar.OOPTaskManager.DaemonTask"));
static LoggerPtr MetaLogger(Logger::getLogger("OOPar.OOPDataManager.MetaData"));
 */
#endif


OOPMReturnType OOPMatVecMultiply::Execute()
{
	
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Task T:" << Id() << " Entering execution";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	
  cout << "Executing Task " << Id() << " For Matrix Vector multiplication\n";
	cout.flush();
	TPZMatrix * Matrix = dynamic_cast<TPZMatrix * >(fDependRequest.ObjectPtr(0));
	//Matrix->Print("Matriz na tarefa", cout, EFormatted);
	//m_Vector.Print("Vetor na tarefa", cout, EFormatted);
	cout.flush();
	TPZFMatrix Vector(Matrix->Cols(), 1);
	Matrix->Multiply(m_Vector, Vector);
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Task T:" << Id() << " Performing multiplication";
		sout << "Multiplying \n";
		Matrix->Print("SubMatrix", sout, EFormatted);
		m_Vector.Print("SubVec", sout, EFormatted);
		Vector.Print("SubRes", sout, EFormatted);
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif

	/*
	for(i = 0; i < Matrix->Rows(); i++)
	{
		double value = 0;
		for(j = 0; j < Matrix->Cols(); j++)
		{
			value += Matrix->Get(i,j) * m_Vector.Get(j,0);
		}
		Vector.Put(i, 0, value);
	}
	 */
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Task T:" << Id() << " Creating Merge tasks";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	
  //IncrementWriteDependentData();
  CreateMergeTask(Vector);
	cout << "Leaving Executing Task " << endl;
  return ESuccess;
}

void OOPMatVecMultiply::CreateMergeTask(TPZFMatrix & vector)
{	
	std::cout << "Creating Merge Tasks\n";
	std::cout.flush();
	OOPObjectId vecId;
	int subMatTarget = this->GetProcID();
	std::cout << "Target Id " << subMatTarget << endl;
	std::cout.flush();
	OOPMergeMatrix * task = new OOPMergeMatrix(0);
	std::cout << "From Processor " << GetProcID() << endl;
	std::cout.flush();
	
	task->SetFVector(vector);
	task->m_SubId = subMatTarget;
	
	//vector.Print("Vetor depois do task", cout, EFormatted);
	cout.flush();
	
	cout << "Tag de acesso Antes " << endl;
	cout.flush();
	OOPAccessTag tag = fDependRequest.GetTag(0);
	cout << "Tag de acesso " << tag << endl;
	cout.flush();
	OOPDataVersion globalVer;
	globalVer.SetLevelVersion(0,-1);
	task->AddDependentData(OOPAccessTag(m_GlobalVecId, EWriteAccess, globalVer, 0));

	task->AddDependentData(OOPAccessTag(m_IndexId, EReadAccess, globalVer, 0));
	task->Submit();
	
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Task T:" << Id() << " Submitted MergeTask with Id T:"<< task->Id() ;
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	
	
	std::cout << "Submitted task for Vector merge\n";
	std::cout.flush();

}
void OOPMatVecMultiply::Write(TPZStream & buf, int withclassid)
{
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Task T:" << Id() << " Being Written on Buffer";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
	
#endif
	OOPTask::Write(buf, withclassid);
	
	m_IndexId.Write(buf, 0);
	m_GlobalVecId.Write(buf, 0);
	
	m_Vector.Write(buf, 0);

	int clsid = ClassId();
	buf.Write(&clsid);
}
void OOPMatVecMultiply::Read(TPZStream & buf, void * context)
{
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Task T:" << Id() << " Being read from Buffer";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
	
#endif
	OOPTask::Read(buf, context);

  m_IndexId.Read(buf, 0);
	m_GlobalVecId.Read(buf, 0);

	m_Vector.Read(buf, 0);
 	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
}
template class TPZRestoreClass<OOPMatVecMultiply, OOPMATVECMULTIPLY_ID>;


