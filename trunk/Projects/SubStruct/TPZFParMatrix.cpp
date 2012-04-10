//
//  TPZFParMatrix.cpp
//  OOPAR
//
//  Created by Philippe Devloo on 4/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "TPZFParMatrix.h"

TPZFParMatrix::~TPZFParMatrix()
{
    
}

/**
 * @brief Read and Write methods
 */
void TPZFParMatrix::Read(TPZStream &buf, void *context)
{
    DebugStop();
}

void TPZFParMatrix::Write(TPZStream &buf, int withclassid)
{
    DebugStop();
}

void TPZFParMatrix::Multiply(const TPZFMatrix<REAL> &A, TPZFMatrix<REAL> &B, int opt, int stride) const 
{
    DebugStop();
}

void TPZFParMatrix::MultAdd(const TPZFMatrix<REAL> &x, const TPZFMatrix<REAL> &y, TPZFMatrix<REAL> &z, const REAL alpha, const REAL beta, const int opt, const int stride) const
{
    DebugStop();
}

int TPZFParMatrix::Redim(const int rows, const int cols)
{
    DebugStop();
    return 0;
}

int TPZFParMatrix::Zero()
{
    DebugStop();
    return 0;
}

void TPZFParMatrix::SolveCG(int & 	numiterations, TPZSolver<REAL> & 	preconditioner, const TPZFMatrix<REAL> & 	F, TPZFMatrix<REAL> & result, TPZFMatrix<REAL> * residual, REAL & tol, const int 	FromCurrent)
{
    DebugStop();
}

TPZFMatrix<REAL> & TPZFParMatrix::operator = (const TPZFParMatrix & copy)
{
    DebugStop();
    return *this;
}

TPZFMatrix<REAL> & TPZFParMatrix::operator = (const TPZFMatrix & copy)
{
    DebugStop();
    return *this;
}
