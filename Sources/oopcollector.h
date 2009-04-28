
#include "ooptask.h"

/**
 *
 * Implements a collector oopar pattern. Responsible for operations of gathering or scattering vector information throughout the parallel environment.
 * Most implementations of numerical methods requires some sort of gather/ scatter functionality in some part of the code.
 * Created by Gustavo Longhin on 4/28/09.
 * since 28-04-09
 *
 */
template < class T>
OOPCollector : public OOPTask
{
  OOPWaitTask(int Procid);
	
  ~OOPWaitTask();
	
  virtual void Write(TPZStream & buf, int withclassid);
  virtual void Read(TPZStream & buf, void * context);
  virtual int ClassId() const
	{
		return OOPCOLLECTOR_TASK_ID;
	}
  virtual OOPMReturnType Execute();
	/**
	 * Performs the Scatter operation.
	 * This method is internally called by the Execute method according to what opperation to perform.
	 */
	void Scatter();
	void Gather();
protected:
	/**
	 * Data to be scattered / gathered by the object
	 */
	TPZVec<T> m_FromVector;
	/**
	 * Holds ids of the nodes where current data must Scatter to. Destination nodes.
	 */
	TPZVec<int> m_ScatterTo;
	/**
	 * Holds ids of the nodes where current data must Gather to. Destinationn nodes.
	 */
	TPZVec<int> m_GatherTo;

};
