// -*- c++ -*-
/* Generated by Together */
#ifndef TPARANALYSIS_H
#define TPARANALYSIS_H

#include "TParCompute.h"
#include "TParMesh.h"
#include "oopdatamanager.h"
#include "ooptask.h"
#include "oopdataversion.h"
#include "fluxdefs.h"

class TPZStream;
using namespace std;

/**
 * @brief Implements the functionalities of the Analysis classes incorporating
 * parallelization optimization. \n
 * This class is not part of the parallel environment, although it make some calls to the Data and Task manager.
 * @note Its an auxiliar class which triggers the environment.
 */
class   TParAnalysis:public OOPTask
{
public:
	virtual ~TParAnalysis(){}
	TParAnalysis();
	TParAnalysis (int Procid);
	TParAnalysis (int Procid, int numpartitions, int numproc);
    /**
     * @brief Initializes and submit all necessary data and tasks to the Data and Task managers. 
     */
	void    SetupEnvironment ();
	virtual int ClassId () const
	{
		return TPARANAYSIS_ID;
	}
    /**
     * @brief Sets the version of the mesh, state and rhs to random values and creates the parcompute task
     */
	void    CreateParCompute ();
    /**
     * @brief Sets the version of the rhs and state appropriate for the TParCompute class to kick in
     */
	void    SetAppropriateVersions ();
    /**
     * @brief Modify the solution version to adequate the TParCompute tasks
     */
	void    AdaptSolutionVersion (OOPDataVersion & version);
	virtual OOPMReturnType Execute ();
	void    Print (ostream & out = cout);
	/**
	 * @brief Packs the object in on the buffer so it can be transmitted through the network. \n
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
	OOPObjectId fRelationTable;
	int     fNumPartitions;
	int fNumProcessors;
	OOPDataVersion fTaskVersion;
	
	vector < OOPObjectId > fRhsId;
	vector < OOPObjectId > fMeshId;
	vector < OOPObjectId > fStateId;
};

extern ofstream TaskLog;

#endif // TPARANALYSIS_H
