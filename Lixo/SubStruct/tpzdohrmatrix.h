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
#ifndef TPZDOHRMATRIX_H
#define TPZDOHRMATRIX_H

#include "pzmatrix.h"
#include <list>
#include <sstream>
#include "tpzautopointer.h"
#include "tpzdohrsubstruct.h"
#include "tpzdohrassembly.h"
/**
This class represents a matrix divided into substructures

@author Philippe Devloo
*/
class TPZDohrMatrix : public TPZMatrix
{
public:
  /**
   * The matrix class is a placeholder for a list of substructures
   */
   typedef std::list<TPZAutoPointer<TPZDohrSubstruct> > SubsList;
private:
   SubsList fGlobal;
	   
   int fNumCoarse; //n(c)
public:

	TPZAutoPointer<TPZDohrAssembly> fAssembly;

	TPZDohrMatrix(TPZAutoPointer<TPZDohrAssembly> dohrassembly);
    
    TPZDohrMatrix(const TPZDohrMatrix &cp) : fGlobal(cp.fGlobal), fAssembly(cp.fAssembly), fNumCoarse(cp.fNumCoarse)
    {
    }
    
    CLONEDEF(TPZDohrMatrix)

    ~TPZDohrMatrix();

    const SubsList &SubStructures() const
    {
      return fGlobal;
    }
    
    int NumCoarse() const
    {
      return fNumCoarse;
    }
    
   /**
    * Just a method for tests
    */
    TPZAutoPointer<TPZDohrSubstruct> GetFirstSub() {
      return (*fGlobal.begin());
    }
   /**
    * Just a method for tests
    */
    void Print(std::ostream &out) {
		out << "Number of coarse equations " << fNumCoarse << std::endl;
      SubsList::iterator iter;
      for (iter=fGlobal.begin();iter!=fGlobal.end();iter++) {
        (*iter)->Print(out);
      }
    }
    
    void SetNumCornerEqs(int nc)
    {
      fNumCoarse = nc;
    }
   /**
    * Another method for tests
    */
    void Teste(TPZFMatrix &diag) {
      SubsList::iterator iter;
      for (iter=fGlobal.begin();iter!=fGlobal.end();iter++) {
        (*iter)->ContributeGlobalDiagonal(diag);
      }
      for (iter=fGlobal.begin();iter!=fGlobal.end();iter++) {
        (*iter)->ComputeWeights(diag);
      }
    }
   /**
    * Initialize the necessary datastructures
    */
    void Initialize();   
	/**
    * It adds a substruct
    */
    void AddSubstruct(TPZAutoPointer<TPZDohrSubstruct> substruct)
    {
      fGlobal.push_back(substruct);
    }
    
    /// The only method any matrix class needs to implement
  /**
     * It computes z = beta * y + alpha * opt(this)*x but z and x can not overlap in memory.
     * @param x Is x on the above operation
     * @param y Is y on the above operation
     * @param z Is z on the above operation
     * @param alpha Is alpha on the above operation
     * @param beta Is beta on the above operation
     * @param opt Indicates if is Transpose or not
     * @param stride Indicates n/N where n is dimension of the right hand side vector and N is matrix dimension
   */
    virtual void MultAdd(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z,const REAL alpha,const REAL beta,const int opt,const int stride) const;
	
	/**
	 * Adjust the residual to zero the residual of the internal connects
	 */
	void AdjustResidual(TPZFMatrix &res);
	
	/**
	 * Add the solution corresponding to the internal residual
	 */
	void AddInternalSolution(TPZFMatrix &solution);


};

#endif
