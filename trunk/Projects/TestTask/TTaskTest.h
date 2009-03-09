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
   TPZFMatrix m_Matrix;
   TPZGeoMesh m_Mesh;
public:
	virtual ~TTaskTest(){}
	
	TTaskTest(){} 
	/**
     * Simple constructor  
     */
	TTaskTest (int ProcId) : OOPTask(ProcId) , m_Matrix(5000,5000,1.) {
		TPZVec<int> nx(2,1000);
		TPZVec<REAL> x0(3,0);
		TPZVec<REAL> x1(3,1);
		TPZGenGrid grid(nx, x0, x1,1,0);
		grid.Read(m_Mesh);
	} 
    /**
	* Execute the task, verifying that all needed data acesses are satisfied.
	* The TParCompute is OOPTask descendent class and this method provides the access by the TaskManager.
	* @since 02/06/2003
	*/
	virtual OOPMReturnType Execute (); 

	virtual int ClassId () const
	{
		return TTASKTEST_ID; 
	}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
	void Write (TPZStream & buf,int withclassid);
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
	void Read (TPZStream & buf, void * context);
};

extern ofstream TaskLog;
#endif // TLOCALCOMPUTE_H
