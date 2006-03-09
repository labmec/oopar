//
// C++ Implementation: cesartest
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <cesar@labmec.fec.unicamp.br>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>

//OOPar includes
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopfilecomm.h"
//#include "ooperror.h"
#include "oopwaittask.h"
//#include "fluxdefs.h"
#include "OOPVector.h"
#include "OOPDouble.h"


#ifdef MPI
#include "oopmpicomm.h"
#endif

#include "tmedonhotask.h"

// include log4cxx header files.
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

// Define a static logger variable so that it references the
// Logger instance named "MyApp".
static LoggerPtr logger(Logger::getLogger("MainApp"));

//OOPar global objects
OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;
void SubmitMed(int proc, OOPObjectId &xid, OOPObjectId &yid, OOPObjectId &crossid, OOPObjectId &dotid, 
   OOPDataVersion &verread, OOPDataVersion &verwrite,int first, int last);


#ifdef MPI
int mpimain (int argc, char **argv);
#endif

void FillMedonho(TMedonhoTask &med);

int main(int argc, char **argv) {
  int result = EXIT_SUCCESS;
  cout << "merdatry\n";
  try {
    // BasicConfigurator replaced with PropertyConfigurator.
    cout << "merdatry1\n";
    PropertyConfigurator::configure("/mnt/hd/cesar/OOPar/debug/Projects/TestCesar/log4cxx.cfg");
    cout << "merdatry2\n";
    LOG4CXX_INFO(logger, "Entering Main Application.");
    cout << "merdatry3\n";
#ifdef MPI
    cout << "merdain\n" ;
    result =  mpimain(argc,argv);
    cout << "merdaout\n";
#else
    LOG4CXX_INFO(logger,"Exiting program...Not available mpi");
    result =  0;
#endif
    LOG4CXX_INFO(logger, "Exiting application.");
  }
  catch(Exception&){
    cout << "...erro..."<< endl;
    result = EXIT_FAILURE;
  }
  cout << "The end of sob\n";
  return result;
}


#ifdef MPI
int mpimain (int argc, char **argv)
{
  LOG4CXX_INFO(logger,"Entering mpimain program");
  CM = new OOPMPICommManager (argc, argv);
  CM->Initialize((char*)argv, argc);
  
  char filename[256];
  sprintf(filename,"datalogger%d.log", CM->GetProcID());
  OOPDataLogger * LogDM = new OOPDataLogger(filename);
  ::LogDM = LogDM;
//   sprintf(filename,"tasklog%d.log", CM->GetProcID());  
//   TaskLog.open(filename);
//   sprintf(filename,"datalog%d.log", CM->GetProcID());  
//   DataLog.open(filename);
//   sprintf(filename,"datamanlog%d.log", CM->GetProcID());  
//   DataManLog.open(filename);
//   sprintf(filename,"transferdatalog%d.log", CM->GetProcID());  
//   TransferDataLog.open(filename);
//   sprintf(filename,"taskqueue%d.log", CM->GetProcID());  
//   TaskQueueLog.open(filename);
//   sprintf(filename,"taskmanlog%d.log", CM->GetProcID());  
//   TaskManLog.open(filename);
//   sprintf(filename,"dataqueuelog%d.log", CM->GetProcID());  
//   DataQueueLog.open(filename);

  TM = new OOPTaskManager (CM->GetProcID ());
  DM = new OOPDataManager (CM->GetProcID ());

  //int numproc = CM->NumProcessors();//atoi(argv[argc-1]);
  // At this point the environment will lock because it will go into a blocking receive...
  {
    stringstream sout;
    sout << "Entering execute for " << CM->GetProcID() << endl;
    LOG4CXX_DEBUG(logger,sout.str());
  }
  TM->Execute();
  {
    LOG4CXX_DEBUG(logger,"After TM->Execute");
  }

  if(CM->IAmTheMaster())
  {
    LOG4CXX_INFO(logger,"Inserting tasks");
    const int size = 100;
    //inserir aqui as tarefas...
    //InsertTasks(30);
    //string file ("tasks.txt");
//    CreateTaskFromFile(file);
    OOPVector<double> *x, *y, *cross;
    OOPDouble *dot;
    OOPObjectId xid,yid,crossid, dotid;
    x = new OOPVector<double>();
    x->fVecValue.Resize(size);
    y = new OOPVector<double>();
    y->fVecValue.Resize(size);
    cross = new OOPVector<double>();
    cross->fVecValue.Resize(size);
    dot = new OOPDouble;
    xid = DM->SubmitObject(x,1);
    yid = DM->SubmitObject(y,1);
    crossid = DM->SubmitObject(cross,1);
    dotid = DM->SubmitObject(dot,1);
    OOPDataVersion verread, verwrite;
    ++verwrite;
    ++verread;
    int numproc = CM->NumProcessors();
    int ipr = 0;
    
    int first = 0;
    int d = size / numproc;
    int last = d;
    
    for(ipr = 0; ipr<numproc-1; ipr++)
    {
      SubmitMed(ipr,xid,yid,crossid,dotid,verread,verwrite,first,last);
      first = last;
      last += d;
    }
    SubmitMed(ipr,xid,yid,crossid,dotid,verread,verwrite,first,size);
    {
      // x,y,cross,dot deixarem de ser acessivel
      OOPWaitTask *wt = new OOPWaitTask(CM->GetProcID());
      OOPDataVersion ver;
      OOPMDataDepend depx(xid,EWriteAccess,ver);
      OOPMDataDepend depy(yid,EWriteAccess,ver);
      OOPMDataDepend depcross(crossid,EWriteAccess,ver);
      OOPMDataDepend depdot(dotid,EWriteAccess,ver);
      wt->AddDependentData(depx);
      wt->AddDependentData(depy);
      wt->AddDependentData(depcross);
      wt->AddDependentData(depdot);
      
      wt->Submit();
      
      wt->Wait();
      
      x = dynamic_cast<OOPVector <double > *>(wt->GetDepObjPtr(0));
      y = dynamic_cast<OOPVector <double > *>(wt->GetDepObjPtr(1));
      cross = dynamic_cast<OOPVector <double > *>(wt->GetDepObjPtr(2));
      dot = dynamic_cast<OOPDouble *>(wt->GetDepObjPtr(3));
      
      // initializar os dados      
      int i;
      for (i=0;i<100;i++){
        x->fVecValue[i] = (double) (i+1);
        y->fVecValue[i] = 1. / ((double) (i+1));
      }
      dot->fValue = 0.;
      
      wt->IncrementWriteDependentData();
      wt->Finish();
    }
    {
      // x,y,cross,dot deixarem de ser acessivel
      OOPWaitTask *wt = new OOPWaitTask(CM->GetProcID());
      OOPDataVersion ver;
      OOPMDataDepend depx(xid,EWriteAccess,verread);
      OOPMDataDepend depy(yid,EWriteAccess,verread);
      OOPMDataDepend depcross(crossid,EWriteAccess,verwrite);
      OOPMDataDepend depdot(dotid,EWriteAccess,verwrite);
      wt->AddDependentData(depx);
      wt->AddDependentData(depy);
      wt->AddDependentData(depcross);
      wt->AddDependentData(depdot);
      
      wt->Submit();
      
      wt->Wait();
      
      x = dynamic_cast<OOPVector<double > *>(wt->GetDepObjPtr(0));
      y = dynamic_cast<OOPVector<double > *>(wt->GetDepObjPtr(1));
      cross = dynamic_cast<OOPVector<double > *>(wt->GetDepObjPtr(2));
      dot = dynamic_cast<OOPDouble *>(wt->GetDepObjPtr(3));
      
      // ver o resultado----------------------------      
      int i,j,siz = cross->fVecValue.NElements();
      cout << "Dot = " <<  dot->fValue << endl;
      for (i=0;i<siz;i++){
         if (!(i%10)) cout << endl;
         cout << cross->fVecValue[i] << "\t";
      }
      cout << endl;
      //---------------------------------fim resultado
      wt->IncrementWriteDependentData();
      wt->Finish();
    }
    int i;
    OOPTerminationTask * tt;
    for(i=numproc-1;i >= 0;i--){
      tt = new OOPTerminationTask(i);
      tt->Submit();
    }
  }

  TM->Wait();
  cout << "Deleting DM\n";
  delete  DM;
  cout << "Deleting TM\n";
  delete  TM;
  cout << "Deleting CM\n";
  delete  CM;
  delete LogDM;

  cout << "Leaving mpimain\n";
  cout.flush();
  return 0;
}
#endif

void SubmitMed(int proc, OOPObjectId &xid, OOPObjectId &yid, OOPObjectId &crossid, OOPObjectId &dotid, 
   OOPDataVersion &verread, OOPDataVersion &verwrite,int first, int last)
{
  TMedonhoTask *med = new TMedonhoTask (0,first,last);
  OOPMDataDepend depx(xid,EReadAccess,verread);
  OOPMDataDepend depy(yid,EReadAccess,verread);
  OOPMDataDepend depcross(crossid,EWriteAccess,verwrite);
  OOPMDataDepend depdot(dotid,EWriteAccess,verwrite);
  med->AddDependentData(depx);
  med->AddDependentData(depy);
  med->AddDependentData(depcross);
  med->AddDependentData(depdot);
  med->Submit();
  ++verwrite;
}
