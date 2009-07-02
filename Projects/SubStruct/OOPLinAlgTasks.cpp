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




OOPLinAlgTasks::OOPLinAlgTasks(int processor) : OOPTask(processor)
{
	m_Type = EUndefined;
}
OOPLinAlgTasks::OOPLinAlgTasks() : OOPTask()
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
	buf.Write((int*)&m_Type, 1);
}
void OOPLinAlgTasks::Read(TPZStream & buf, void * context)
{
	OOPTask::Read(buf, context);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
	buf.Read((int*)&m_Type, 1);
}

OOPMReturnType OOPLinAlgTasks::Execute()
{
	switch (m_Type) 
	{
		case EZAXPY:
		{
			ZAXPY();
			break;
		}
		case ETimesBetaPlusZ:
		{
			TimesBetaPlusZ();
			break;
		}
		case EMultAdd:
		{
			MultAdd();
			break;
		}
		case EMultiply:
		{
			Multiply();
			break;
		}
		case EZero:
		{
			Zero();
			break;
		}
		case ECopyOperator:
		{
			Copy();
			break;
		}
		case ERedim:
		{
			Redim();
			break;
		}
		case EDot:
		{
			Dot();
			break;
		}
		default:
			break;
	}
	return ESuccess;
}
void OOPLinAlgTasks::RemoteTimesBetaPlusZ(TPZFParMatrix & x, const REAL beta, const TPZFParMatrix & z)
{
	this->SetType(ETimesBetaPlusZ);
	this->AddDependentData(OOPAccessTag(x.Id(), EWriteAccess, x.Version(),0));
	OOPDouble * dbeta = new OOPDouble;
	dbeta->fValue = beta;
	OOPObjectId betaId = DM->SubmitObject(dbeta);
	OOPDataVersion betaVersion;
	this->AddDependentData(OOPAccessTag(betaId, EReadAccess, betaVersion, 0));
	this->AddDependentData(OOPAccessTag(z.Id(), EReadAccess, z.Version(),0));
	x.IncrementVersion();
	this->Submit();
	
}
void OOPLinAlgTasks::RemoteZAXPY(TPZFParMatrix & x, REAL beta, const TPZFParMatrix & z)
{
	this->SetType(EZAXPY);
	this->AddDependentData(OOPAccessTag(x.Id(), EWriteAccess, x.Version(),0));
	OOPDouble * dbeta = new OOPDouble;
	dbeta->fValue = beta;
	OOPObjectId betaId = DM->SubmitObject(dbeta);
	OOPDataVersion betaVersion;
	this->AddDependentData(OOPAccessTag(betaId, EReadAccess, betaVersion, 0));
	this->AddDependentData(OOPAccessTag(z.Id(), EReadAccess, z.Version(),0));
	x.IncrementVersion();
	this->Submit();
}

void OOPLinAlgTasks::ZAXPY()
{
	//Implements x.ZAXPY(alpha, p);
	TPZFMatrix * x = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	OOPDouble * alpha = dynamic_cast<OOPDouble * > (fDependRequest.ObjectPtr(1));
	TPZFMatrix * p = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(2));
	x->ZAXPY(alpha->fValue, *p);
}

void OOPLinAlgTasks::TimesBetaPlusZ()
{
	//Implements p.TimesBetaPlusZ(beta,z);
	TPZFMatrix * p = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	OOPDouble * beta = dynamic_cast<OOPDouble * > (fDependRequest.ObjectPtr(1));
	TPZFMatrix * z = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(2));
	p->TimesBetaPlusZ(beta->fValue, *z);
}

void OOPLinAlgTasks::RemoteMultAdd(const TPZFParMatrix & me, const TPZFParMatrix & x,const TPZFParMatrix &y, 
																	 TPZFParMatrix &z, const REAL alpha,const REAL beta,const int opt,
																	 const int stride)
{
	//It triggers the task for the distributed operation
	this->SetType(EMultAdd);
	this->AddDependentData(OOPAccessTag(me.Id(), EReadAccess, me.Version(), 0));
	this->AddDependentData(OOPAccessTag(x.Id(), EReadAccess, x.Version(), 0));
	this->AddDependentData(OOPAccessTag(y.Id(), EReadAccess, y.Version(), 0));
	this->AddDependentData(OOPAccessTag(z.Id(), EWriteAccess, z.Version(), 0));
	
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
	
	this->AddDependentData(OOPAccessTag(alphaId, EReadAccess, theVersion, 0));
	this->AddDependentData(OOPAccessTag(betaId, EReadAccess, theVersion, 0));
	this->AddDependentData(OOPAccessTag(optId, EReadAccess, theVersion, 0));
	this->AddDependentData(OOPAccessTag(strideId, EReadAccess, theVersion, 0));
	
	this->Submit();
}

void OOPLinAlgTasks::MultAdd()
{
	//Implements me.MultAdd(x,b,r,-1.,1.);

	TPZFMatrix * me = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	TPZFMatrix * x = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(1));
	TPZFMatrix * y = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(2));
	TPZFMatrix * z = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(3));
	
	OOPDouble * alpha = dynamic_cast<OOPDouble * > (fDependRequest.ObjectPtr(4));
	OOPDouble * beta = dynamic_cast<OOPDouble * > (fDependRequest.ObjectPtr(5));
	OOPInt * opt = dynamic_cast<OOPInt * > (fDependRequest.ObjectPtr(6));
	OOPInt * stride = dynamic_cast<OOPInt * > (fDependRequest.ObjectPtr(7));

	me->MultAdd(*x, *y, *z, alpha->fValue, beta->fValue, opt->fValue, stride->fValue);
}

void OOPLinAlgTasks::RemoteMultiply(const TPZFParMatrix & me, const TPZFParMatrix &A, TPZFParMatrix&B, int opt, int stride)
{
	this->SetType(EMultiply);
	this->AddDependentData(OOPAccessTag(me.Id(), EReadAccess, me.Version(), 0));
	this->AddDependentData(OOPAccessTag(A.Id(), EReadAccess, A.Version(), 0));
	this->AddDependentData(OOPAccessTag(B.Id(), EWriteAccess, B.Version(), 0));
	
	B.IncrementVersion();
	
	OOPDataVersion theVersion;
	
	OOPInt * dopt = new OOPInt;
	dopt->fValue = opt;
	OOPObjectId optId = DM->SubmitObject(dopt);
	
	OOPInt * dstride = new OOPInt;
	dstride->fValue = stride;
	OOPObjectId strideId = DM->SubmitObject(dstride);
	
	this->AddDependentData(OOPAccessTag(optId, EReadAccess, theVersion, 0));
	this->AddDependentData(OOPAccessTag(strideId, EReadAccess, theVersion, 0));
	
	this->Submit();
}

void OOPLinAlgTasks::Multiply()
{
	TPZFMatrix * me = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	TPZFMatrix * A = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(1));
	TPZFMatrix * B = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(2));

	OOPInt * opt = dynamic_cast<OOPInt * > (fDependRequest.ObjectPtr(3));
	OOPInt * stride = dynamic_cast<OOPInt * > (fDependRequest.ObjectPtr(4));
	
	me->Multiply(*A, *B, opt->fValue, stride->fValue);
	
}

void OOPLinAlgTasks::RemoteZero(TPZFParMatrix & me)
{
	this->SetType(EZero);
	this->AddDependentData(OOPAccessTag(me.Id(), EWriteAccess, me.Version(), 0));
	this->Submit();
	me.IncrementVersion();
}
void OOPLinAlgTasks::Zero()
{
	TPZFMatrix * me = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
  me->Zero();	
}

void OOPLinAlgTasks::RemoteCopy(TPZFParMatrix & me, const TPZFParMatrix & copy)
{
	this->AddDependentData(OOPAccessTag(me.Id(), EWriteAccess, me.Version(), 0));
	this->AddDependentData(OOPAccessTag(copy.Id(), EReadAccess, copy.Version(), 0));
	this->SetType(ECopyOperator);
	this->Submit();
	me.IncrementVersion();
}
void OOPLinAlgTasks::Copy()
{
	TPZFMatrix * me = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	TPZFMatrix * copy = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(1));
	*me = *copy;
}

void OOPLinAlgTasks::RemoteRedim(TPZFParMatrix & me, int rows, int cols)
{
	this->AddDependentData(OOPAccessTag(me.Id(), EWriteAccess, me.Version(), 0));
	OOPInt * drows = new OOPInt;
	drows->fValue = rows;
	OOPObjectId rowsId = DM->SubmitObject(drows);
	
	OOPInt * dcols = new OOPInt;
	dcols->fValue = cols;
	OOPObjectId colsId = DM->SubmitObject(dcols);
	OOPDataVersion dv;
	
	this->AddDependentData(OOPAccessTag(rowsId, EWriteAccess, dv, 0));
	this->AddDependentData(OOPAccessTag(colsId, EWriteAccess, dv, 0));
	
	this->SetType(ERedim);
	this->Submit();
	me.IncrementVersion();
}
void OOPLinAlgTasks::Redim()
{
	TPZFMatrix * me = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	OOPInt * rows = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(1));
	OOPInt * cols = dynamic_cast<OOPInt *> (fDependRequest.ObjectPtr(2));
	me->Redim(rows->fValue, cols->fValue);
}

REAL OOPLinAlgTasks::RemoteDot(const TPZFParMatrix & A, const TPZFParMatrix & B)
{
	this->AddDependentData(OOPAccessTag(A.Id(), EReadAccess, A.Version(), 0));
	this->AddDependentData(OOPAccessTag(B.Id(), EReadAccess, B.Version(), 0));
	OOPDouble * dotval = new OOPDouble;
	dotval->fValue = 0;
	OOPObjectId dotId = DM->SubmitObject(dotval);
	OOPDataVersion dv;
	
	this->AddDependentData(OOPAccessTag(dotId, EWriteAccess, dv, 0));
	
	this->SetType(EDot);
	this->Submit();
	
	//Submit wait task which will bring the dot result back to the main thread
	dv++;
	OOPWaitTask * wdot = new OOPWaitTask(0);
	wdot->AddDependentData(OOPAccessTag(dotId, EReadAccess, dv, 0));
	wdot->Submit();
	wdot->Wait();
	OOPDouble * dot = dynamic_cast<OOPDouble *> (wdot->GetDepObjPtr(0));
	REAL result = dot->fValue;
	wdot->Finish();
	return result;
}

void OOPLinAlgTasks::Dot()
{
	TPZFMatrix * A = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(0));
	TPZFMatrix * B = dynamic_cast<TPZFMatrix * > (fDependRequest.ObjectPtr(1));
	OOPDouble * result = dynamic_cast<OOPDouble * > (fDependRequest.ObjectPtr(2));
	result->fValue = ::Dot(*A, *B);
}

int OOPLinAlgTasks::ClassId() const
{
	return OOPLINALGTASK_ID;
}


template class TPZRestoreClass<OOPLinAlgTasks, OOPLINALGTASK_ID>;

