// -*- c++ -*-
/* Generated by Together */

#ifndef TPARCOMPUTE_H
#define TPARCOMPUTE_H
#include "ooptask.h"
#include "oopobjectid.h"
#include "TPartitionRelation.h"
#include <vector>
using namespace std;

class TLocalCompute;

/**
 * Implements the unique task which triggers the parallel flux computations.
 * Is responsible for assigning partitions and all its necessary data for a independent flux computation to different processors.
 * Hierarchically, this class are on the highest level on the parallel environment.
 */
class TParCompute : public OOPTask {
public:

  /**
   * Constructor for the task
   * @param procid : processor where the task should execute
   * @param numpartitions : number of partitions of the mesh
   */
    TParCompute(int procid, int numpartitions);

	/**
     * Appends one Rhs ID to the fRhsId vector
     */
	void SetRhsId(OOPObjectId & Id);
	/**
     * Appends one StateID to the fRhsId vector
     */
	void SetStateId(OOPObjectId & Id);
	/**
     * Sets the whole Rhs ID vector
     */
    void SetRhsId(vector<OOPObjectId> & Id);

	/**
     * Sets the whole State ID vector
     */
    void SetStateId(vector<OOPObjectId> & Idp0);

    /**
     * Retrieves the OOPObjectId from the �ith� element on the fStateIds vector.
     * @param pos Index of the enquired partition
     * @since 03/06/2002
     */
    OOPObjectId &GetStateId(int pos);

    /**
     * Retrieves the OOPObjectId for the �ith� element on the fRhsIds vector.
     * @param pos Index of the enquired partition
     * @since 03/06/2003
     */
    OOPObjectId &GetRhsId(int pos);

    /**
     * Sets the Id of the Partition Relation object
     * @param Id Id of the TPartitionRelation object
     * @since 03/06/2003
     */
    void SetPartitionRelationId(OOPObjectId & Id, OOPDataVersion &version);

    /**
     * Returns the Id of the TPartitionRelation object.
     * @since 03/06/2003
     */
    OOPObjectId GetPartitionRelationId();

private:
    /**
     * Within the Execute Method we should be able to get the data pointer
     */
     void InitializePartitionRelationPointer();

public:
    /**
     * Set the version of the state and rhs upon which the created tasks will depend
     */
     void SetVersionDependency(OOPDataVersion &version);

     /**
      * In this method the data dependencies of the current task will be initialized
      */
      void InitializeTaskDependencies();

     /**
      * During the execution of this method, the flux tasks will be created
      */

     OOPMReturnType Execute();
private:

    /**
     * Create tasks responsible for the computation of fluxes on independent partitions.
     * Partitios are obtained from MeTiS application on the original mesh.
     * Since the ParCompResidual is a recurrent task, the task creation will actually create the tasks only in its first run, on subsequent runs it will only actuate on the subtasks data dependence.
     * @since 03/06/2003
     */
    void CreateFluxesTasks(  );

    /**
     * Initializes the DataVersion object according to amount of contributions
     * from neighbor partitions
     * @since 03/06/2003
     */
    void InitializeSolutionVersion();

private:


    /**
     * Holds all Ids from Flux objects in the parallel computation
     */
    vector<OOPObjectId> fRhsIds;

    /**
     * Holds all Ids from State variables in the parallel computation
     */
    vector<OOPObjectId> fStateIds;

    /**
     * pointer to the PartitionRelation object (valid during execute)
     */
    TPartitionRelation *fPartRelationPtr;
    /**
     * Holds the Id of the Partition Relation structure.
     * @since 03/06/2003
     */
    OOPObjectId fPartRelationId;
    
    /**
     * The version of Partition Relation
     */
    OOPDataVersion fPartRelationVersion;
    /**
     * Holds the Ids of sub tasks created in the process.
     * @since 03/06/2003
     */
    vector<OOPObjectId> fTaskIds;

    /**
     * Holds the version of the solution and data upon which the tasks will depend
     */
     OOPDataVersion fDataVersions;

     /**
      * Holds the TLocalCompute poiters.
      * In the case of recurrence tasks this is populated only in the first run.
      * @since 04/06/2003 
      */
     TLocalCompute * fTasks;

     /**
      * Number of partitions 
      */
     int fNPartitions;
};
#endif //TPARCOMPUTE_H
