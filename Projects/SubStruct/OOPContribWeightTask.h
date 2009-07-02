#ifndef OOPCONTRIBWEIGHTTASKH
#define OOPCONTRIBWEIGHTTASKH

/*
 *  OOPContribWeightTask.h
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "ooptask.h"
#include "pzvec.h"

#define OOPCONTRIBWEIGHTTASK_ID 272703

class OOPContribWeightTask : public OOPTask
{
public:
	OOPContribWeightTask(int processor);
	OOPContribWeightTask();
private:
	TPZVec<double> m_Values;
	
public:
	/**
	 * Serialization methods
	 */
	virtual void Write(TPZStream & buf, int withclassid);
	virtual void Read(TPZStream & buf, void * context);
	
	virtual OOPMReturnType Execute();
	
	int ClassId()
	{
		return OOPCONTRIBWEIGHTTASK_ID;
	}
	
};


#endif