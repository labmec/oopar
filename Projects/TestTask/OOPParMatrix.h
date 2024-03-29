#ifndef OOPPARMAT_H
#define OOPPARMAT_H
/*
 *  OOPParMatrix.h
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "pzfmatrix.h"
#include "oopobjectid.h"

class OOPSubMatrix;

class OOPParMatrix 
{
public:
	OOPParMatrix(){};
	void BuildMe(int size, TPZMatrix<REAL> * matrix, TPZFMatrix<REAL> & vU);
	void DivideMe(int nPieces, TPZMatrix<REAL> * OrigMatrix, std::vector<TPZMatrix<REAL> *> & subMats, 
				  std::vector< std::pair< std::vector<int>, std::vector<int> > > & Indices);
	
};

#endif