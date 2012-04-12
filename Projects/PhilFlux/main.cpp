
#ifdef OOP_MPI
#include "mpi.h"
#include "oopmpicomm.h"
#include "OOPDataLogger.h"
#endif
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
//#include "oopfilecomm.h"
//#include "ooperror.h"
#include "TParVector.h"
#include "tsmalltask.h"
#include "oopwaittask.h"

#include "oopterminationtask.h"

//#include "fluxdefs.h"
#include <iostream>
#include <fstream>
#include <map>
#include <list>
using namespace std;

ofstream TaskLog;
ofstream DataLog;
ofstream DataManLog;
ofstream TransferDataLog;
ofstream TaskQueueLog;
ofstream TaskManLog;
ofstream DataQueueLog;

void InsertTasks(TPZAutoPointer<OOPTaskManager> TM, int numtasks);


struct DataAccessOrg {
	int fTaskid;
	int fProcid;
	int fDataId;
	int fVersion;
	OOPMDataState fAccessType;
	DataAccessOrg()
	{
		fTaskid=-1;
		fProcid=-1;
		fDataId=-1;
		fVersion=-1;
	}
	DataAccessOrg (int taskid,int procid, int id, int version, OOPMDataState access){
		fTaskid = taskid;
		fProcid = procid;
		fDataId = id;
		fVersion = version;
		fAccessType = access; 
	}
};

void WriteDataAccess(ostream &out, vector<OOPAccessTag> &list){
	cout << "Entering WriteDataAccess...";
	int i,nt = list.size();
	//  out << ": Number of tasks = " << endl;
	out << nt << endl;
	//  out << ": Tasks\n Data_Id \t Version \t Access type" << endl;
	for (i=0;i<nt;i++){
		out << list[i].Id().GetId();
		vector <int> level;
		vector <int> cardin;
		OOPDataVersion version = list[i].Version();
		version.GetData(cardin,level);
		int sz = level.size();
		for (int c=0;c<sz;c++){
			out  << "\t" << level[c] << "\t" << cardin[c]; 
		}
		out << "\t" << (int)(list[i].AccessMode()) << endl;
	}
	cout << " exiting!!!" << endl;
}

void ReadDataAccess (istream &arq, std::list<DataAccessOrg> &mylist) {
	cout << "Entering ReadDataAccess...";
	//first description line
	//  char buf[512];
	//  getline(&buf,512,);
	//second description line
	
	//  arq.getline();
	//  list.resize(nt);
	char buf[512];
	arq.getline(buf,512);
	while(arq) {
		DataAccessOrg ac;
		int accesstype;
		arq >> ac.fTaskid >> ac.fProcid >> ac.fDataId >> ac.fVersion >> accesstype;
		ac.fAccessType = (OOPMDataState)accesstype;
		if(!arq) break;
		mylist.push_back(ac);
	}
	cout << " exiting!!!" << endl;
}

void CreateObjIds (TPZAutoPointer<OOPTaskManager> TM, list<DataAccessOrg> &mylist, 
                   map<int,OOPObjectId,less<int> >&fileId2globalId,
                   vector<OOPAccessTag> &depend,
                   map<OOPObjectId,OOPAccessTag> &higVersions){
	
	cout << "Entering CreateObjIds...";
	int i,nt = mylist.size();
	depend.resize(nt);
	list<DataAccessOrg>::iterator ac_It;
	
	for (i=0,ac_It=mylist.begin();ac_It!=mylist.end();i++,ac_It++){
		if(i>=nt) break;
		//Verifies if the object_id was already created
		int file_id = (*ac_It).fDataId;//mylist[i].fDataId;
		map<int,OOPObjectId,less<int> >::iterator file_id_It;
		file_id_It = fileId2globalId.find(file_id);
		
		//Get the object_id
		OOPObjectId vicid;
		if (fileId2globalId.find(file_id) == fileId2globalId.end()){
			TParVector *victim = new TParVector();
			victim->Resize(10);
			vicid = TM->DM()->SubmitObject(victim);
			fileId2globalId [ file_id ] = vicid;
		} else {
			vicid = (*file_id_It).second;
		}
		//cout << "The file object id is " << mylist[i].fDataId << " the global id is " << vicid << endl;
		
		//Create the depend object
		OOPDataVersion localversion;
		localversion.SetLevelVersion(0,/*mylist[i]*/(*ac_It).fVersion);
		OOPAccessTag dp(vicid, /*mylist[i]*/(*ac_It).fAccessType , localversion, 0);
		depend [i] = dp;
		
		//Identify the higher versions for each object_id    
		map<OOPObjectId,OOPAccessTag>::iterator hig_It;
		hig_It = higVersions.find(vicid);
		if (hig_It == higVersions.end()){
			higVersions[vicid] = dp;
			continue;
		}
		if (dp.AccessMode()==EWriteAccess) dp.IncrementVersion();
		OOPDataVersion prevHg = ((*hig_It).second).Version();
		OOPDataVersion curr_ver = dp.Version();
		if (curr_ver > prevHg) {
			higVersions[vicid] = dp; 
		}
	}
	cout << " exiting!!!" << endl;
}


void CreateTaskFromFile(TPZAutoPointer<OOPTaskManager> TM, string &file) {
	int i;
	
	ifstream arq (file.c_str());
	list<DataAccessOrg> mylist;
	
	//Read the file
	ReadDataAccess (arq, mylist);
	
	//Create the depends
	map<int,OOPObjectId,less<int> > fileId2globalId;
	vector<OOPAccessTag> depend;
	map<OOPObjectId,OOPAccessTag> higVersions;
	
	CreateObjIds (TM, mylist, fileId2globalId, depend, higVersions);
	
	int /*it,*/ numproc = TM->CM()->NumProcessors();
	int lasttask = -1;
	list<DataAccessOrg>::iterator it;
	//  int numdeps = depend.size();
	TSmallTask *st = 0;
	int counter;
	for(it=mylist.begin(),counter=0; it!=mylist.end(); it++,counter++) {
		int task = (*it).fTaskid;
		if(task != lasttask) {
			cout << "task = " << task << " lasttask = " << lasttask << endl;
			lasttask = task;
			if(st) 
			{
				OOPObjectId id = TM->Submit(st);
				cout << __PRETTY_FUNCTION__ << " submitted task id " << id << " task " << lasttask << endl;
			}
			st = new TSmallTask((*it).fProcid);
		}
		st->AddDependentData(depend[counter]);
	}
	OOPObjectId id = TM->Submit(st);
	cout << __PRETTY_FUNCTION__ << " submitted task id " << id << " task " << lasttask << endl;
	
	// build a task such that the current thread has access to internal OOP data
	OOPWaitTask *wt = new OOPWaitTask(TM->CM()->GetProcID());
	map<OOPObjectId,OOPAccessTag>::iterator hig_It;
	for (hig_It = higVersions.begin();hig_It != higVersions.end();hig_It++){
		OOPAccessTag depend = (*hig_It).second;
		wt->AddDependentData(depend);      
	}  
	cout << "Before sumitting the wait task" << endl;
	TM->Submit(wt);
	// This will put the current thread to wait till the data is available
	cout << "Before waiting\n";
	wt->Wait();
	cout << "Got out of wait\n";
	// At this point I have version access to victim
	//     OOPMetaData *obj = wt->Depend().Dep(0).ObjPtr();
	//     ver= obj->Version();
	//     ver.IncrementLevel(numtasks);
	//     obj->SetVersion(ver,wt->Id());
	//     cout << "Before wait finish\n";
	wt->Finish();
	cout << "After wait finish\n";
	cout << "I GOT THROUGH\n";
	
	OOPTerminationTask * tt;
	for(i=0;i < numproc;i++){
		tt = new OOPTerminationTask(i);
		TM->Submit(tt);
	}
}

#include "oopinitializeenvironment.h"
#include "pzlog.h"
#include "OOPDataLogger.h"

int main (int argc, char **argv)
{

	InitializePZLOG();
    TPZAutoPointer<OOPTaskManager> TM = InitializeEnvironment(argc, argv, 2, 4);
	char filename[256];
	sprintf(filename,"datalogger%d.log", TM->CM()->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
	::LogDM = LogDM;
	sprintf(filename,"tasklog%d.log", TM->CM()->GetProcID());  
	TaskLog.open(filename);
	sprintf(filename,"datalog%d.log", TM->CM()->GetProcID());  
	DataLog.open(filename);
	sprintf(filename,"datamanlog%d.log", TM->CM()->GetProcID());  
	DataManLog.open(filename);
	sprintf(filename,"transferdatalog%d.log", TM->CM()->GetProcID());  
	TransferDataLog.open(filename);
	sprintf(filename,"taskqueue%d.log", TM->CM()->GetProcID());  
	TaskQueueLog.open(filename);
	sprintf(filename,"taskmanlog%d.log", TM->CM()->GetProcID());  
	TaskManLog.open(filename);
	sprintf(filename,"dataqueuelog%d.log", TM->CM()->GetProcID());  
	DataQueueLog.open(filename);
	
	if(TM->CM()->IAmTheMaster())
	{
		cout << "Inserting tasks\n";
		cout.flush();
		InsertTasks(TM,30);
		//string file ("tasks.txt");
		//CreateTaskFromFile(TM,file);
	} else {
		cout << "IAmTheMaster returned " << TM->CM()->IAmTheMaster() << endl;
		cout.flush();
	}
	
    ShutDownEnvironment(TM);
	delete LogDM;
	
	cout << "Leaving main\n";
	cout.flush();
	return 0;
}

void InsertTasks(TPZAutoPointer<OOPTaskManager> TM, int numtasks) 
{
	TParVector *victim = new TParVector();
	victim->Resize(1000);
	
	OOPObjectId vicid = TM->DM()->SubmitObject(victim);
	
	cout << "The object id for victim is " << vicid << endl;
	// build a task such that the current thread has access to internal OOP data
	OOPWaitTask *wt = new OOPWaitTask(TM->CM()->GetProcID());
	OOPDataVersion ver;
	wt->AddDependentData(OOPAccessTag (vicid,EWriteAccess,ver, 0));
	cout << "Before sumitting the wait task" << endl;
	TM->Submit(wt);
	// This will put the current thread to wait till the data is available
	cout << "Before waiting\n";
	wt->Wait();
	cout << "Got out of wait\n";
	// At this point I have version access to victim
    OOPAccessTagList &tags = wt->GetDependencyList();
    ver.SetLevelVersion(0, 1);
	ver.IncrementLevel(numtasks+1);
    tags.SetVersion(0, ver);
    std::cout << "Setting the version of the object to " << ver << " class id " << tags.ObjectPtr(0)->ClassId() <<  std::endl;
    //wt->TaskFinished();
	//  obj->SetVersion(ver,wt->Id());
	cout << "Before wait finish\n";
	wt->Finish();
	cout << "After wait finish\n";
	int it, numproc = TM->CM()->NumProcessors();
	for(it=0; it<numtasks; it++) {
		TSmallTask *st = new TSmallTask(it%numproc);
		OOPDataVersion stver;
        stver.SetLevelVersion(0, 1);
		stver.IncrementLevel(-1);
		stver.SetLevelVersion(1,it+1);
		cout << "Created the smalltask numproc " << numproc << "\n";
		if((numproc>1) && it%(numproc-1)) 
		{
			cout << "Adding data dependency with version " << stver<< "\n";
			cout.flush();
			OOPAccessTag dp(vicid,EWriteAccess,stver, 0);
			st->AddDependentData(dp);
		}
		else
		{
			cout << "Adding data dependency with version " << stver<< "\n";
			cout.flush();
			OOPAccessTag dp(vicid,EWriteAccess,stver , 0);
			st->AddDependentData(dp);
		}
		cout << "Submitting the small task\n";
		TM->Submit(st);
	}
	cout << "Before another wait task\n";
	wt = new OOPWaitTask(TM->CM()->GetProcID());
	ver = OOPDataVersion();
	ver.Increment();
    ver.Increment();
	wt->AddDependentData(OOPAccessTag(vicid,EWriteAccess,ver, 0));
	TM->Submit(wt);
	// This will put the current thread to wait till the data is available
	wt->Wait();
	wt->Finish();
	cout << "I GOT THROUGH\n";
	
}
