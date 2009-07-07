/*
 *  TPZFParMatrix.cpp
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 5/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "TPZFParMatrix.h"
#include "OOPLinAlgTasks.h"


void TPZFParMatrix::Write(TPZStream &buf, int withclassid)
{
	TPZSaveable::Write(buf, withclassid);
	m_Id.Write(buf, withclassid);
	int val = m_IsSync;
	buf.Write( &val, 1);
	m_Version.Write(buf, withclassid);
}
void TPZFParMatrix::Read(TPZStream &buf, void *context)
{
	TPZSaveable::Read(buf, context);
	m_Id.Read(buf, context);
	int val = 0;
	buf.Read(&val, 1);
	m_IsSync = val;
	m_Version.Read(buf, context);
}


TPZFParMatrix::TPZFParMatrix(const TPZFParMatrix & copy)
{
	this->m_Id = copy.m_Id;
	this->m_Version = copy.m_Version;
}


void TPZFParMatrix::SynchronizeFromRemote()
{
	TPZFParMatrix::TPZAccessParMatrix access(*this);
	TPZFMatrix * mat = dynamic_cast<TPZFMatrix *> (&access.GetMatrix());
	int i, j;
	for(i = 0; i < Rows(); i++)
	{
		for(j = 0; j < Cols(); j++)
		{
			PutVal(i,j, mat->GetVal(i, j));
		}
	}
	m_IsSync = true;
}
void TPZFParMatrix::SynchronizeFromLocal()
{
	TPZFParMatrix::TPZAccessParMatrix access(*this);
	TPZFMatrix * mat = dynamic_cast<TPZFMatrix *> (&access.GetMatrix());
	int i, j;
	for(i = 0; i < Rows(); i++)
	{
		for(j = 0; j < Cols(); j++)
		{
			mat->PutVal(i,j, GetVal(i, j));
		}
	}
	m_IsSync = true;
}


void TPZFParMatrix::ZAXPY(const REAL alpha, const TPZFParMatrix &p) 
{
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
	lintask->RemoteZAXPY(*this, alpha, p);
	m_IsSync = false;
}

void TPZFParMatrix::ZAXPY(const REAL alpha, const TPZFMatrix &p) 
{
	const TPZFParMatrix * pmat = NULL;
	pmat = dynamic_cast<const TPZFParMatrix * > (&p);
	if(pmat)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
		lintask->RemoteZAXPY(*this, alpha, *pmat);
		m_IsSync = false;
	}
	else
	{
		TPZFMatrix::ZAXPY(alpha, p);
	}
	
}


void TPZFParMatrix::TimesBetaPlusZ(const REAL beta, const TPZFParMatrix &z)
{
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
	lintask->RemoteTimesBetaPlusZ(*this, beta, z);
	m_IsSync = false;
}
void TPZFParMatrix::TimesBetaPlusZ(const REAL beta, const TPZFMatrix &z)
{
	const TPZFParMatrix * pmat = NULL;
	pmat = dynamic_cast<const TPZFParMatrix*>(&z);
	if(pmat)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
		lintask->RemoteTimesBetaPlusZ(*this, beta, *pmat);
		m_IsSync = false;
	}
	else
	{
		TPZFMatrix::TimesBetaPlusZ(beta, z);
	}
}

void TPZFParMatrix::MultAdd(const TPZFParMatrix &x,const TPZFParMatrix &y, TPZFParMatrix &z, const REAL alpha,const REAL beta,const int opt,const int stride) const 
{
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
	lintask->RemoteMultAdd(*this, x, y, z, alpha, beta, opt, stride);
}
void TPZFParMatrix::MultAdd(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z, const REAL alpha,const REAL beta,const int opt,const int stride) const 
{
	const TPZFParMatrix * px = NULL;
	const TPZFParMatrix * py = NULL;
	TPZFParMatrix * pz = NULL;

	px = dynamic_cast<const TPZFParMatrix *>(&x);
	py = dynamic_cast<const TPZFParMatrix *>(&y);
	pz = dynamic_cast<TPZFParMatrix *>(&z);
	
	if(px && py && pz)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
		lintask->RemoteMultAdd(*this, *px, *py, *pz, alpha, beta, opt, stride);
	}
	else
	{
		TPZFMatrix::MultAdd(x, y, z, alpha, beta, opt, stride);
	}
}


void TPZFParMatrix::Multiply(const TPZFParMatrix &A, TPZFParMatrix&B, int opt, int stride) const 
{
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
	lintask->RemoteMultiply(*this, A, B, opt, stride);
}
void TPZFParMatrix::Multiply(const TPZFMatrix &A, TPZFMatrix&B, int opt, int stride) const 
{
	const TPZFParMatrix * pA = NULL;
	TPZFParMatrix * pB = NULL;
	pA = dynamic_cast<const TPZFParMatrix *> (&A);
	pB = dynamic_cast<TPZFParMatrix *> (&B);
	
	if(pA && pB)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
		lintask->RemoteMultiply(*this, *pA, *pB, opt, stride);
	}
	else
	{
		TPZFMatrix::Multiply(A, B, opt, stride);
	}
}

int TPZFParMatrix::Zero()
{
	TPZFMatrix::Zero();
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(0);
	lintask->RemoteZero(*this);
/**
	int size = fRow * fCol * sizeof(REAL);
	memset(fElem,'\0',size);
*/
	fDecomposed = 0;
	m_IsSync = false;
	return( 1 );
}

int TPZFParMatrix::Redim(const int rows, const int cols)
{
	TPZMatrix::Redim(rows, cols);
	OOPLinAlgTasks * rtask = new OOPLinAlgTasks(0);
	rtask->RemoteRedim(*this, rows, cols);
	return 0;
}

TPZFMatrix & TPZFParMatrix::operator = (const TPZFParMatrix & copy)
{
	OOPLinAlgTasks * copyt = new OOPLinAlgTasks(0);
	copyt->RemoteCopy(*this, copy);
	return *this;
}


TPZFMatrix & TPZFParMatrix::operator = (const TPZFMatrix & copy)
{
	const TPZFParMatrix * parcopy = dynamic_cast<const TPZFParMatrix *>(&copy);
	if(!parcopy) exit (-1);
	OOPLinAlgTasks * copyt = new OOPLinAlgTasks(0);
	copyt->RemoteCopy(*this, *parcopy);
	return *this;
}

//Pure methods
REAL Dot(const TPZFParMatrix &A, const TPZFParMatrix &B) 
{
#ifdef SYNCHONIZED_DOT	
	TPZFParMatrix::TPZAccessParMatrix accsA(A);
	TPZFParMatrix::TPZAccessParMatrix accsB(B);
	TPZFMatrix * matA = &accsA.GetMatrix();
	TPZFMatrix * matB = &accsB.GetMatrix();
	return Dot(*matA, *matB);
#else
	OOPLinAlgTasks * dotTask = new OOPLinAlgTasks(0);
	return dotTask->RemoteDot(A, B);
#endif
};



//Methods for TPZFParMatrix::TPZAccessParMatrix:: class

TPZFParMatrix::TPZAccessParMatrix::TPZAccessParMatrix(const TPZFParMatrix & par)
{
	m_WT = new OOPWaitTask(0);
	m_WT->AddDependentData(OOPAccessTag(par.Id(), EReadAccess, par.Version(), 0));
	m_WT->Submit();
	m_WT->Wait();
}
TPZFParMatrix::TPZAccessParMatrix::TPZAccessParMatrix(TPZFParMatrix & par)
{
	m_WT = new OOPWaitTask(0);
	m_WT->AddDependentData(OOPAccessTag(par.Id(), EWriteAccess, par.Version(), 0));
	m_WT->Submit();
	m_WT->Wait();
	par.IncrementVersion();
}
TPZFParMatrix::TPZAccessParMatrix::~TPZAccessParMatrix()
{
	m_WT->Finish();
}

TPZFMatrix & TPZFParMatrix::TPZAccessParMatrix::GetMatrix()
{
	TPZFMatrix * mat =  dynamic_cast<TPZFMatrix *> (m_WT->GetDepObjPtr(0));
	return * mat;
}
#include "cg.h"
#include "pzsolve.h"

void TPZFParMatrix::SolveCG(	int & 	numiterations,
											 TPZSolver & 	preconditioner,
											 const TPZFMatrix & 	F,
											 TPZFMatrix & result,
											 TPZFMatrix * residual,
											 REAL & tol,
											 const int 	FromCurrent)
{
	const TPZFParMatrix * parF = dynamic_cast<const TPZFParMatrix *> (&F);
	TPZFParMatrix * parResult = dynamic_cast<TPZFParMatrix *>(&result);
	TPZFParMatrix * parResidual = dynamic_cast<TPZFParMatrix *>(residual);
	CG(*this, *parResult, *parF, preconditioner, parResidual, numiterations, tol, FromCurrent);
}



template class TPZRestoreClass<TPZFParMatrix, TPZFPARMATRIX_ID>;

extern OOPDataManager * DM;
extern OOPTaskManager * TM;
