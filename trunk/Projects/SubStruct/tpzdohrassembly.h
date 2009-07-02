/*
 *  tpzdohrassembly.h
 *  SubStruct
 *
 *  Created by Philippe Devloo on 04/03/09.
 *  Copyright 2009 UNICAMP. All rights reserved.
 *
 */

#ifndef TPZDOHRASSEMBLYH
#define TPZDOHRASSEMBLYH

#include "pzvec.h"
#include "pzsave.h"

class TPZFMatrix;

class TPZDohrAssembly : public TPZSaveable
{
public:
	/// For each substructure the equation numbering of the substructures
	TPZVec< TPZVec< int > > fFineEqs;
	
	/// for each substructure the equation numbering of the coarse equations
	TPZVec< TPZVec< int > > fCoarseEqs;
	
	/// sum the values in the local matrix into the global matrix
	void Assemble(int isub, const TPZFMatrix &local, TPZFMatrix &global);
	
	/// extract the values from the global matrix into the local matrix
	void Extract(int isub, const TPZFMatrix &global, TPZFMatrix &local);
	
	/// sum the values in the local matrix into the global matrix
	void AssembleCoarse(int isub, const TPZFMatrix &local, TPZFMatrix &global);
	
	/// extract the values from the global matrix into the local matrix
	void ExtractCoarse(int isub, const TPZFMatrix &global, TPZFMatrix &local);
private:
	virtual void Write(TPZStream &buf, int withclassid);
	virtual void Read(TPZStream &buf, void *context);
	void WriteVector(TPZStream & buf, TPZVec< TPZVec < int > >  & vec);
	void ReadVector(TPZStream & buf, TPZVec< TPZVec < int > >  & vec);
};
#endif