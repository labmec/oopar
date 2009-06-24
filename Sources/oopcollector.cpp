/**
 *  oopcollector.cpp
 *  OOPar
 *
 *  Created by Gustavo Longhin on 4/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "oopcollector.h"

template class OOPCollector<REAL>;


template class TPZRestoreClass < OOPCollector< REAL > , OOPCOLLECTOR_TASK_ID >;

template <class T>
OOPCollector< T >::OOPCollector(int ProcId) : OOPTask(ProcId)
{
	m_Type = EUndefined;
	m_ScatterTo.Resize(0);
	m_GatherTo.Resize(0);
	m_FromVector.Resize(0);
}

template <class T>
OOPCollector< T >::OOPCollector() : OOPTask()
{
	m_Type = EUndefined;
	m_ScatterTo.Resize(0);
	m_GatherTo.Resize(0);
	m_FromVector.Resize(0);
}


template <class T>
OOPCollector<T>::~OOPCollector()
{
}

template <class T>
void OOPCollector<T>::Write (TPZStream & buf, int withclassid)
{
  OOPTask::Write (buf, withclassid);
	TPZSaveable::WriteObjects(buf, m_ScatterTo);
	TPZSaveable::WriteObjects(buf, m_GatherTo);
	TPZSaveable::WriteObjects(buf, m_FromVector);
	m_TargetId.Write(buf, withclassid);
	buf.Write((int *)m_Type, 1);
}

template <class T>
void OOPCollector<T>::Read (TPZStream & buf, void *context)
{
  OOPTask::Read (buf, context);
	TPZSaveable::ReadObjects(buf, m_ScatterTo);
	TPZSaveable::ReadObjects(buf, m_GatherTo);
	TPZSaveable::ReadObjects(buf, m_FromVector);
	m_TargetId.Read(buf, context);
	buf.Read((int *)m_Type, 1);

}

template <class T>
TPZSaveable * OOPCollector<T>::Restore (TPZStream & buf, void *context)
{
  OOPCollector<T> *v = new OOPCollector<T> (0);
#warning verify context parameter here
  v->Read (buf, context);
  return v;
}

template <class T>
OOPMReturnType OOPCollector<T>::Execute()
{
	switch (m_Type)
	{
		case EScatterer:
		{
			Scatter();
		}
		break;
		case EGatherer:
		{
			Gather();
		}
		break;
		default:
		{
			std::cout << "Undefined operation type \n Bailing out !";
			exit(-1);
		}
		break;
	}
	return ESuccess;
}

template < class T>
void OOPCollector<T>::Scatter()
{
	//Gain access to the DM managed object;
	TPZVec<T> * tgVec = dynamic_cast<TPZVec<T> * > (fDependRequest.ObjectPtr(0));
	int i;
	for(i = 0; i< m_ScatterTo.NElements(); i++)
	{
		int idtg = m_ScatterTo[i];
		*tgVec[idtg] += m_FromVector[i];
	}
}

template < class T>
void OOPCollector<T>::Gather()
{
	//Gain access to the DM managed object;
	TPZVec<T> * tgVec = dynamic_cast<TPZVec<T> * > (fDependRequest.ObjectPtr(0));
	int i;
	for(i = 0; i< m_GatherTo.NElements(); i++)
	{
		*tgVec[m_GatherTo[i]] += m_FromVector[i];
	}
}

