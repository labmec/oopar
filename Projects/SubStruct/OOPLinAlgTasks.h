#ifndef OOPLINALGTASK_H
#define OOPLINALGTASK_H

/*
 *  OOPLinAlgTasks.h
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 5/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#include "ooptask.h"


#define OOPLINALGTASK_ID 272704



enum ELinAlgOpType {
	EZAXPY,
	ETimesBetaPlusZ,
	EMultAdd,
	EMultiply,
	EZero,
	ECopyOperator,
	ERedim,
	EDot,
	EUndefined
};

class TPZFParMatrix;

/**
 * Implements basic Linear Algebra operations on the OOPar distributed environment.
 * Variables used in the operations are submitted according to the order of apearence on the Serial call to
 * the method being implemented.
 * For example:
 * A call to x.ZAXPY(alpha,p) will have the variables involved submitted in the following order:
 * 0 - x, which will be the translated in a simple dependency request to distributed part of the caller object
 * 1 - alpha , which will be a OOPDouble object submitted uppon entrance of the method.
 * 2 - p, which also will be translated in the dependency request to the p object involved.
 * During execution of the task object pointers will be obtained according that sequence of dependencies requests.
 */

class OOPLinAlgTasks : public OOPTask
{
public:
	OOPLinAlgTasks(int processor);
	OOPLinAlgTasks();
private:
	ELinAlgOpType m_Type;
public:
	/**
	 * Sets the type of operation to be performed
	 */
	void SetType(ELinAlgOpType theType);
	/**
	 * Returns the type of operation which will be performed on the Execute method
	 */
	ELinAlgOpType GetType();
	/**
	 * Serialization methods
	 */
	virtual void Write(TPZStream & buf, int withclassid);
	virtual void Read(TPZStream & buf, void * context);
		
	virtual OOPMReturnType Execute();
	
	void RemoteZAXPY(TPZFParMatrix & x, REAL beta, const TPZFParMatrix & z);
	void ZAXPY();

	void RemoteTimesBetaPlusZ(TPZFParMatrix & x, const REAL beta, const TPZFParMatrix & z);
	void TimesBetaPlusZ();

	void RemoteMultAdd(const TPZFParMatrix & me, const TPZFParMatrix &x,const TPZFParMatrix &y, TPZFParMatrix &z, const REAL alpha,const REAL beta,const int opt, const int stride);
	void MultAdd();
	
	void RemoteMultiply(const TPZFParMatrix & me, const TPZFParMatrix &A, TPZFParMatrix&B, int opt, int stride);
	void Multiply();
	
	void RemoteZero(TPZFParMatrix & me);
	void Zero();
	
	void RemoteCopy(TPZFParMatrix & me, const TPZFParMatrix & copy);
	void Copy();

	void RemoteRedim(TPZFParMatrix & me, int rows, int cols);
	void Redim();
	
	REAL RemoteDot(const TPZFParMatrix & A, const TPZFParMatrix & B);
	void Dot();
	
	
	virtual int ClassId() const;
};

#endif