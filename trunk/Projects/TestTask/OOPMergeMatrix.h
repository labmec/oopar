#ifndef OOPMRGMAT_H
#define OOPMRGMAT_H

/*
 *  OOPMergeMatrix.h
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "ooptask.h"
#include "pzsave.h"
#include "pzfmatrix.h"

class OOPSubMatrix;

#define OOPMERGETASK_ID 1004

class   OOPMergeMatrix :public OOPTask
{
	/**
	 * For the K.u = f multiplication, holds the f vector
	 */
	TPZFMatrix<REAL> m_Vector;
	
public:
	int m_SubId;
	
	virtual ~OOPMergeMatrix(){}
	
	OOPMergeMatrix()
	{
		m_SubId = -1;
	} 
	OOPMergeMatrix(int ProcId) : OOPTask(ProcId)
	{
		m_SubId = -1;
	} 
	virtual OOPMReturnType Execute (); 
	
	virtual int ClassId () const
	{
		return OOPMERGETASK_ID; 
	}
	void Write (TPZStream & buf,int withclassid);
	void Read (TPZStream & buf, void * context);
	
	void SetFVector(TPZFMatrix<REAL> vector)
	{
		m_Vector = vector;
	}
};

extern ofstream MergeLog;

#endif