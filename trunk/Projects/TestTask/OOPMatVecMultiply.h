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

class OOPMatVecMultiply : public OOPTask
{
	/**
	 * For the K.u = f multiplication, it holds the u vector
	 */
	TPZFMatrix m_Vector;
	/**
	 * Holds the Id of the Indexation object
	 */
	OOPObjectId m_IndexId;
	/**
	 * Holds the Id of the Global Vector
	 */
	OOPObjectId m_GlobalVecId;
	
	/**
	 * Triggers merge tasks
	 */
	void CreateMergeTask(TPZFMatrix & vector);
	
public:
	virtual ~OOPMatVecMultiply(){}
	/**
	 * Simple constructors.
	 */
	OOPMatVecMultiply(){}; 
	OOPMatVecMultiply(int ProcId) : OOPTask(ProcId) {};
	/**
	 * Sets the u vector
	 */
	void SetUVector(TPZFMatrix vector)
	{
		m_Vector = vector;
	}
	
	/**
	 * Sets the Indexation object Id
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


extern ofstream MatVecLog;
#endif