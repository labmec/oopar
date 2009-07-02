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
#include "tpzdohrprecond.h"
//#include <iostream>
//#include <cstdlib>

#include <sstream>
#include "pzlog.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("substruct.dohrprecond"));
#endif


using namespace std;

TPZDohrPrecond::TPZDohrPrecond(TPZDohrMatrix &origin, TPZAutoPointer<TPZDohrAssembly> assemble)
 : TPZMatrix(origin), fGlobal(origin.SubStructures()), fNumCoarse(origin.NumCoarse()), fAssemble(assemble)
{
  
  Initialize();
}


TPZDohrPrecond::~TPZDohrPrecond()
{
  if (fCoarse) 
  {
    delete fCoarse;
    fCoarse = 0;
  }
}

void TPZDohrPrecond::MultAddPREVIOUS(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z, const REAL alpha,const REAL beta,const int opt,const int stride) const {
  if ((!opt && Cols() != x.Rows()*stride) || Rows() != x.Rows()*stride)
    Error( "Operator* <matrices with incompatible dimensions>" );
  if(x.Cols() != y.Cols() || x.Cols() != z.Cols() || x.Rows() != y.Rows() || x.Rows() != z.Rows()) {
    Error ("TPZFMatrix::MultiplyAdd incompatible dimensions\n");
  }
  int rows = Rows();
  int cols = Cols();
  int xcols = x.Cols();
  TPZFMatrix xloc(x);
  int ic, c, r;
  PrepareZ(y,z,beta,opt,stride);
  
  /* Computing r(c) */
  TPZFMatrix CoarseResidual(fNumCoarse,1);
  CoarseResidual.Zero();
  std::list<TPZAutoPointer<TPZDohrSubstruct> >::const_iterator it;
	
  for(it= fGlobal.begin(); it != fGlobal.end(); it++) {
    (*it)->LoadWeightedResidual(xloc);
    (*it)->Contribute_rc(CoarseResidual);
  }
#ifdef LOG4CXX
	{
		std::stringstream sout;
		CoarseResidual.Print("Coarse Residual",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
  /* Computing K(c)_inverted*r(c) and stores it in "product" */
  fCoarse->SetDirect(ELU);
  //Dado global 
  TPZFMatrix product(fNumCoarse,1);
  fCoarse->Solve(CoarseResidual,product);
#ifdef LOG4CXX
	{
		std::stringstream sout;
		product.Print("Product",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
	/* Computing v1 and v2 */
  TPZFMatrix v1(cols,1,0.);
  TPZFMatrix v2(cols,1,0.);
  //Criar tarefa que execute a distribuicao de cada elemento do fGlobal
  for(it= fGlobal.begin(); it != fGlobal.end(); it++)
  {
  // Gerenciamento Global->Local sobre o product
    //product é administrado pelo DM mas permanece no processador 0
    // tarefa separada, expansao da solucao coarse
    (*it)->Contribute_v1(v1,product);
    
    // contribute v2 deve ser uma tarefa inicializada mais cedo
    (*it)->Contribute_v2(v2);
#ifdef LOG4CXX
	  {
		  std::stringstream sout;
		  v1.Print("Accumulated v1",sout);
		  v2.Print("Accumulated v2",sout);
		  LOGPZ_DEBUG(logger,sout.str())
	  }
#endif
  }
  
  /* Computing v3 */
  TPZFMatrix v3(cols,1,0.);
  TPZFMatrix v1Plusv2(cols,1);
  /**
    At this stage both v1 and v3 are available and on the main processor.
  */
  v1.Add(v2,v1Plusv2);
#ifndef MAKEINTERNAL
  for(it= fGlobal.begin(); it != fGlobal.end(); it++)
  {
  //v3 e o dado de saida
  //analogo ao product
    (*it)->Contribute_v3(v3, x, v1Plusv2);
  }
  /* And now v1 will be equal to v1+v2+v3 */
  //Tarefa no processador 0
  //com a contribuicao 
  v1Plusv2.Add(v3,v1);
#else
	v1 = v1Plusv2;
#endif
  // wait task para finalizacao da chamada
  // esperar a versao correta do v1
  /* Sum v1+v2+v3 with z */
  for (c=0; c<rows; c++) {
    z(c,0) += v1(c,0);
  }
}

void TPZDohrPrecond::MultAdd(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z, const REAL alpha,const REAL beta,const int opt,const int stride) const {
	if ((!opt && Cols() != x.Rows()*stride) || Rows() != x.Rows()*stride)
		Error( "Operator* <matrices with incompatible dimensions>" );
	if(x.Cols() != y.Cols() || x.Cols() != z.Cols() || x.Rows() != y.Rows() || x.Rows() != z.Rows()) {
		Error ("TPZFMatrix::MultiplyAdd incompatible dimensions\n");
	}
	int rows = Rows();
	int cols = Cols();
	int xcols = x.Cols();
	int ic, c, r;
	PrepareZ(y,z,beta,opt,stride);
#ifdef LOG4CXX
	{
		std::stringstream sout;
		x.Print("x entry vector",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
	
	/* Computing r(c) */
	TPZFMatrix CoarseResidual(fNumCoarse,1);
	CoarseResidual.Zero();
	std::list<TPZAutoPointer<TPZDohrSubstruct> >::const_iterator it;
	
	int isub = 0;
	for(it= fGlobal.begin(); it != fGlobal.end(); it++, isub++) {
		TPZFMatrix xloc, CoarseResidual_local;
		fAssemble->Extract(isub,x,xloc);
//		(*it)->LoadWeightedResidual(xloc);
		(*it)->Contribute_rc_local(xloc,CoarseResidual_local);
		fAssemble->AssembleCoarse(isub,CoarseResidual_local,CoarseResidual);
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		CoarseResidual.Print("Coarse Residual",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
	/* Computing K(c)_inverted*r(c) and stores it in "product" */
	fCoarse->SetDirect(ELU);
	//Dado global 
	TPZFMatrix CoarseSolution(fNumCoarse,1);
	fCoarse->Solve(CoarseResidual,CoarseSolution);
#ifdef LOG4CXX
	{
		std::stringstream sout;
		CoarseSolution.Print("Product",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
	TPZFMatrix v2(cols,1,0.);
	isub=0;
	//Criar tarefa que execute a distribuicao de cada elemento do fGlobal
	for(it= fGlobal.begin(); it != fGlobal.end(); it++,isub++)
	{
		// Gerenciamento Global->Local sobre o product
		//product é administrado pelo DM mas permanece no processador 0
		// tarefa separada, expansao da solucao coarse
		TPZFMatrix v1_local,CoarseSolution_local;
		fAssemble->ExtractCoarse(isub,CoarseSolution,CoarseSolution_local);
		(*it)->Contribute_v1_local(v1_local,CoarseSolution_local);
		
		// contribute v2 deve ser uma tarefa inicializada mais cedo
		TPZFNMatrix<100> Residual_local,v2_local;
		fAssemble->Extract(isub,x,Residual_local);
		(*it)->Contribute_v2_local(Residual_local,v2_local);
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << "Substructure " << isub << std::endl;
			Residual_local.Print("Residual local",sout);
			LOGPZ_DEBUG(logger,sout.str())
		}
#endif
		v2_local += v1_local;
		fAssemble->Assemble(isub,v2_local,v2);
#ifdef LOG4CXX
		{
			std::stringstream sout;
			v2.Print("Accumulated v2",sout);
			LOGPZ_DEBUG(logger,sout.str())
		}
#endif
	}
	
#ifndef MAKEINTERNAL	
	isub=0;
	//Criar tarefa que execute a distribuicao de cada elemento do fGlobal
	for(it= fGlobal.begin(); it != fGlobal.end(); it++,isub++)
	{
		TPZFNMatrix<100> v2Expand((*it)->fNEquations,1,0.), v3Expand((*it)->fNEquations,1,0.);
		int neqs = (*it)->fGlobalEqs.NElements();
		TPZFMatrix v3_local(neqs,1,0.), v2_local(neqs,1,0.);
		fAssemble->Extract(isub,v2,v2_local);
		int i;
		for (i=0;i<neqs;i++) 
		{
			std::pair<int,int> ind = (*it)->fGlobalEqs[i];
			v2Expand(ind.first,0) += v2_local(i,0);
		}
		
		(*it)->Contribute_v3_local(v2Expand,v3Expand);
		for (i=0;i<neqs;i++) 
		{
			std::pair<int,int> ind = (*it)->fGlobalEqs[i];
			v3_local(i,0) += v3Expand(ind.first,0);
		}
#ifdef LOG4CXX
		{
			std::stringstream sout;
			v2Expand.Print("v1+v2 Expand",sout);
			v3Expand.Print("v3 Expand", sout);
			v2_local.Print("v1+v2 local",sout);
			v3_local.Print("v3 local",sout);
			LOGPZ_DEBUG(logger,sout.str())
		}
#endif
		fAssemble->Assemble(isub,v3_local,v2);
	}		
#endif
	// wait task para finalizacao da chamada
	// esperar a versao correta do v1
	/* Sum v1+v2+v3 with z */
	for (c=0; c<rows; c++) {
		z(c,0) += v2(c,0);
	}
}

void TPZDohrPrecond::MultAddTest(const TPZFMatrix &x,const TPZFMatrix &y, TPZFMatrix &z, const REAL alpha,const REAL beta,const int opt,const int stride) {
  if ((!opt && Cols() != x.Rows()*stride) || Rows() != x.Rows()*stride)
    Error( "Operator* <matrices with incompatible dimensions>" );
  if(x.Cols() != y.Cols() || x.Cols() != z.Cols() || x.Rows() != y.Rows() || x.Rows() != z.Rows()) {
    Error ("TPZFMatrix::MultiplyAdd incompatible dimensions\n");
  }
  int rows = Rows();
  int cols = Cols();
  int xcols = x.Cols();
  int ic, c, r;
  PrepareZ(y,z,beta,opt,stride);
  TPZFMatrix xloc(x);
  /* Computing r(c) */
  TPZFMatrix CoarseResidual(fNumCoarse,1);
  CoarseResidual.Zero();
  std::list<TPZAutoPointer<TPZDohrSubstruct> >::iterator it;
/*  for(it= fGlobal.begin(); it != fGlobal.end(); it++) {
    (*it)->AdjustResidual(xloc);
  }*/
  for(it= fGlobal.begin(); it != fGlobal.end(); it++) {
    (*it)->LoadWeightedResidual(xloc);
    (*it)->Contribute_rc(CoarseResidual);
  }
  /* Computing K(c)_inverted*r(c) and stores it in "product" */
  fCoarse->SetDirect(ELU);
  TPZFMatrix product(fNumCoarse,1);
  fCoarse->Solve(CoarseResidual,product);
  /* Computing v1 and v2 */
  TPZFMatrix v1(cols,1,0.);
  TPZFMatrix v2(cols,1,0.);
  /* Computing K(c)_inverted*testV1 and stores it in "product" */
  for(it= fGlobal.begin(); it != fGlobal.end(); it++)
  {
    (*it)->Contribute_v1(v1,product);
    (*it)->Contribute_v2(v2);
  }
  product.Resize(fNumCoarse,cols);
  /* transformV1 = v1's transformation */
  /* Computing v3 */
  TPZFMatrix v3(cols,1,0.);
  TPZFMatrix v1Plusv2(cols,1,0.);
  v1.Add(v2,v1Plusv2);
#ifndef MAKEINTERNAL
  for(it= fGlobal.begin(); it != fGlobal.end(); it++)
  {
    (*it)->Contribute_v3(v3, x, v1Plusv2);
  }
  /* And now v1 will be equal to v1+v2+v3 */
  v1Plusv2.Add(v3,v1);
#else
	v1 = v1Plusv2;
#endif
  /* Sum v1+v2+v3 with z */
  for (c=0; c<rows; c++) {
    z(c,0) += v1(c,0);
  }
}

void TPZDohrPrecond::Initialize()
{
  /* Computing K(c) */
  TPZFMatrix *coarse = new TPZFMatrix(fNumCoarse,fNumCoarse,0.);
  std::list<TPZAutoPointer<TPZDohrSubstruct> >::iterator it;
  for(it= fGlobal.begin(); it != fGlobal.end(); it++) {
    (*it)->Contribute_Kc(*coarse);
  }
  fCoarse = new TPZStepSolver(coarse);
}

void TPZDohrPrecond::TriggerContributeV1(const std::list<TPZAutoPointer<TPZDohrSubstruct> > & Global, TPZFMatrix & v1, TPZFMatrix & product) const
{
  std::list<TPZAutoPointer<TPZDohrSubstruct> >::const_iterator it;
  for(it= Global.begin(); it != Global.end(); it++)
  {
  // Gerenciamento Global->Local sobre o product
    //product é administrado pelo DM mas permanece no processador 0
    // tarefa separada, expansao da solucao coarse
    (*it)->Contribute_v1(v1,product);
    
  }
}
void TPZDohrPrecond::TriggerContributeV2(const std::list<TPZAutoPointer<TPZDohrSubstruct> > & Global, TPZFMatrix & v2) const
{
  std::list<TPZAutoPointer<TPZDohrSubstruct> >::const_iterator it;  
  for(it= Global.begin(); it != Global.end(); it++)
  {
  // Gerenciamento Global->Local sobre o product
    //product é administrado pelo DM mas permanece no processador 0
    // tarefa separada, expansao da solucao coarse
    
    // contribute v2 deve ser uma tarefa inicializada mais cedo
    (*it)->Contribute_v2(v2);
  }
}
void TPZDohrPrecond::TriggerContributeV3(const std::list<TPZAutoPointer<TPZDohrSubstruct> > & Global, TPZFMatrix & x, TPZFMatrix & v1Plusv2, TPZFMatrix & v3) const
{
  std::list<TPZAutoPointer<TPZDohrSubstruct> >::const_iterator it;  
  for(it= Global.begin(); it != Global.end(); it++)
  {
  //v3 e o dado de saida
  //analogo ao product
    (*it)->Contribute_v3(v3, x, v1Plusv2);
  }
}