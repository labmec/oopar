// -*- c++ -*-
/* Generated by Together */

#ifndef TLOCALCOMPUTE_H
#define TLOCALCOMPUTE_H
#include "ooptask.h"
class TPartitionRelation;
class TContribution;

/**
 * Implements the local flux computation on each partition.
 * Objects of this class are created by TParCompute object which also submits them to the TaskManager.
 * Each TLocalCompute is a OOPTask dependent on the following objects:
 * --One State objects with read access.
 * --One Rhs objects with write access.
 * --The TPartitionRelation object with read access.
 * @author Gustavo Camargo Longhin
 * @version 0.9 
 */
class TLocalCompute : public OOPTask {
public:    

	/**
     * Simple constructor 
     */
    TLocalCompute(int ProcId, int partition);

    /**
	* Execute the task, verifying that all needed data acesses are satisfied.
	* The TParCompute is OOPTask descendent class and this method provides the access by the TaskManager.
	* @since 02/06/2003
	*/
	virtual OOPMReturnType Execute ();    

    void ComputeFrontierFluxes();

    /**
     * sets the ids of the right hand sides which take part in the flux computation
     */
    void SetRhsIds(vector<OOPObjectId> &rhsids, OOPDataVersion &rhsversion);

    /**
     * Generates the tasks for transmitting the local computation for its neighbor partitions. 
     */
    void TransmitFLuxes(TContribution &);

    void ComputeLocalFluxes();
private:    

  /**
   * Partition number to which this object corresponds
   */
  int fPartition;
    /**
     * pointer to the PartitionRelation object (valid during execute)
     */
    TPartitionRelation * fPartRelationPtr;

    /**
     * Ids of the right hand sides
     */
    vector<OOPObjectId> fRhsIds;
    /**
     * version of the right hand side to which the current task will contribute
     */
    OOPDataVersion fRhsVersion;
    /**
     * Within the Execute Method we should be able to get the data pointer
     */
    void InitializePartitionRelationPointer();
  /**
   * Computes the version upon which the communication tasks will depend
   */
  OOPDataVersion GetCommunicationVersion();

};
#endif //TLOCALCOMPUTE_H
