/*
 *  parsubstruct.cpp
 *  SubStruct
 *
 *  Created by Gustavo Longhin on 3/12/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */




#ifdef OOP_MPI
#include "oopmpicomm.h"
#else
#include "oopsocketcommmanager.h"
#endif



#include <iostream>
#include <sstream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopwaittask.h"
#include "oopsnapshottask.h"
#include "pzlog.h"

#ifdef LOGPZ
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif
#include "oopaccesstag.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif

//Dohr Includes

/**
 *
 #include "pzfmatrix.h"
*/
#include "tpzdohrassembly.h"
#include "tpzdohrsubstruct.h"

//Parallel Dohr includes
#include "OOPComputeMatricesTask.h"
#include "OOPContribWeightTask.h"

using namespace std;


OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;



int matmain(int argc, char **argv)
{
  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0][1] << std::endl;
	std::cout.flush();
#ifdef OOP_MPI
	CM = new OOPMPICommManager(argc, argv);
#else
  CM = new OOPSocketCommManager;
	((OOPSocketCommManager *)CM)->Initialize();
	//CM->Initialize(argv[0], 4);
	((OOPSocketCommManager *)CM)->Barrier();
#endif
#ifdef LOG4CXX
#ifdef OOP_SOCKET
  std::stringstream sin;
  sin << "/Users/longhin/Projetos/XCode/OOParApp/build/Debug/log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#elif OOP_MPI
  std::stringstream sin;
  sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
	
#endif
	
#endif
	
#ifdef OOP_MPE
  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
#endif
  TM = new OOPTaskManager (CM->GetProcID ());
  DM = new OOPDataManager (CM->GetProcID ());
	
	
  TM->SetNumberOfThreads(10);
  TM->Execute();
  if(!CM->GetProcID())
  {
		//On processor ZERO
		//
		std::cout << "Inside Processor 0 \n";
		std::cout.flush();
		int nprocs = CM->NumProcessors();
		int i;
		
		/**
		 * Submit DohrSubStruct object for each processor in the environment
		 * Obviouslly this call must processed by something similar to GenSubstruct
		 */
		TPZDohrSubstruct * substr = NULL;
		std::vector<OOPObjectId> m_SubStrIds;
		m_SubStrIds.resize(CM->NumProcessors());
		for(i = 0; i < nprocs; i++)
		{
			substr = new TPZDohrSubstruct;
			m_SubStrIds[i] = DM->SubmitObject(substr);
		}
		
		
		/**
		 * Submit Diagonal Object on the main processor.
		 * Submit Assembly objects for the mapping from original to distributed meshes.
		 * Only one Assembly object is submitted. All Tasks which require mapping functionalities must be submitted with ReadAccess requests for the Asssembly object.
		 */
		TPZDohrAssembly * assembly = new TPZDohrAssembly;
		OOPObjectId AssemblyId = DM->SubmitObject(assembly);
		int diagSize = 0;
		TPZFMatrix * Diagonal = new TPZFMatrix(diagSize);
		OOPObjectId DiagId = DM->SubmitObject(Diagonal);
		/**
		 * For each processor a SubDiagonalTask object must be submitted. A SubDiagonal object holds in its data structure values to be added in the main diagonal
		 * which rezides on Processor ZERO. The Execute method of a SUbDiagonal task adds its information to the main diagonal.
		 * After computing its information on the precessor ZERO, information are again distributed to remaining processors. Not sure if some SubDiagonalData
		 * objects are required. Assuming it won't be.
		 */
		OOPComputeMatricesTask * cdiagt;
		OOPDataVersion assemblyVersion;
		for(i = 0; i < nprocs; i++)
		{
			cdiagt = new OOPComputeMatricesTask(i);
			cdiagt->AddDependentData(OOPAccessTag(AssemblyId, EReadAccess, assemblyVersion, 0));
			cdiagt->AddDependentData(OOPAccessTag(m_SubStrIds[i], EReadAccess, assemblyVersion, 0));
			cdiagt->SetMainDiagId(DiagId);
		}
		
		/**
		 * In the processor ZERO, the MainDiagonal object must receive CM->NumProcessors() contribuitons. Therefore at this points its version
		 * will CM->NumProcessors(). At this stage NProcs tasks for propagating the result obtained in the MainDiag object must be submitted to
		 * the parallel environment. In this implementation the ContributeWeightTask Tasks are going to be submitted from processor ZERO. Further
		 * studies can verify the implications of that choice.
		 * Wait tasks for each main stages are submitted as well.
		 */
		
		OOPWaitTask * wt = new OOPWaitTask(0);
		OOPDataVersion wtVersion;
		wtVersion.SetLevelVersion(0, CM->NumProcessors());
		
		wt->AddDependentData( OOPAccessTag(DiagId, EReadAccess, wtVersion,0));
    wt->Submit();
		cout << "Submitting WT with version " << wtVersion << endl;
		cout.flush();
    wt->Wait();
#ifdef LOGPZ
		{
			std::stringstream sout;
			sout << "Main Diagonal in processor ZERO Received NProcs contribuiton\n";
			sout << "Main thread in processor ZERO is continuing\n";
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif
    wt->Finish();
		
		/**
		 * Submitting ContributeWeightTask
		 */
		OOPDataVersion DDVersion;
		OOPContribWeightTask * ctask;
		for(i=0;i<nprocs;i++)
		{
			ctask = new OOPContribWeightTask(i);
			ctask->AddDependentData(OOPAccessTag(m_SubStrIds[i], EWriteAccess, DDVersion,0));
			ctask->Submit();			
		}

		
		/**
		 * For a distributed vector.
		 * Scatter data contained in local copy to the corresponding distributed parts
		 */
		
		//OOPCollector<TPZVec<double> > * collector;
		
		
#ifdef LOGPZ
		{
			std::stringstream sout;
			sout << "Leaving after WaitTask\n";
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif
		
    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->Submit();
    }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    tt->Submit();
		
	}
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Calling wait on all TMs\n";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	TM->Wait();
#ifdef LOGPZ
	{
		std::stringstream sout ;
		sout << "Leaving wait on all TMs\n";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Deleting DM on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	delete  DM;
#ifdef OOP_SOCKET
	//	((OOPSocketCommManager *)CM)->Barrier();
#endif
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Deleting TM on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	delete  TM;
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Deleting CM on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
#ifdef OOP_SOCKET
	//((OOPSocketCommManager *)CM)->Barrier();
#endif
	delete  CM;
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Leaving Application on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
  return 0;
}


int TestSerialization()
{
	CM = new OOPSocketCommManager;
	((OOPSocketCommManager *)CM)->Initialize();
	((OOPSocketCommManager *)CM)->Barrier();
	#ifdef LOG4CXX
	  std::stringstream sin;
	  sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
	  log4cxx::PropertyConfigurator::configure(sin.str());
	#endif


	#ifdef OOP_MPE
	  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
	  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
	  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
	  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
	#endif
	  TM = new OOPTaskManager (CM->GetProcID ());
	  DM = new OOPDataManager (CM->GetProcID ());


	  TM->SetNumberOfThreads(10);
	  TM->Execute();
	  if(!CM->GetProcID())
	  {

	  }

}

#include "TPZFParMatrix.h"
#include "OOPDumbCommMan.h"
#include "TPZCopySolve.h"

int TestFParMatrix()
{
	DM = new OOPDataManager(0);
	TM = new OOPTaskManager(0);
	CM = new OOPDumbCommManager;
	TM->Execute();
	int i, j;
	int msize;
	std::cout << "Dimens‹o \n";
	std::cin >> msize;
	std::cout << "Usando Dimens‹o " << msize << std::endl ;
	
	
	TPZFMatrix thefMat(msize,msize);
	TPZFMatrix fullv1(msize, 1);
	TPZFMatrix fullv2(msize, 1);

	for(i = 0; i < msize; i++)
	{
		for(j = i; j < msize; j++)
		{
			double val = random();
			thefMat.PutVal(i,j, val);
			thefMat.PutVal(j,i, val);
		}
		fullv1.PutVal(i, 0, i*100);
		thefMat.PutVal(i,i, thefMat.GetVal(i,i) * 10000);
	}
	TPZFParMatrix * par = new TPZFParMatrix(thefMat);
	
	
	TPZFParMatrix par2(thefMat);
	TPZFParMatrix res(thefMat.Rows(), thefMat.Cols());
	TPZFParMatrix res2(thefMat.Rows(), thefMat.Cols());
	
	TPZFParMatrix v1(fullv1);
	TPZFParMatrix v2(fullv2);
	TPZFParMatrix * v3 = new TPZFParMatrix(fullv2);
	
	TPZAutoPointer<TPZMatrix> pointerPar(par);
	
	TPZStepSolver solver(pointerPar);
	//(const int numiterations, const TPZMatrixSolver &pre, const REAL tol, const int FromCurrent)
	TPZCopySolve csolve(NULL);
	solver.SetCG(50, csolve, 0.01, 0);
	
	solver.Solve(v1, v2, v3);
	
/*
 {
		
		
//		TPZFParMatrix par2(thefMat);
//		TPZFParMatrix res(thefMat.Rows(), thefMat.Cols());
//		TPZFParMatrix res2(thefMat.Rows(), thefMat.Cols());
		
		//TPZFParMatrix v1(fullv1);
		//TPZFParMatrix v2(fullv2);
		TPZFMatrix * v3 = new TPZFMatrix(fullv2);
		
		TPZAutoPointer<TPZMatrix> pointerPar(par);
		
		TPZStepSolver solver(pointerPar);
		//(const int numiterations, const TPZMatrixSolver &pre, const REAL tol, const int FromCurrent)
		TPZCopySolve csolve(NULL);
		solver.SetCG(50, csolve, 0.01, 0);
		
		solver.Solve(fullv1, fullv2, v3);
	}	
*/	
	/**
	 thefMat.Print("Full", std::cout, EFormatted);
	double ddot = -1;
	ddot = Dot(par, par2);
	
	double ddotf = Dot(thefMat, thefMat);
	if(ddot == ddotf)
	{
		std::cout << "Dot Ok\n";
	}
	
	
	par.SynchronizeFromRemote();
	par.Print("Prior to ZAXPY", std::cout, EFormatted);
	
	par.ZAXPY(1.1, par2);
	{
		TPZFParMatrix::TPZAccessParMatrix accPar(par);
		accPar.GetMatrix().Print("After ZAXPY", std::cout, EFormatted);
	}
	
	par.Multiply(par2, res, 1, 1);
	{
		TPZFParMatrix::TPZAccessParMatrix accPar(par);
		accPar.GetMatrix().Print("After Multiply", std::cout, EFormatted);
	}
	{
		TPZFParMatrix::TPZAccessParMatrix accPar(res);
		accPar.GetMatrix().Print("After Multiply on Res", std::cout, EFormatted);
	}

	par.MultAdd( par2, res, res2, 1.1, 1.1, 1, 1);
	{
		{
			TPZFParMatrix::TPZAccessParMatrix accPar(res);
			accPar.GetMatrix().Print("After MultAdd on Res", std::cout, EFormatted);
		}
		{
			TPZFParMatrix::TPZAccessParMatrix accPar(res2);
			accPar.GetMatrix().Print("After MultAdd on Res2", std::cout, EFormatted);
		}
	}
	par.Zero();
	{
		TPZFParMatrix::TPZAccessParMatrix accPar(par);
		accPar.GetMatrix().Print("After Zero", std::cout, EFormatted);
	}
	
	*/
	
	OOPTerminationTask * tt = new OOPTerminationTask(0);
	tt->AddDependentData(OOPAccessTag(par->Id(), EReadAccess, par->Version(),0));
	tt->Submit();
	TM->Wait();
	return 0;
}
int main(int argc, char **argv)
{
  //debugmpimain(argc, argv); 
	//matmain(argc, argv);
  //debugmain(argc, argv);
	//TestFParMatrix();
	TestSerialization();

  return 0;
}


