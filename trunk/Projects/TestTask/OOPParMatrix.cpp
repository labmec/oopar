/*
 *  OOPParMatrix.cpp
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPParMatrix.h"

#include <iostream>

void OOPParMatrix::BuildMe(int size, TPZMatrix<REAL> * matrix, TPZFMatrix<REAL> & vU)
{
	matrix->Resize(size, size);
	int i, j;
	
	vU.Resize(size, 1);
	for(i = 0; i < size; i++)
	{
		double value;
		for(j=0; j < size; j++)
		{
			value = rand()/1e8;
			matrix->Put(i, j, value);
		}
		vU.Put(i, 0,value * 1.4);
	}
	
}
void OOPParMatrix::DivideMe(int nPieces, TPZMatrix<REAL> * OrigMatrix, std::vector<TPZMatrix<REAL> *> & subMats, 
							std::vector< std::pair< std::vector<int>, std::vector<int> > > & Indices)
{
	int i;
	int rows = OrigMatrix->Rows();
	int nrows = rows/nPieces;
	int lastrow = nrows;
	if(nrows * nPieces != rows)
	{
		lastrow = rows - (nPieces -1)*nrows;
	}
	
	std::vector<int> lrows, lcols;
	lrows.resize(nPieces + 1);
	lcols.resize(nPieces + 1);
	
	for(i = 0; i< nPieces ; i++)
	{
		lrows[i] = i * nrows;
		lcols[i] = i * nrows;
	}
	
	lrows[nPieces] = rows;
	lcols[nPieces] = rows;
	
	subMats.resize(nPieces * nPieces);
	Indices.resize(nPieces * nPieces);
	
	int n, j;
	int ii = 0;
	int jj = 0;
	n = 0;
	
	for(i = 1; i < nPieces+1; i++)
	{
		for(j = 1;j < nPieces+1; j++)
		{
			TPZMatrix<REAL> * subM = OrigMatrix->Clone();
			subM->Resize(lrows[i] - lrows[i-1], lcols[j] - lcols[j-1]);
			std::vector<int> vRows;
			vRows.resize(lrows[i] - lrows[i-1]);
			std::vector<int> vCols;
			vCols.resize(lcols[j] - lcols[j-1]);
			int li = 0;
			for(ii = lrows[i-1]; ii < lrows[i]; ii++)
			{
				int lj=0;
				for(jj = lcols[j-1]; jj < lcols[j]; jj++)
				{
					double value = OrigMatrix->Get(ii, jj);
					subM->Put(li,lj,value);
					vCols[lj]=jj;
					lj++;
				}
				vRows[li]=ii;
				li++;
			}
			subMats[n] = subM;
			Indices[n] = std::pair< std::vector<int>, std::vector<int> >(vRows, vCols);
			n++;
		}
	}
	/*
	 std::cout << "Li " << li << " II " << ii << std::endl;
	 std::cout.flush();
	 std::cout << "Lj " << lj << " JJ " << jj << std::endl;
	 std::cout.flush();
	 */
	
}
