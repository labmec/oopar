#ifndef OOPCOMPUTEMATRICESTASKH
#define OOPCOMPUTEMATRICESTASKH

/*
 *  OOPComputeMatricesTask.h
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#define OOPCOMPUTEMATRICESTASK_ID 272701

#include <ooptask.h>

/**
 * This task computes the diagonal contribution from all distributed nodes, and submit tasks which will contribute on the main diagonal on the processor ZERO.
 * Submission of such tasks are performed in the Execute() method.
 */

class OOPComputeMatricesTask : public OOPTask
{
public:
	OOPComputeMatricesTask(int processor);
	OOPComputeMatricesTask();
private:
	/**
	 * Holds the Id of the main Diagonal object, to which this object will generate tasks to contribution.
	 */
	OOPObjectId m_MainDiagId;
	
public:
	/**
	 * Sets the Id of the main Diagonal object to which tasks will triggered to contribute to.
	 */
	void SetMainDiagId(OOPObjectId & Id);
	/**
	 * Serialization methods
	 */
	virtual void Write(TPZStream & buf, int withclassid);
	virtual void Read(TPZStream & buf, void * context);
	
	virtual OOPMReturnType Execute();
	
	int ClassId()
	{
		return OOPCOMPUTEMATRICESTASK_ID;
	}
};

#endif