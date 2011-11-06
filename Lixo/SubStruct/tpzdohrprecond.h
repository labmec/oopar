/***************************************************************************
 *   Copyright (C) 2006 by Philippe Devloo   *
 *   phil@fec.unicamp.br   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TPZDOHRPRECOND_H
#define TPZDOHRPRECOND_H

#include "pzmatrix.h"
#include <list>
#include "tpzautopointer.h"
#include "tpzdohrsubstruct.h"
#include "tpzdohrmatrix.h"
#include "tpzdohrassembly.h"

/**
This class represents a matrix which computes the preconditioner developed by Dohrmann

@author Philippe Devloo
*/
class TPZDohrPrecond : public TPZMatrix
{
 /**
  * The matrix class is a placeholder for a list of substructures
  */
  std::list<TPZAutoPointer<TPZDohrSubstruct> > fGlobal;
 /**
  * The global matrix associated with the coarse degrees of freedom
  */
  TPZStepSolver * fCoarse; //K(c)
 /**
  * The global residual vector associated with the coarse degrees of freedom
  */
//  TPZFMatrix fCoarseResidual; //r(c)
  /**
  * The product K(c)_inv*r(c)
  */
//  TPZFMatrix fInvKcrc; //r(c)
 /**
  * Size of the coarse system
  */
  int fNumCoarse; //n(c)
	
	TPZAutoPointer<TPZDohrAssembly> fAssemble;
 
public:
    /// comentario do caio
    TPZDohrPrecond(TPZDohrMatrix &origin, TPZAutoPointer<TPZDohrAssembly> assemble);

    ~TPZDohrPrecond();
    
    CLONEDEF(TPZDohrPrecond)

  /// Initialize the necessary datastructures
  /**
  * It will compute the coarse matrix, coarse residual and any other necessary data structures
  */
  void Initialize();
    
  void AddSubstruct(TPZAutoPointer<TPZDohrSubstruct> substruct);
    
  /// The only method any matrix class needs to implement
  /**
  * In this case the variable x represents the rhs and z the result of the preconditioning
  * When used as a preconditioner y will be zero
  * In fact, it will compute v1+v2+v3
  * It computes z = beta * y + alpha * opt(this)*x but z and x can not overlap in memory.
  * @param x Is x on the above operation. It must be a vector!
  * @param y Is y on the above operation
  * @param z Is z on the above operation
  * @param alpha Is alpha on the above operation
  * @param beta Is beta on the above operation
  * @param opt Indicates if is Transpose or not
  * @param stride Indicates n/N where n is dimension of the right hand side vector and N is matrix dimension
  */
  virtual void MultAdd(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z,
                        const REAL alpha,const REAL beta,const int opt,const int stride) const;
	virtual void MultAddPREVIOUS(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z,
						 const REAL alpha,const REAL beta,const int opt,const int stride) const;

  virtual void MultAddTest(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z,
                        const REAL alpha,const REAL beta,const int opt,const int stride);

  /// specify the solution process for the coarse matrix
  void SetSolver(TPZSolver &solver);
  /**
  * Set of functions to be used with the OOPar implementation
  */
  void TriggerContributeV1(const std::list<TPZAutoPointer<TPZDohrSubstruct> > & Global, TPZFMatrix & v1, TPZFMatrix & product) const;
  void TriggerContributeV2(const std::list<TPZAutoPointer<TPZDohrSubstruct> > & Global, TPZFMatrix & v2) const;
  void TriggerContributeV3(const std::list<TPZAutoPointer<TPZDohrSubstruct> > & Global, TPZFMatrix & x, TPZFMatrix & v1Plusv2, TPZFMatrix & v3) const;
};

#endif
