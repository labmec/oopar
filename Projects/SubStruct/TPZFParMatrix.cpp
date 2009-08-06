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

#include "pzlog.h"
#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif

const int TARGET_PROC = 1;

void TPZFParMatrix::Write(TPZStream &buf, int withclassid)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__ ;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	TPZSaveable::Write(buf, withclassid);
	m_Id.Write(buf, withclassid);
	int val = m_IsSync;
	buf.Write( &val, 1);
	m_Version.Write(buf, withclassid);
}
void TPZFParMatrix::Read(TPZStream &buf, void *context)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	TPZSaveable::Read(buf, context);
	m_Id.Read(buf, context);
	int val = 0;
	buf.Read(&val, 1);
	m_IsSync = val;
	m_Version.Read(buf, context);
}


TPZFParMatrix::TPZFParMatrix(const TPZFParMatrix & copy)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "TPZFParMatrix copy Constructor\n" ;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif

	this->m_Id = copy.m_Id;
	this->m_Version = copy.m_Version;
}


void TPZFParMatrix::SynchronizeFromRemote()
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
	lintask->RemoteZAXPY(*this, alpha, p);
	m_IsSync = false;
}

void TPZFParMatrix::ZAXPY(const REAL alpha, const TPZFMatrix &p) 
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	const TPZFParMatrix * pmat = NULL;
	pmat = dynamic_cast<const TPZFParMatrix * > (&p);
	if(pmat)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
	lintask->RemoteTimesBetaPlusZ(*this, beta, z);
	m_IsSync = false;
}
void TPZFParMatrix::TimesBetaPlusZ(const REAL beta, const TPZFMatrix &z)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	const TPZFParMatrix * pmat = NULL;
	pmat = dynamic_cast<const TPZFParMatrix*>(&z);
	if(pmat)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
	lintask->RemoteMultAdd(*this, x, y, z, alpha, beta, opt, stride);
}
void TPZFParMatrix::MultAdd(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z, const REAL alpha,const REAL beta,const int opt,const int stride) const 
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	const TPZFParMatrix * px = NULL;
	const TPZFParMatrix * py = NULL;
	TPZFParMatrix * pz = NULL;

	px = dynamic_cast<const TPZFParMatrix *>(&x);
	py = dynamic_cast<const TPZFParMatrix *>(&y);
	pz = dynamic_cast<TPZFParMatrix *>(&z);
	
	if(px && py && pz)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
		lintask->RemoteMultAdd(*this, *px, *py, *pz, alpha, beta, opt, stride);
	}
	else
	{
		TPZFMatrix::MultAdd(x, y, z, alpha, beta, opt, stride);
	}
}


void TPZFParMatrix::Multiply(const TPZFParMatrix &A, TPZFParMatrix&B, int opt, int stride) const 
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
	lintask->RemoteMultiply(*this, A, B, opt, stride);
}
void TPZFParMatrix::Multiply(const TPZFMatrix &A, TPZFMatrix&B, int opt, int stride) const 
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	const TPZFParMatrix * pA = NULL;
	TPZFParMatrix * pB = NULL;
	pA = dynamic_cast<const TPZFParMatrix *> (&A);
	pB = dynamic_cast<TPZFParMatrix *> (&B);
	
	if(pA && pB)
	{
		OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
		lintask->RemoteMultiply(*this, *pA, *pB, opt, stride);
	}
	else
	{
		TPZFMatrix::Multiply(A, B, opt, stride);
	}
}

int TPZFParMatrix::Zero()
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Entering " << __PRETTY_FUNCTION__ << std::endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	TPZFMatrix::Zero();
	OOPLinAlgTasks * lintask = new OOPLinAlgTasks(TARGET_PROC);
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	TPZMatrix::Redim(rows, cols);
	OOPLinAlgTasks * rtask = new OOPLinAlgTasks(TARGET_PROC);
	rtask->RemoteRedim(*this, rows, cols);
	return 0;
}

TPZFMatrix & TPZFParMatrix::operator = (const TPZFParMatrix & copy)
{
	OOPLinAlgTasks * copyt = new OOPLinAlgTasks(TARGET_PROC);
	copyt->RemoteCopy(*this, copy);
	return *this;
}


TPZFMatrix & TPZFParMatrix::operator = (const TPZFMatrix & copy)
{
	const TPZFParMatrix * parcopy = dynamic_cast<const TPZFParMatrix *>(&copy);
	if(!parcopy) exit (-1);
	OOPLinAlgTasks * copyt = new OOPLinAlgTasks(TARGET_PROC);
	copyt->RemoteCopy(*this, *parcopy);
	return *this;
}

//Pure methods
REAL Dot(const TPZFParMatrix &A, const TPZFParMatrix &B) 
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
#ifdef SYNCRHONIZED_DOT
	TPZFParMatrix::TPZAccessParMatrix accsA(A);
	TPZFParMatrix::TPZAccessParMatrix accsB(B);
	TPZFMatrix * matA = &accsA.GetMatrix();
	TPZFMatrix * matB = &accsB.GetMatrix();
	return Dot(*matA, *matB);
#else
	return OOPLinAlgTasks::RemoteDot(A, B, TARGET_PROC);
#endif
};



//Methods for TPZFParMatrix::TPZAccessParMatrix:: class

TPZFParMatrix::TPZAccessParMatrix::TPZAccessParMatrix(const TPZFParMatrix & par)
{
	m_WT = new OOPWaitTask(DM->GetProcID());
	m_WT->AddDependentData(OOPAccessTag(par.Id(), EReadAccess, par.Version(), DM->GetProcID()));
	DM->TM()->Submit(m_WT);
	m_WT->Wait();
}
TPZFParMatrix::TPZAccessParMatrix::TPZAccessParMatrix(TPZFParMatrix & par)
{
	m_WT = new OOPWaitTask(DM->GetProcID());
	m_WT->AddDependentData(OOPAccessTag(par.Id(), EWriteAccess, par.Version(), DM->GetProcID()));
	DM->TM()->Submit(m_WT);
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Calling " << __PRETTY_FUNCTION__ << std::endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif
	const TPZFParMatrix * parF = dynamic_cast<const TPZFParMatrix *> (&F);
	TPZFParMatrix * parResult = dynamic_cast<TPZFParMatrix *>(&result);
	TPZFParMatrix * parResidual = dynamic_cast<TPZFParMatrix *>(residual);
	CG(*this, *parResult, *parF, preconditioner, parResidual, numiterations, tol, FromCurrent);
}

REAL Norm(const TPZFParMatrix &A)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Calling " << __PRETTY_FUNCTION__ << std::endl;
		LOGPZ_DEBUG(logger, sout.str());
		std::cout << sout.str().c_str() << std::endl;
		std::cout.flush();
	}
#endif

	return sqrt(Dot(A,A));
}


template class TPZRestoreClass<TPZFParMatrix, TPZFPARMATRIX_ID>;

extern OOPDataManager * DM;
//extern OOPTaskManager * TM;
