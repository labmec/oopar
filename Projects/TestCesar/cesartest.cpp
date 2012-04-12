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

// including log4cxx header files.
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


void SubmitMed(TPZAutoPointer<OOPTaskManager> TM, int proc, OOPObjectId &xid, OOPObjectId &yid, OOPObjectId &crossid, OOPObjectId &dotid, 
			   OOPDataVersion &verread, OOPDataVersion &verwrite,int first, int last);


#include "oopinitializeenvironment.h"
#include "pzlog.h"
#include "OOPDataLogger.h"
void FillMedonho(TMedonhoTask &med);

int main(int argc, char **argv) {
    
    InitializePZLOG();
    int numproc = 2;
    int numthread = 3;
    TPZAutoPointer<OOPTaskManager> TM = InitializeEnvironment(argc,argv,numproc,numthread);
	char filename[256];
	sprintf(filename,"datalogger%d.log", TM->CM()->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
	::LogDM = LogDM;
	
		
	if(TM->CM()->IAmTheMaster())
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
		xid = TM->DM()->SubmitObject(x);
		yid = TM->DM()->SubmitObject(y);
		crossid = TM->DM()->SubmitObject(cross);
		dotid = TM->DM()->SubmitObject(dot);
		OOPDataVersion verread, verwrite;
		++verwrite;
		++verread;
		int numproc = TM->CM()->NumProcessors();
		int ipr = 0;
		
		int first = 0;
		int d = size / numproc;
		int last = d;
		
		for(ipr = 0; ipr<numproc-1; ipr++)
		{
			SubmitMed(TM,ipr,xid,yid,crossid,dotid,verread,verwrite,first,last);
			first = last;
			last += d;
		}
		SubmitMed(TM,ipr,xid,yid,crossid,dotid,verread,verwrite,first,size);
		{
			// x,y,cross,dot deixarem de ser acessivel
			OOPWaitTask *wt = new OOPWaitTask(TM->CM()->GetProcID());
			OOPDataVersion ver;

            OOPAccessTag depx(xid, EWriteAccess, ver,0);
			OOPAccessTag depy(yid,EWriteAccess,ver,0);
			OOPAccessTag depcross(crossid,EWriteAccess,ver,0);
			OOPAccessTag depdot(dotid,EWriteAccess,ver,0);
			wt->AddDependentData(depx);
			wt->AddDependentData(depy);
			wt->AddDependentData(depcross);
			wt->AddDependentData(depdot);
			
			TM->Submit(wt);
			
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
			OOPWaitTask *wt = new OOPWaitTask(TM->CM()->GetProcID());
			OOPDataVersion ver;
			OOPAccessTag depx(xid,EWriteAccess,verread,0);
			OOPAccessTag depy(yid,EWriteAccess,verread,0);
			OOPAccessTag depcross(crossid,EWriteAccess,verwrite,0);
			OOPAccessTag depdot(dotid,EWriteAccess,verwrite,0);
			wt->AddDependentData(depx);
			wt->AddDependentData(depy);
			wt->AddDependentData(depcross);
			wt->AddDependentData(depdot);
			
			TM->Submit(wt);
			
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
        
    }
    ShutDownEnvironment(TM);
	delete LogDM;
	
	cout << "Leaving main\n";
	cout.flush();
	return 0;
}

void SubmitMed(TPZAutoPointer<OOPTaskManager> TM, int proc, OOPObjectId &xid, OOPObjectId &yid, OOPObjectId &crossid, OOPObjectId &dotid, 
			   OOPDataVersion &verread, OOPDataVersion &verwrite,int first, int last)
{
	TMedonhoTask *med = new TMedonhoTask (0,first,last);
	OOPAccessTag depx(xid,EReadAccess,verread,proc);
	OOPAccessTag depy(yid,EReadAccess,verread,proc);
	OOPAccessTag depcross(crossid,EWriteAccess,verwrite,proc);
	OOPAccessTag depdot(dotid,EWriteAccess,verwrite,proc);
	med->AddDependentData(depx);
	med->AddDependentData(depy);
	med->AddDependentData(depcross);
	med->AddDependentData(depdot);
	TM->Submit(med);
	++verwrite;
}
