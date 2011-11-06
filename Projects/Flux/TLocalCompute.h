// -*- c++ -*-
/* Generated by Together */
#ifndef TLOCALCOMPUTE_H
#define TLOCALCOMPUTE_H
#include "ooptask.h"
#include "fluxdefs.h"

class OOPStorageBuffer;
class TPartitionRelation;
class TContribution;

/**
 * @brief Implements the local flux computation on each partition.
 *
 * Objects of this class are created by TParCompute object which also submits them to the TaskManager.
 * @note Each TLocalCompute is a OOPTask dependent on the following objects: \n
 * --One State objects with read access.\n
 * --One Rhs objects with write access.\n
 * --The TPartitionRelation object with read access.
 * @author Gustavo Camargo Longhin
 * @version 0.9 
 */
class TLocalCompute : public OOPTask
{

public:
	/**
	 * @brief Default destructor.
	 */
	virtual ~TLocalCompute() { }
	/**
	 * @brief Default constructor.
	 */
	TLocalCompute() { }

	/**
     * @brief Simple constructor
	 * @param ProcId Id of the processor
	 * @param partition Partition number to which this object corresponds
     */
	TLocalCompute (int ProcId, int partition);
    /**
	* Execute the task, verifying that all needed data acesses are satisfied.
	* The TParCompute is OOPTask descendent class and this method provides the access by the TaskManager.
	* @since 02/06/2003
	*/
	virtual OOPMReturnType Execute ();
	void    ComputeFrontierFluxes ();
    /**
     * sets the ids of the right hand sides which take part in the flux computation
     */
	void    SetRhsIds (vector < OOPObjectId > &rhsids,
			   OOPDataVersion & rhsversion);
    /**
     * Generates the tasks for transmitting the local computation for its neighbor partitions. 
     */
	void    TransmitFLuxes ();
	void    ComputeLocalFluxes ();
	virtual int ClassId () const
	{
		return TLOCALCOMPUTE_ID;
	}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
	virtual void Write (TPZStream & buf,int withclassid=0);
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
	virtual void Read (TPZStream & buf, void * context = 0);
	static TPZSaveable *Restore (TPZStream & buf, void * context = 0);
      private:
  /**
   * Partition number to which this object corresponds
   */
	int     fPartition;
    /**
     * pointer to the PartitionRelation object (valid during execute)
     */
	TPartitionRelation *fPartRelationPtr;
    /**
     * Ids of the right hand sides
     */
	        vector < OOPObjectId > fRhsIds;
    /**
     * version of the right hand side to which the current task will contribute
     */
	OOPDataVersion fRhsVersion;
    /**
     * Within the Execute Method we should be able to get the data pointer
     */
	void    InitializePartitionRelationPointer ();
};

extern ofstream TaskLog;

#endif // TLOCALCOMPUTE_H
