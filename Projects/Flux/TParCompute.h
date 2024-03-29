// -*- c++ -*-
/* Generated by Together */
#ifndef TPARCOMPUTE_H
#define TPARCOMPUTE_H

#include "ooptask.h"
#include "oopobjectid.h"
#include "TPartitionRelation.h"
#include "fluxdefs.h"

#include <vector>

class OOPStorageBuffer;
using namespace std;
class   TLocalCompute;

/**
 * @brief Implements the unique task which triggers the parallel flux computations. \n
 * Is responsible for assigning partitions and all its necessary data for a independent flux computation to different processors.
 * Hierarchically, this class are on the highest level on the parallel environment.
 */
class   TParCompute : public OOPTask
{
public:
	virtual ~TParCompute(){}
	TParCompute();
	/**
	 * @brief Constructor for the task
	 * @param procid : processor where the task should execute
	 * @param numpartitions : number of partitions of the mesh
	 */
	TParCompute (int procid, int numpartitions);
	/**
	 * @brief Sets the whole Rhs ID vector
	 */
	void    SetRhsId (vector < OOPObjectId > &Id,
					  OOPDataVersion & rhsversion);
	/**
	 * @brief Sets the whole State ID vector
	 */
	void    SetStateId (vector < OOPObjectId > &Idp0,
						OOPDataVersion & stateversion);
	/**
	 * @brief Sets the whole Mesh ID vector
	 */
	void    SetMeshId (vector < OOPObjectId > &meshid,
					   OOPDataVersion & meshversion);
	/**
	 * @brief Retrieves the OOPObjectId from the �ith� element on the fStateIds vector.
	 * @param pos Index of the enquired partition
	 * @since 03/06/2002
	 */
	OOPObjectId & GetStateId (int pos);
	/**
	 * @brief Retrieves the OOPObjectId for the �ith� element on the fRhsIds vector.
	 * @param pos Index of the enquired partition
	 * @since 03/06/2003
	 */
	OOPObjectId & GetRhsId (int pos);
	/**
	 * @brief Sets the Id of the Partition Relation object
	 * @param Id Id of the TPartitionRelation object
	 * @since 03/06/2003
	 */
	void    SetPartitionRelationId (OOPObjectId & Id,
									OOPDataVersion & version);
	/**
	 * @brief Returns the Id of the TPartitionRelation object.
	 * @since 03/06/2003
	 */
	OOPObjectId GetPartitionRelationId ();
private:
    /**
     * @brief Within the Execute Method we should be able to get the data pointer
     */
	void InitializePartitionRelationPointer ();
public:
    /**
     * @brief Set the version of the state and rhs upon which the created tasks will depend
     */
	void    SetVersionDependency (OOPDataVersion & version);
	/**
	 * @brief In this method the data dependencies of the current task will be initialized
	 */
	void    InitializeTaskDependencies ();
	/**
	 * @brief During the execution of this method, the flux tasks will be created
	 */
	OOPMReturnType Execute ();
private:
    /**
     * @brief Create tasks responsible for the computation of fluxes on independent partitions. \n
     * Partitios are obtained from MeTiS application on the original mesh.
     * @note Since the ParCompResidual is a recurrent task, the task creation will actually create the tasks only in its first run, on subsequent runs it will only actuate on the subtasks data dependence.
     * @since 03/06/2003
     */
	void    CreateFluxesTasks ();
    /**
     * @brief Initializes the DataVersion object according to amount of contributions
     * from neighbor partitions
     * @since 03/06/2003
     */
	void    InitializeSolutionVersion ();
public:
	virtual int  ClassId () const
	{
		return TPARCOMPUTE_ID;
	}
	/**
	 * @brief Packs the object in on the buffer so it can be transmitted through the network.
	 * The Pack function  packs the object's class_id while function Unpack() doesn't,
	 * allowing the user to identify the next object to be unpacked.
	 * @param buf A pointer to TSendStorage class to be packed.
	 */
	virtual void Write (TPZStream & buf, int withclassid=0);
	/**
	 * @brief Unpacks the object class_id
	 * @param buf A pointer to TSendStorage class to be unpacked.
	 */
	virtual void Read (TPZStream & buf, void * context = 0);
	static TPZSaveable *Restore (TPZStream & buf, void * context = 0);
private:
    /**
     * @brief Holds all Ids from Flux objects in the parallel computation
     */
	vector < OOPObjectId > fRhsIds;
    /**
     * @brief Holds all Ids from State variables in the parallel computation
     */
	vector < OOPObjectId > fStateIds;
    /**
     * @brief Holds all Ids from mesh variables in the parallel computation
     */
	vector < OOPObjectId > fMeshIds;
    /**
     * @brief Pointer to the PartitionRelation object (valid during execute)
     */
	TPartitionRelation *fPartRelationPtr;
    /**
     * @brief Holds the Id of the Partition Relation structure.
     * @since 03/06/2003
     */
	OOPObjectId fPartRelationId;
    /**
     * @brief The version of Partition Relation
     */
	OOPDataVersion fPartRelationVersion;
    /**
     * @brief Holds the Ids of sub tasks created in the process.
     * @since 03/06/2003
     */
	vector < OOPObjectId > fTaskIds;
    /**
     * @brief Holds the version of the solution and data upon which the tasks will depend
     */
	OOPDataVersion fDataVersions;
    /**
     * @brief Holds the version of the mesh  upon which the tasks will depend
     */
	OOPDataVersion fMeshVersions;
	/**
	 * @brief Number of partitions 
	 */
	int     fNPartitions;
};

extern int GLogMsgCounter;
extern ofstream TaskLog;

#endif // TPARCOMPUTE_H
