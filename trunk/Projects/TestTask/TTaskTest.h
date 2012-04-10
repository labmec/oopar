/**
 * @file
 * @brief Contains the definition of the TTaskTest class to TestTask project.
 */
#ifndef TTASKTEST_H
#define TTASKTEST_H
#include "ooptask.h"
#include "pzsave.h"
#include "pzfmatrix.h"
#include "pzgengrid.h"
#include "pzgmesh.h"

#define TTASKTEST_ID 1000

class   TTaskTest :public OOPTask
{
	TPZFMatrix<REAL> m_Matrix;
	TPZGeoMesh m_Mesh;
	
public:
	/** @brief Default destructor */
	virtual ~TTaskTest(){}
	/** @brief Default constructor */
	TTaskTest(){} 
	
	/**
     * @brief Simple constructor given the id of the processor
     */
	TTaskTest (int ProcId) : OOPTask(ProcId) , m_Matrix(5000,5000,1.) {
		TPZVec<int> nx(2,1000);
		TPZVec<REAL> x0(3,0);
		TPZVec<REAL> x1(3,1);
		TPZGenGrid grid(nx, x0, x1,1,0);
		grid.Read(&m_Mesh);
	}
	
    /**
	 * @brief Execute the task, verifying that all needed data acesses are satisfied. \n
	 * The TParCompute is OOPTask descendent class and this method provides the access by the TaskManager.
	 * @since 02/06/2003
	 */
	virtual OOPMReturnType Execute (); 
	
	virtual int ClassId () const
	{
		return TTASKTEST_ID; 
	}
	
	/**
	 * @brief Packs the object in on the buffer so it can be transmitted through the network. \n
	 * The Pack function  packs the object's class_id while function Unpack() doesn't,
	 * allowing the user to identify the next object to be unpacked.
	 * @param buf A pointer to TPZStream class to be packed.
	 * @param withclassid identifier of the current class
	 */
	void Write(TPZStream & buf,int withclassid);
	/**
	 * @brief Unpacks the object class_id
	 * @param buf A pointer to TPZStream class to be unpacked.
	 * @param context Pointer of the data.
	 */
	void Read(TPZStream & buf, void * context);
};

extern ofstream TaskLog;

#endif // TLOCALCOMPUTE_H
