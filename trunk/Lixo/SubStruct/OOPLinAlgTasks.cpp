/*
 *  OOPLinAlgTasks.cpp
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 5/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPInt.h"
#include "OOPDouble.h"

#include "TPZFParMatrix.h"
#include "OOPLinAlgTasks.h"

#include "pzlog.h"
#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif



OOPLinAlgTasks::OOPLinAlgTasks(int processor) :
  OOPTask(processor)
{
  m_Type = EUndefined;
}
OOPLinAlgTasks::OOPLinAlgTasks() :
  OOPTask()
{
  m_Type = EUndefined;
}

void OOPLinAlgTasks::SetType(ELinAlgOpType theType)
{
  m_Type = theType;
}

ELinAlgOpType OOPLinAlgTasks::GetType()
{
  return m_Type;
}
void OOPLinAlgTasks::Write(TPZStream & buf, int withclassid)
{
  OOPTask::Write(buf, withclassid);
  int clsid = ClassId();
  buf.Write(&clsid);
  buf.Write((int*) &m_Type, 1);
}
void OOPLinAlgTasks::Read(TPZStream & buf, void * context)
{
  OOPTask::Read(buf, context);
  int clsid = 0;
  buf.Read(&clsid);
  if (ClassId() != clsid)
  {
    cout << "ClassId Missmatch\n";
  }
  buf.Read((int*) &m_Type, 1);
}


OOPMReturnType OOPLinAlgTasks::Execute()
{
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Entering " << __PRETTY_FUNCTION__ << std::endl;
    sout << "With Type " << m_Type;
    LOGPZ_DEBUG(logger, sout.str());
    std::cout << sout.str().c_str() << std::endl;
    std::cout.flush();
  }
#endif

  switch (m_Type) {
  case EZAXPY: {
    ZAXPY();
    break;
  }
  case ETimesBetaPlusZ: {
    TimesBetaPlusZ();
    break;
  }
  case EMultAdd: {
    MultAdd();
    break;
  }
  case EMultiply: {
    Multiply();
    break;
  }
  case EZero: {
#ifdef LOG4CXX
    {
      std::stringstream sout;
      sout << "Calling Zero on " << __PRETTY_FUNCTION__ << "\n";
      sout << "In processor # " << this->GetProcID();
      LOGPZ_DEBUG(logger, sout.str());
      std::cout << sout.str().c_str() << std::endl;
      std::cout.flush();
    }
#endif
    Zero();
    break;
  }
  case ECopyOperator: {
#ifdef LOG4CXX
    {
      std::stringstream sout;
      sout << "Calling Copy on " << __PRETTY_FUNCTION__ << "\n";
      sout << "In processor # " << this->GetProcID();
      LOGPZ_DEBUG(logger, sout.str());
      std::cout << sout.str().c_str() << std::endl;
      std::cout.flush();
    }
#endif
    Copy();
    break;
  }
  case ERedim: {
    Redim();
    break;
  }
  case EDot: {
#ifdef LOG4CXX
    {
      std::stringstream sout;
      sout << "Calling Dot on " << __PRETTY_FUNCTION__ << "\n";
      sout << "In processor # " << this->GetProcID();
      LOGPZ_DEBUG(logger, sout.str());
      std::cout << sout.str().c_str() << std::endl;
      std::cout.flush();
    }
#endif
    Dot();
    break;
  }
  default: {
#ifdef LOG4CXX
    {
      std::stringstream sout;
      sout << "Going out of " << __PRETTY_FUNCTION__ << " with Type NOT SET\n";
      LOGPZ_DEBUG(logger, sout.str());
      std::cout << sout.str().c_str() << std::endl;
      std::cout.flush();
    }
#endif
    break;
  }
  }
  return ESuccess;
}
void OOPLinAlgTasks::RemoteTimesBetaPlusZ(TPZFParMatrix & x, const REAL beta,
		const TPZFParMatrix & z, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);
  lTask->SetType(ETimesBetaPlusZ);
  lTask->AddDependentData(OOPAccessTag(x.Id(), EWriteAccess, x.Version(),
      ProcId));
  OOPDouble * dbeta = new OOPDouble;
  dbeta->fValue = beta;
  OOPObjectId betaId = DM->SubmitObject(dbeta);
  OOPDataVersion betaVersion;
  lTask->AddDependentData(OOPAccessTag(betaId, EReadAccess, betaVersion,
      ProcId));
  lTask->AddDependentData(OOPAccessTag(z.Id(), EReadAccess, z.Version(),
      ProcId));
  x.IncrementVersion();
  DM->TM()->Submit(lTask);

}
void OOPLinAlgTasks::RemoteZAXPY(TPZFParMatrix & x, REAL beta,
		const TPZFParMatrix & z, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);
  lTask->SetType(EZAXPY);
  lTask->AddDependentData(OOPAccessTag(x.Id(), EWriteAccess, x.Version(),
      ProcId));
  OOPDouble * dbeta = new OOPDouble;
  dbeta->fValue = beta;
  OOPObjectId betaId = DM->SubmitObject(dbeta);
  OOPDataVersion betaVersion;
  lTask->AddDependentData(OOPAccessTag(betaId, EReadAccess, betaVersion,
      ProcId));
  lTask->AddDependentData(OOPAccessTag(z.Id(), EReadAccess, z.Version(),
      ProcId));
  x.IncrementVersion();
  DM->TM()->Submit(lTask);
}

void OOPLinAlgTasks::ZAXPY() {
	//Implements x.ZAXPY(alpha, p);
	TPZFMatrix * x = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
	OOPDouble * alpha = dynamic_cast<OOPDouble *> (fDependRequest.ObjectPtr(1));
	TPZFMatrix * p = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(2));
	x->ZAXPY(alpha->fValue, *p);
}

void OOPLinAlgTasks::TimesBetaPlusZ()
{
  //Implements p.TimesBetaPlusZ(beta,z);
  TPZFMatrix * p = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
  OOPDouble * beta = dynamic_cast<OOPDouble *> (fDependRequest.ObjectPtr(1));
  TPZFMatrix * z = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(2));
  p->TimesBetaPlusZ(beta->fValue, *z);
}

void OOPLinAlgTasks::RemoteMultAdd(const TPZFParMatrix & me,
		const TPZFParMatrix & x, const TPZFParMatrix &y, TPZFParMatrix &z,
		const REAL alpha, const REAL beta, const int opt, const int stride, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);

  //It triggers the task for the distributed operation
  lTask->SetType(EMultAdd);
  lTask->AddDependentData(OOPAccessTag(me.Id(), EReadAccess, me.Version(),
      ProcId));
  lTask->AddDependentData(OOPAccessTag(x.Id(), EReadAccess, x.Version(),
      ProcId));
  lTask->AddDependentData(OOPAccessTag(y.Id(), EReadAccess, y.Version(),
      ProcId));
  lTask->AddDependentData(OOPAccessTag(z.Id(), EWriteAccess, z.Version(),
      ProcId));

  z.IncrementVersion();

  OOPDataVersion theVersion;

  OOPDouble * dalpha = new OOPDouble;
  dalpha->fValue = alpha;
  OOPObjectId alphaId = DM->SubmitObject(dalpha);

  OOPDouble * dbeta = new OOPDouble;
  dbeta->fValue = beta;
  OOPObjectId betaId = DM->SubmitObject(dbeta);

  OOPInt * dopt = new OOPInt;
  dopt->fValue = opt;
  OOPObjectId optId = DM->SubmitObject(dopt);

  OOPInt * dstride = new OOPInt;
  dstride->fValue = stride;
  OOPObjectId strideId = DM->SubmitObject(dstride);

  lTask->AddDependentData(OOPAccessTag(alphaId, EReadAccess, theVersion,
      ProcId));
  lTask->AddDependentData(
      OOPAccessTag(betaId, EReadAccess, theVersion, ProcId));
  lTask->AddDependentData(OOPAccessTag(optId, EReadAccess, theVersion, ProcId));
  lTask->AddDependentData(OOPAccessTag(strideId, EReadAccess, theVersion,
      ProcId));

  DM->TM()->Submit(lTask);
}

void OOPLinAlgTasks::MultAdd() {
	//Implements me.MultAdd(x,b,r,-1.,1.);

	TPZFMatrix * me = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
	TPZFMatrix * x = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(1));
	TPZFMatrix * y = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(2));
	TPZFMatrix * z = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(3));

	OOPDouble * alpha = dynamic_cast<OOPDouble *> (fDependRequest.ObjectPtr(4));
	OOPDouble * beta = dynamic_cast<OOPDouble *> (fDependRequest.ObjectPtr(5));
	OOPInt * opt = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(6));
	OOPInt * stride = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(7));

	me->MultAdd(*x, *y, *z, alpha->fValue, beta->fValue, opt->fValue,
			stride->fValue);
}

void OOPLinAlgTasks::RemoteMultiply(const TPZFParMatrix & me,
		const TPZFParMatrix &A, TPZFParMatrix&B, int opt, int stride, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);
  lTask->SetType(EMultiply);
  lTask->AddDependentData(OOPAccessTag(me.Id(), EReadAccess, me.Version(),
      ProcId));
  lTask->AddDependentData(OOPAccessTag(A.Id(), EReadAccess, A.Version(),
      ProcId));
  lTask->AddDependentData(OOPAccessTag(B.Id(), EWriteAccess, B.Version(),
      ProcId));

  B.IncrementVersion();

  OOPDataVersion theVersion;

  OOPInt * dopt = new OOPInt;
  dopt->fValue = opt;
  OOPObjectId optId = DM->SubmitObject(dopt);

  OOPInt * dstride = new OOPInt;
  dstride->fValue = stride;
  OOPObjectId strideId = DM->SubmitObject(dstride);

  lTask->AddDependentData(OOPAccessTag(optId, EReadAccess, theVersion, ProcId));
  lTask->AddDependentData(OOPAccessTag(strideId, EReadAccess, theVersion,
      ProcId));

  DM->TM()->Submit(lTask);
}

void OOPLinAlgTasks::Multiply() {
	TPZFMatrix * me = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
	TPZFMatrix * A = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(1));
	TPZFMatrix * B = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(2));

	OOPInt * opt = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(3));
	OOPInt * stride = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(4));

	me->Multiply(*A, *B, opt->fValue, stride->fValue);

}

void OOPLinAlgTasks::RemoteZero(TPZFParMatrix & me, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Calling " << __PRETTY_FUNCTION__ << " on Processor # " << ProcId << "\n";
    LOGPZ_DEBUG(logger, sout.str());
    std::cout << sout.str().c_str() << std::endl;
    std::cout.flush();
  }
#endif

  lTask->SetType(EZero);
  lTask->AddDependentData(OOPAccessTag(me.Id(), EWriteAccess, me.Version(),
      ProcId));
  me.IncrementVersion();
  DM->TM()->Submit(lTask);

}
void OOPLinAlgTasks::Zero()
{
  TPZFMatrix * me = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
  me->Zero();
}

void OOPLinAlgTasks::RemoteCopy(TPZFParMatrix & me, const TPZFParMatrix & copy, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);
  lTask->AddDependentData(OOPAccessTag(me.Id(), EWriteAccess, me.Version(),
      ProcId));
  lTask->AddDependentData(OOPAccessTag(copy.Id(), EReadAccess, copy.Version(),
      ProcId));
  lTask->SetType(ECopyOperator);
  me.IncrementVersion();
  DM->TM()->Submit(lTask);

}
void OOPLinAlgTasks::Copy()
{
	TPZFMatrix * me = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
	TPZFMatrix * copy =
			dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(1));
	*me = *copy;
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Me->Rows " << me->Rows() << endl;
		sout << "Copy->Rows " << copy->Rows() << endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif

}

void OOPLinAlgTasks::RemoteRedim(TPZFParMatrix & me, int rows, int cols, int ProcId)
{
  OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);

  lTask->AddDependentData(OOPAccessTag(me.Id(), EWriteAccess, me.Version(),
      ProcId));
  OOPInt * drows = new OOPInt;
  drows->fValue = rows;
  OOPObjectId rowsId = DM->SubmitObject(drows);

  OOPInt * dcols = new OOPInt;
  dcols->fValue = cols;
  OOPObjectId colsId = DM->SubmitObject(dcols);
  OOPDataVersion dv;

  lTask->AddDependentData(OOPAccessTag(rowsId, EWriteAccess, dv, ProcId));
  lTask->AddDependentData(OOPAccessTag(colsId, EWriteAccess, dv, ProcId));

  lTask->SetType(ERedim);
  me.IncrementVersion();
  DM->TM()->Submit(lTask);
}
void OOPLinAlgTasks::Redim() {
	TPZFMatrix * me = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
	OOPInt * rows = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(1));
	OOPInt * cols = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(2));
	me->Redim(rows->fValue, cols->fValue);
}

REAL OOPLinAlgTasks::RemoteDot(const TPZFParMatrix & A, const TPZFParMatrix & B, int ProcId)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Entering " << __PRETTY_FUNCTION__ << " on Processor # " << ProcId << "\n";
		sout << "Adding Dependent Data with ReadAccess over " << A.Id() << " and " << B.Id();
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	OOPLinAlgTasks * lTask = new OOPLinAlgTasks(ProcId);
	lTask->AddDependentData(OOPAccessTag(A.Id(), EReadAccess, A.Version(),
			ProcId));
	lTask->AddDependentData(OOPAccessTag(B.Id(), EReadAccess, B.Version(),
			ProcId));
	OOPDouble * dotval = new OOPDouble;
	dotval->fValue = 0;
	OOPObjectId dotId = DM->SubmitObject(dotval);
	OOPDataVersion dv;

	lTask->AddDependentData(OOPAccessTag(dotId, EWriteAccess, dv, ProcId));
	lTask->SetType(EDot);
	OOPObjectId lId = DM->TM()->Submit(lTask);
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Submitting LinalgTask with Type EDot with ID:" << lId << endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif

	//Submit wait task which will bring the dot result back to the main thread
	dv.Increment();
	OOPWaitTask * wdot = new OOPWaitTask(DM->GetProcID());
	wdot->AddDependentData(
			OOPAccessTag(dotId, EReadAccess, dv,DM->GetProcID()));
	OOPObjectId id = DM->TM()->Submit(wdot);

#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Waiting for Dot operation completion on Processor # " << ProcId << "\n";
		sout << "Waiting for DotResult in Version " << dv << endl;
		sout << "My ID " << id << endl;
		sout << "Waiting for DotId " << dotId << endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	wdot->Wait();
	OOPDouble * dot = dynamic_cast<OOPDouble *> (wdot->GetDepObjPtr(0));
	REAL result = dot->fValue;
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Dot operation result = " << result << " on Processor # " << ProcId << "\n";;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	wdot->Finish();
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Dot finished on Processor # " << ProcId << "\n";;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	return result;
}

void OOPLinAlgTasks::Dot()
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Inside " << __PRETTY_FUNCTION__ << " On Processor # " << GetProcID() << "\n";;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}

#endif
	TPZFMatrix * A = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(0));
	TPZFMatrix * B = dynamic_cast<TPZFMatrix *> (fDependRequest.ObjectPtr(1));
	OOPDouble * result =
			dynamic_cast<OOPDouble *> (fDependRequest.ObjectPtr(2));
	result->fValue = ::Dot(*A, *B);
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Leaving " << __PRETTY_FUNCTION__ << " On Processor # " << GetProcID() << "\n";;
		sout << "Computed Dot value = " << result->fValue << endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}

#endif

}

int OOPLinAlgTasks::ClassId() const {
	return OOPLINALGTASK_ID;
}

template class TPZRestoreClass<OOPLinAlgTasks, OOPLINALGTASK_ID> ;

