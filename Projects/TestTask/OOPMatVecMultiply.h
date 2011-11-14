#ifndef OOPMATVECMT_H
#define OOPMATVECMT_H

/*
 *  OOPMatVecMultiply.h
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "ooptask.h"
#include "pzsave.h"
#include "pzfmatrix.h"

class TPZFMatrix;

#define OOPMATVECMULTIPLY_ID 1002
/**
 * @brief Implements a Matrix Vector multiplication on the OOPar environment. \n
 * Objects which take part on the operation are obtained by means of ObjectIds.
 */
class OOPMatVecMultiply : public OOPTask
{
	/**
	 * @brief For the \f$ K.u = f \f$ multiplication, it holds the u vector
	 */
	TPZFMatrix m_Vector;
	/**
	 * @brief Holds the Id of the Indexation object
	 */
	OOPObjectId m_IndexId;
	/**
	 * @brief Holds the Id of the Global Vector
	 */
	OOPObjectId m_GlobalVecId;
	
	/**
	 * @brief Triggers merge tasks
	 */
	void CreateMergeTask(TPZFMatrix & vector);
	
public:
	virtual ~OOPMatVecMultiply(){}
	/**
	 * @brief Simple constructors.
	 */
	OOPMatVecMultiply(){}; 
	OOPMatVecMultiply(int ProcId) : OOPTask(ProcId) {};
	/**
	 * @brief Sets the u vector
	 */
	void SetUVector(TPZFMatrix vector)
	{
		m_Vector = vector;
	}
	
	/**
	 * @brief Sets the Indexation object Id
	 */
	void SetIndexId(OOPObjectId Id)
	{
		m_IndexId = Id;
	}
	
	void SetGlobalVecId(OOPObjectId Id)
	{
		m_GlobalVecId = Id;
	}
	
	virtual OOPMReturnType Execute(); 
	
	virtual int ClassId() const
	{
		return OOPMATVECMULTIPLY_ID; 
	}
	/**
	 * @brief Packs the object in on the buffer so it can be transmitted through the network. \n
	 * The Pack function  packs the object's class_id while function Unpack() doesn't,
	 * allowing the user to identify the next object to be unpacked.
	 * @param buf A pointer to TSendStorage class to be packed.
	 */
	void Write (TPZStream & buf,int withclassid);
	/**
	 * Unpacks the object class_id
	 * @param buf A pointer to TSendStorage class to be unpacked.
	 */
	void Read (TPZStream & buf, void * context);
};


extern ofstream MatVecLog;

#endif
