
#include "ooptask.h"

/**
 *
 * Implements a collector oopar pattern. Responsible for operations of gathering or scattering vector information throughout the parallel environment.
 * Most implementations of numerical methods requires some sort of gather/ scatter functionality in some part of the code.
 * Created by Gustavo Longhin on 4/28/09.
 * since 28-04-09
 *
 */

enum ECollectType {
	EUndefined,
	EGatherer,
	EScatterer
};


template < class T>
class OOPCollector : public OOPTask
{
public:
  OOPCollector(int Procid);
	OOPCollector();
	
  ~OOPCollector();
	
  virtual void Write(TPZStream & buf, int withclassid);
  virtual void Read(TPZStream & buf, void * context);
  virtual int ClassId() const
	{
		return OOPCOLLECTOR_TASK_ID;
	}
  virtual OOPMReturnType Execute();
	/**
	 * Sets the type of collector information
	 */
	void SetAsScatterer()
	{
		m_Type = EScatterer;
	}
	void SetAsGatherer()
	{
		m_Type = EGatherer;
	}
	/**
	 * Returns the type of operation the current collector is assigned to
	 */
	ECollectType GetType()
	{
		return m_Type;
	}
	/**
	 * Sets the Id of the Target object to which the Gather/Scatter operations will b performed.
	 */
	void SetTargetId(OOPObjectId Id)
	{
		m_TargetId = Id;
	}
	/**
	 * Returns the Id of the target object to which the Gather/Scatter operation are going to performed.
	 */
	OOPObjectId GetTargetId()
	{
		return m_TargetId;
	}
	/**
	 * Sets the data to Scatter/Gather
	 */
	void SetData(TPZVec<T> & From)
	{
		m_FromVector = From;
	}
	static TPZSaveable *Restore (TPZStream & buf, void *context = 0);
private:
	/**
	 * Performs the Scatter/Gather operation.
	 * This method is internally called by the Execute method according to what opperation to perform.
	 */
	virtual void Scatter();
	virtual void Gather();
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
	
	/**
	 * Holds the ID of the Object the task will Gather/Scatter to
	 */
	OOPObjectId m_TargetId;

	/**
	 * Indicates the type of operation to be performed
	 */
	ECollectType m_Type;
	
};


