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
#include "tpzdohrmatrix.h"
//#include "tpzdohrsubstruct.h"

#include "tpzdohrassembly.h"
#include "pzlog.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("substruct.dohrsubstruct"));
#endif

TPZDohrMatrix::TPZDohrMatrix(TPZAutoPointer<TPZDohrAssembly> assembly)
 : TPZMatrix(), fAssembly(assembly)
{
}


TPZDohrMatrix::~TPZDohrMatrix()
{
}


void TPZDohrMatrix::MultAdd(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z,
                        const REAL alpha,const REAL beta,const int opt,const int stride) const
{
  if ((!opt && Cols() != x.Rows()*stride) || Rows() != x.Rows()*stride)
    Error( "Operator* <matrixs with incompatible dimensions>" );
  if(x.Cols() != y.Cols() || x.Cols() != z.Cols() || x.Rows() != y.Rows() || x.Rows() != z.Rows()) {
    Error ("TPZFMatrix::MultiplyAdd incompatible dimensions\n");
  }
  int rows = Rows();
  int cols = Cols();
  int xcols = x.Cols();
  int ic, c, r;
  PrepareZ(y,z,beta,opt,stride);
  
  SubsList::const_iterator iter;
	int isub = 0;
  for (iter=fGlobal.begin();iter!=fGlobal.end();iter++,isub++) {
	  TPZFMatrix xlocal,zlocal;
	  fAssembly->Extract(isub,x,xlocal);
	  zlocal.Redim(xlocal.Rows(),xlocal.Cols());
    (*iter)->ContributeKULocal(alpha,xlocal,zlocal);
	  fAssembly->Assemble(isub,zlocal,z);
//         z.Print("Resultado intermediario");
  }
}

void TPZDohrMatrix::Initialize() 
{
	std::cout << "Number of substructures " << fGlobal.size() << std::endl;
	TPZFMatrix diag(Rows(),1,0.);
	SubsList::iterator iter;
	int isub = 0;
	for (iter=fGlobal.begin();iter!=fGlobal.end();iter++,isub++) {
        //Basic initialization for each substructure (compute the matrices)
        (*iter)->Initialize();
		TPZFMatrix diaglocal;
        (*iter)->ContributeDiagonalLocal(diaglocal);
		LOGPZ_DEBUG(logger,"Before assemble diagonal")
		this->fAssembly->Assemble(isub,diaglocal,diag);
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << "Substructure " << isub << " ";
			diag.Print("Global Diagonal matrix",sout);
			LOGPZ_DEBUG(logger,sout.str())
		}
#endif
        std::cout << '*';
        std::cout.flush();
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		diag.Print("Global Diagonal matrix",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
	std::cout << std::endl;
	for (iter=fGlobal.begin(),isub=0;iter!=fGlobal.end();iter++,isub++) {
        //Computes the Weights for each substructure
		TPZFMatrix diaglocal;
		this->fAssembly->Extract(isub,diag,diaglocal);
        (*iter)->ComputeWeightsLocal(diaglocal);
		
	}
}
	
/**
 * Adjust the residual to zero the residual of the internal connects
 */
void TPZDohrMatrix::AdjustResidual(TPZFMatrix &res)
{
	SubsList::iterator iter;
	for (iter=fGlobal.begin();iter!=fGlobal.end();iter++) {
		(*iter)->AdjustResidual(res);
	}
}

/**
 * Add the solution corresponding to the internal residual
 */
void TPZDohrMatrix::AddInternalSolution(TPZFMatrix &solution)
{
	SubsList::iterator iter;
	for (iter=fGlobal.begin();iter!=fGlobal.end();iter++) {
		(*iter)->AddInternalSolution(solution);
	}
}

