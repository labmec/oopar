#ifndef OOPCOLLECTOR_HH
#define OOPCOLLECTOR_HH

#include "ooptask.h"

/**
 * @brief Enum to collector type.
 * @ingroup managerdata
 */
enum ECollectType {
	EUndefined,
	EGatherer,
	EScatterer
};

/**
 * @brief Implements a collector oopar pattern. Responsible for operations of gathering or scattering vector information throughout the parallel environment. \n
 * Most implementations of numerical methods requires some sort of gather/ scatter functionality in some part of the code.
 * @author Gustavo Longhin
 * @since 28-04-09
 * @ingroup managerdata
 */
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
	 * @brief Sets the type of collector information
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
	 * @brief Returns the type of operation the current collector is assigned to
	 */
	ECollectType GetType()
	{
		return m_Type;
	}
	/**
	 * @brief Sets the Id of the Target object to which the Gather/Scatter operations will b performed.
	 */
	void SetTargetId(OOPObjectId Id)
	{
		m_TargetId = Id;
	}
	/**
	 * @brief Returns the Id of the target object to which the Gather/Scatter operation are going to performed.
	 */
	OOPObjectId GetTargetId()
	{
		return m_TargetId;
	}
	/**
	 * @brief Sets the data to Scatter/Gather
	 */
	void SetData(TPZVec<T> & From)
	{
		m_FromVector = From;
	}
	static TPZSaveable *Restore (TPZStream & buf, void *context = 0);
private:
	/**
	 * @brief Performs the Scatter/Gather operation. \n
	 * This method is internally called by the Execute method according to what opperation to perform.
	 */
	virtual void Scatter();
	virtual void Gather();
protected:
	/**
	 * @brief Data to be scattered / gathered by the object
	 */
	TPZVec<T> m_FromVector;
	/**
	 * @brief Holds ids of the nodes where current data must Scatter to. Destination nodes.
	 */
	TPZVec<int> m_ScatterTo;
	/**
	 * @brief Holds ids of the nodes where current data must Gather to. Destinationn nodes.
	 */
	TPZVec<int> m_GatherTo;
	
	/**
	 * @brief Holds the ID of the Object the task will Gather/Scatter to
	 */
	OOPObjectId m_TargetId;
	
	/**
	 * @brief Indicates the type of operation to be performed
	 */
	ECollectType m_Type;
	
};

#endif