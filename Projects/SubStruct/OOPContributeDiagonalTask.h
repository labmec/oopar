#ifndef OOPCONTRIBUTEDIAGONALTASKH
#define OOPCONTRIBUTEDIAGONALTASKH


/*
 *  OOPContributeDiagonalTask.h
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <ooptask.h>

#define OOPCONTRIBUTEDIAGONALTASK_ID 272702


/**
 * It carries the contribution computed on each processor to the main processor.
 * It is triggered by the ComputeDiagTask.
 */

class OOPContributeDiagonalTask : public OOPTask
	{
	public:
		OOPContributeDiagonalTask(int processor);
		OOPContributeDiagonalTask()
		{}
	private:
		TPZVec<double> m_Values;
		TPZVec<int> m_Indices;
		
	public:
		/**
		 * Serialization methods
		 */
		virtual void Write(TPZStream & buf, int withclassid);
		virtual void Read(TPZStream & buf, void * context);
		
		virtual OOPMReturnType Execute();
		
		int ClassId()
		{
			return OOPCONTRIBUTEDIAGONALTASK_ID;
		}
	};

#endif