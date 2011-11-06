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
#elif OOP_SOCKET
#include "oopsocketcommmanager.h"
#endif

#ifdef OOP_INTERNAL
#include "oopinternalcommanager.h"
#endif

#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
OOPGenericLockService gMutex;
#endif

#ifdef OOP_INTERNAL
const int NumProcessors = 2;
void SetupEnvironment(TPZVec<TPZAutoPointer<OOPTaskManager> > &TMVec);
#endif

#include <iostream>
#include <sstream>

#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopwaittask.h"
#include "oopsnapshottask.h"
#include "oopterminationtask.h"
#include "pzlog.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif
#include "oopaccesstag.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif

// Dohr includes
#include "tpzdohrassembly.h"
#include "tpzdohrsubstruct.h"

//Parallel Dohr includes
#include "OOPComputeMatricesTask.h"
#include "OOPContribWeightTask.h"

using namespace std;

//OOPCommunicationManager *CM;
OOPDataManager *DM;
//OOPTaskManager *TM;

const int NumThreads = 10;

int matmain(int argc, char **argv)
{
  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0][1] << std::endl;
  std::cout.flush();
  OOPCommunicationManager * CM;
#ifdef OOP_MPI
  CM = new OOPMPICommManager(argc, argv);
#elif OOP_SOCKET
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
  OOPTaskManager * TM = new OOPTaskManager(CM->GetProcID());
  DM = new OOPDataManager(CM->GetProcID(), TM->TM());

  TM->SetDataManager(DM->DM());
  TM->SetCommunicationManager(CM->CM());

  CM->SetTaskManager(TM->TM());

  TM->SetNumberOfThreads(10);
  TM->Execute();
  if (!CM->GetProcID())
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
    for (i = 0; i < nprocs; i++)
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
    for (i = 0; i < nprocs; i++)
      {
      cdiagt = new OOPComputeMatricesTask(i);
      cdiagt->AddDependentData(OOPAccessTag(AssemblyId, EReadAccess,
          assemblyVersion, 0));
      cdiagt->AddDependentData(OOPAccessTag(m_SubStrIds[i], EReadAccess,
          assemblyVersion, 0));
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

    wt->AddDependentData(OOPAccessTag(DiagId, EReadAccess, wtVersion, 0));
    TM->Submit(wt);
    cout << "Submitting WT with version " << wtVersion << endl;
    cout.flush();
    wt->Wait();
#ifdef LOG4CXX
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
    for (i = 0; i < nprocs; i++)
      {
      ctask = new OOPContribWeightTask(i);
      ctask->AddDependentData(OOPAccessTag(m_SubStrIds[i], EWriteAccess,
          DDVersion, 0));
      TM->Submit(ctask);
      }

    /**
     * For a distributed vector.
     * Scatter data contained in local copy to the corresponding distributed parts
     */

    //OOPCollector<TPZVec<double> > * collector;


#ifdef LOG4CXX
    {
      std::stringstream sout;
      sout << "Leaving after WaitTask\n";
      LOGPZ_DEBUG(logger, sout.str().c_str());
    }
#endif

    for (i = 1; i < CM->NumProcessors(); i++)
      {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      TM->Submit(tt);
      }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    TM->Submit(tt);

    }
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Calling wait on all TMs\n";
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
  TM->Wait();
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Leaving wait on all TMs\n";
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Deleting DM on Processor " << CM->GetProcID();
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
#ifdef OOP_SOCKET
  //    ((OOPSocketCommManager *)CM)->Barrier();
#endif
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Deleting TM on Processor " << CM->GetProcID();
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Deleting CM on Processor " << CM->GetProcID();
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
#ifdef OOP_SOCKET
  //((OOPSocketCommManager *)CM)->Barrier();
#endif
#ifdef LOG4CXX
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
  OOPCommunicationManager * CM;
#ifdef OOP_SOCKET
  CM = new OOPSocketCommManager;
  ((OOPSocketCommManager *)CM)->Initialize();
  ((OOPSocketCommManager *)CM)->Barrier();
#endif
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
  OOPTaskManager *TM = new OOPTaskManager(CM->GetProcID());
  DM = new OOPDataManager(CM->GetProcID(), TM->TM());
  TM->SetCommunicationManager(CM->CM());
  TM->SetDataManager(DM->DM());
  CM->SetTaskManager(TM->TM());

  TM->SetNumberOfThreads(10);
  TM->Execute();
  if (!CM->GetProcID())
    {

    }

}



#include "TPZFParMatrix.h"
#include "OOPDumbCommMan.h"
#include "TPZCopySolve.h"

int TestFParMatrix()
{
  OOPCommunicationManager * CM;
#ifdef OOP_SOCKET
  CM = new OOPSocketCommManager;
#endif
#ifdef OOP_INTERNAL
  CM = new OOPInternalCommunicationManager(0,NumProcessors);
#endif
  OOPTaskManager * TM = new OOPTaskManager(CM->GetProcID());
  DM = new OOPDataManager(CM->GetProcID(), TM->TM());
  //OOPDumbCommManager * CM = new OOPDumbCommManager;


#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#endif

#ifdef OOP_INTERNAL
  TPZVec<TPZAutoPointer<OOPTaskManager> > TMVec(NumProcessors);
  TMVec[0] = TM->TM();
#endif

  TM->SetDataManager(DM->DM());
  CM->SetTaskManager(TM->TM());
  TM->SetCommunicationManager(CM->CM());

#ifdef OOP_INTERNAL
  SetupEnvironment(TMVec);
#endif

  TM->Execute();
#ifdef OOP_SOCKET
  ((OOPSocketCommManager *)CM)->Barrier();
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "All processors synchronized with Barrier\n";
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif

 #endif

  if (!CM->GetProcID())
  {
    int i, j;
    int msize = 10;
    //std::cout << "Dimens‹o \n";
    //std::cin >> msize;
    std::cout << "Usando Dimens‹o " << msize << std::endl;

    TPZFMatrix thefMat(msize, msize);
    TPZFMatrix fullv1(msize, 1);
    TPZFMatrix fullv2(msize, 1);

    for (i = 0; i < msize; i++)
      {
      for (j = i; j < msize; j++)
        {
        double val = random();
        thefMat.PutVal(i, j, val);
        thefMat.PutVal(j, i, val);
        }
      fullv1.PutVal(i, 0, i * 100);
      thefMat.PutVal(i, i, thefMat.GetVal(i, i) * 10000);
      }
    TPZFParMatrix * par = new TPZFParMatrix(thefMat);

    TPZFParMatrix par2(thefMat);
    TPZFParMatrix res(thefMat.Rows(), thefMat.Cols());
    TPZFParMatrix res2(thefMat.Rows(), thefMat.Cols());

    TPZFParMatrix * v1 = new TPZFParMatrix(fullv1);
    TPZFParMatrix * v2 = new TPZFParMatrix(fullv2);
    TPZFParMatrix * v3 = new TPZFParMatrix(fullv2);

    TPZAutoPointer<TPZMatrix> pointerPar(par);

    TPZStepSolver solver(pointerPar);
    //(const int numiterations, const TPZMatrixSolver &pre, const REAL tol, const int FromCurrent)
    TPZCopySolve csolve(NULL);
    solver.SetCG(50, csolve, 0.01, 0);
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Versions prior to Solve v1 " << v1->GetVersionAfterWrites(0);
    sout << " v2 " << v2->GetVersionAfterWrites(0) << " v3 " << v3->GetVersionAfterWrites(0) << endl;
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
    solver.Solve(*v1, *v2, v3);
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Versions After to Solve v1 " << v1->GetVersionAfterWrites(0);
    sout << " v2 " << v2->GetVersionAfterWrites(0) << " v3 " << v3->GetVersionAfterWrites(0) << endl;
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
    OOPDataVersion vv1, vv2, vv3;
    vv1 = v1->GetVersionAfterWrites(0);
    vv2 = v2->GetVersionAfterWrites(0);
    vv3 = v3->GetVersionAfterWrites(0);

    OOPWaitTask * wt = new OOPWaitTask(CM->GetProcID());
    wt->AddDependentData(OOPAccessTag(v1->Id(), EWriteAccess,
        vv1, CM->GetProcID()));
    wt->AddDependentData(OOPAccessTag(v2->Id(), EWriteAccess,
        vv2, CM->GetProcID()));
    wt->AddDependentData(OOPAccessTag(v3->Id(), EWriteAccess,
        vv3, CM->GetProcID()));

    CM->TM()->Submit(wt);
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Submitting WaitTask dependent on Versions for v1 " << v1->GetVersionAfterWrites(0);
    sout << " v2 " << v2->GetVersionAfterWrites(0) << " v3 " << v3->GetVersionAfterWrites(0) << endl;
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
    wt->Wait();
    wt->Finish();

#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Submitting TerminatioTask for all processors\n" << endl;
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
    for (i = 1; i < CM->NumProcessors(); i++)
      {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->SetProcOrigin(CM->GetProcID());
      CM->TM()->Submit(tt);
      }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    tt->SetProcOrigin(CM->GetProcID());
    CM->TM()->Submit(tt);
  }

  CM->TM()->Wait();
#ifdef OOP_SOCKET
  ((OOPSocketCommManager *)CM)->Barrier();
#endif
  //TM->ClearPointer();
  //DM->ClearPointer();
  //CM->ClearPointer();
#ifdef OOP_INTERNAL
  int iproc;
  for(iproc=1; iproc < NumProcessors; iproc++)
  {
    TMVec[iproc]->Wait();
    TMVec[iproc]->ClearPointer();
    TMVec[iproc] = TPZAutoPointer<OOPTaskManager>(0);
  }
#endif
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout << "Leaving wait on all TMs\n";
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif
  return 0;
}

int main(int argc, char **argv)
{
  //debugmpimain(argc, argv);
  //matmain(argc, argv);
  //debugmain(argc, argv);
  TestFParMatrix();
  //TestSerialization();

  return 0;
}

#ifdef OOP_INTERNAL
void SetupEnvironment(TPZVec<TPZAutoPointer<OOPTaskManager> >&TMVec)
{
  int iproc, jproc;
  std::vector<OOPInternalCommunicationManager *> AllCMp(NumProcessors);
  std::vector<TPZAutoPointer<OOPCommunicationManager> > AllCM(NumProcessors);
  AllCM[0] = TMVec[0]->CM();
  AllCMp[0] = dynamic_cast<OOPInternalCommunicationManager *>(TMVec[0]->CM().operator->());
  if(!AllCMp[0])
    {
    cout << "SetupEnvironment will only work with OOPInternalCommunicationManager\n";
    exit(-1);
    }
  for(iproc=1; iproc<NumProcessors; iproc++)
    {
    OOPTaskManager *pTM;
    OOPDataManager *pDM;
    AllCMp[iproc]
           = new OOPInternalCommunicationManager(iproc, NumProcessors);
    AllCM[iproc] = AllCMp[iproc]->CM();
    pTM = new OOPTaskManager(AllCM[iproc]->GetProcID());
    TPZAutoPointer<OOPTaskManager> TM(pTM->TM());
    TMVec[iproc] = TM;
    pTM = 0;
    TM->SetCommunicationManager(AllCM[iproc]);
    AllCM[iproc]->SetTaskManager(TM);
    pDM = new OOPDataManager(AllCM[iproc]->GetProcID(), TM);
    TPZAutoPointer<OOPDataManager> DM(pDM->DM());
    pDM = 0;
    TM->SetDataManager(DM);

    TM->SetNumberOfThreads(NumThreads);
    TM->Execute();
    }
  for(iproc=0; iproc<NumProcessors; iproc++)
    {
    for(jproc=0; jproc<NumProcessors; jproc++)
      {
      AllCMp[iproc]->SetCommunicationManager(jproc,AllCM[jproc]);
      }
    AllCM[iproc]->Initialize("Dummy",-1);
    }
}
#endif

