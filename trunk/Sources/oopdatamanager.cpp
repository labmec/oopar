// -*- c++ -*-
#include "oopdatamanager.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"
#include "ooppardefs.h"
#include "oopcommmanager.h"
#include <map>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>
//#include "../gnu/gnudefs.h"
//Includes for testing
//#include "tmultidata.h"
//#include "tmultitask.h"
class   OOPStorageBuffer;
class   OOPStorageBuffer;
class   OOPDataVersion;
class   OOPMetaData;
class   OOPDMRequestTask;
class   OOPDMOwnerTask;
class   OOPSaveable;
using namespace std;
class   OOPObjectId;

#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
LoggerPtr loggerOOPDataManager(Logger::getLogger("OOPAR.OOPDataManager"));
#endif

//#include "pzvec.h"
/*
void OOPDataManager::main(){
	
	OOPObjectId id[40], tid;
	TMultiTask mytask(0);
	TDataVersion ver(10);
	TMultiData dat[40];
	int i=0;
	OOPMDataState st = EReadAccess;
	tid=TM->Submit(&mytask);
	for(i=0;i<40;i++){
		id[i] = DM->SubmitObject(&dat[i], 0);
		DataManLog << "Data Id set to :" ;
		id[i].Print(DataManLog);
		DataManLog.flush();
	}
	
	tid.Print(DataManLog);
	
	for(i=0;i<40;i++){		
		DM->SubmitAccessRequest(tid, id[i], ver, st, 0);
	}
	
	DataManLog << "Deleting Object " << endl;
	id[20].Print(DataManLog);
	DM->DeleteObject(id[20]);
	DM->DeleteObject(id[20]);
	
	if(DM->FindObject(id[20])) DataManLog << "Object found" << endl;
		
	OOPSaveable * ptr = (TMultiData *) DM->GetObjPtr(id[30]);
	int proc;
	proc = DM->GetProcID();
	
	TDataVersion ver2 = dat[10].Version();
	ver2.Print(DataManLog);
	for(i=0;i<30;i++){
		DM->IncrementVersion(id[10]);
	}
	
	DataManLog << DM->HasAccess(id[10], tid, st, ver) << endl; 
	
	TMultiData dat_m;
	OOPObjectId id_m;
	id_m.SetProcId(2);
	id_m.SetId(20);
		
	DataManLog << DM->HasAccess(id_m, tid, st, ver) << endl; 
	
	TMultiTask tm_a(0);
	TMultiData md_a ;
	
	OOPObjectId id_da, id_ta;
	id_da = DM->SubmitObject(&md_a,0);
	id_da.Print(DataManLog);
	id_ta = TM->Submit(&tm_a);
	id_ta.Print(DataManLog);
	
	TDataVersion versao;
	versao.SetLevelVersion(0,5);
	tm_a.AddDependentData(id_da, st, versao);
	DM->SubmitAccessRequest(id_ta, id_da, versao, st, 0);
	
	//OOPMetaData * md_ad = DM->Data(id_da);
	DM->Data(id_da)->SetCardinality(0,10);
	DM->Data(id_da)->IncrementVersionLevel(12);
	versao = DM->Data(id_da)->Version();
	
	versao.Print(DataManLog);
	DM->Data(id_da)->IncrementVersionLevel(25);
	versao = DM->Data(id_da)->Version();
	versao.Print(DataManLog);
	i=0;
	while(i<60){
		DM->IncrementVersion(id_da);
		i++;
	}
	DM->Data(id_da)->SetCardinality(0,100);
	DM->Data(id_da)->IncrementVersionLevel(12);
	DM->Data(id_da)->IncrementVersionLevel(12);
	DM->Data(id_da)->IncrementVersionLevel(12);
	DM->Data(id_da)->IncrementVersionLevel(12);
	versao = DM->Data(id_da)->Version();
	versao.Print(DataManLog);
	DM->Data(id_da)->DecreaseVersionLevel();
	versao = DM->Data(id_da)->Version();
	versao.Print(DataManLog);
	DM->Data(id_da)->DecreaseVersionLevel();
	versao = DM->Data(id_da)->Version();
	versao.Print(DataManLog);
	DM->Data(id_da)->DecreaseVersionLevel();
	versao = DM->Data(id_da)->Version();
	versao.Print(DataManLog);
	
}
*/
OOPDataManager::~OOPDataManager ()
{
	map< OOPObjectId,  OOPMetaData * >::iterator i=fObjects.begin ();
	for(;i!=fObjects.end();i++){
            TPZSaveable *dead = (*i).second->Ptr();
            delete dead;
			delete (i->second);
		  	fObjects.erase(i);
	}
	fObjects.clear ();
#ifdef LOG4CXX
  LOG4CXX_WARN(loggerOOPDataManager,"Terminating DM");
#endif 
	DataManLog << "Terminating DM\n";
	DataManLog.flush();
}
bool OOPDataManager::HasObject (OOPObjectId & id)
{
	  map< OOPObjectId, OOPMetaData *>::iterator i;
	  i=fObjects.find(id);
      if(i!=fObjects.end())
            return true;
            else
            return false;
}
void OOPDataManager::ReleaseAccessRequest (const OOPObjectId & TaskId, const OOPMDataDepend & depend){
      map<OOPObjectId, OOPMetaData *>::iterator it;
      it=fObjects.find(depend.Id());
      if(it!=fObjects.end()){
            (*it).second->ReleaseAccess(TaskId, depend);
      }else{
#ifdef LOG4CXX
        LOG4CXX_WARN(loggerOOPDataManager,"Object not found");
#else        
        cerr << "Object not found\n";
#endif
      }
}
int OOPDataManager::SubmitAccessRequest (const OOPObjectId & TaskId,
					 const OOPMDataDepend & depend,
					 const long ProcId)
{
	DataManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	map <OOPObjectId, OOPMetaData * >::iterator i;
	i=fObjects.find(depend.Id());
	if(i!=fObjects.end()){
    if (!depend.Version ().AmICompatible ((*i).second->Version ()))
    {
      DataManLog << "AmICompatible returned false " << endl;
      return 0;
    }
    (*i).second->SubmitAccessRequest (TaskId, depend, GetProcID ());
    DataManLog << "Access request submitted" << endl;
    (*i).second->Print(DataManLog);
	}else{
    if (depend.Id ().GetProcId () == fProcessor) {
      DataManLog << "SubmitAccessRequest for deleted object, returning 0\n";
#ifdef LOG4CXX
      std::string aux = "SubmitAccessRequest for deleted object, returning 0 size of submitted list";
      aux += itoa(fSubmittedObjects.size());
      LOG4CXX_WARN(loggerOOPDataManager,aux.c_str());
#else        
      cout << "SubmitAccessRequest for deleted object, returning 0 size of submitted list " << fSubmittedObjects.size() << "\n";
#endif
			return 0;
    }
		else {
			OOPMetaData *dat =
				new OOPMetaData (0, depend.Id (),
						 depend.Id ().GetProcId ());
			dat->SetTrace (true);	// Erico
			fObjects[depend.Id()]= dat;	// [id] = dat;
			dat->SubmitAccessRequest (TaskId, depend,
						  GetProcID ());
			return 1;
		}
	}
	return 1;
}
OOPDataManager::OOPDataManager (int Procid)
{
	fProcessor = Procid;
	fObjId.SetProcId (Procid);
	fLastCreated = 0;	// NUMOBJECTS * Procid;
//	fMaxId = 1000;	// fLastCreated + NUMOBJECTS;
	pthread_mutex_init(&fDataMutex, NULL);
}
void OOPDataManager::SubmitAllObjects(){
	pthread_mutex_lock(&fDataMutex);
	list<OOPMetaData *>::iterator lit=fSubmittedObjects.begin();
	for(;lit!=fSubmittedObjects.end();lit++){
		fObjects[(*lit)->Id()]=(*lit);
	}
	fSubmittedObjects.clear();
	pthread_mutex_unlock(&fDataMutex);
}
OOPObjectId OOPDataManager::SubmitObject (TPZSaveable * obj, int trace)
{
  OOPDataVersion ver;
  return SubmitObject(obj,trace,ver);
}
OOPObjectId OOPDataManager::SubmitObject (TPZSaveable * obj, int trace, OOPDataVersion & ver)
{
#ifdef DEBUG
  if(!CM->GetProcID())
  {
      std::ostringstream FileName, FileName2,command,subdir1,subdir2,subdir3;
      subdir1 << "dataman" << CM->GetProcID();
      subdir2 << "dataman" << CM->GetProcID() << "/orig";
      subdir3 << "dataman" << CM->GetProcID() << "/copy";
      mkdir(subdir1.str().c_str() , S_IRWXU | S_IXGRP | S_IRGRP| S_IXOTH | S_IROTH );
      mkdir(subdir2.str().c_str(), S_IRWXU | S_IXGRP | S_IRGRP| S_IXOTH | S_IROTH );
      mkdir(subdir3.str().c_str(), S_IRWXU | S_IXGRP | S_IRGRP| S_IXOTH | S_IROTH );
      FileName << subdir2.str() << "/" << obj->ClassId() << ".sav";
      FileName2 << subdir3.str() << "/" << obj->ClassId() << ".sav";
     {
      TPZFileStream PZFS;
      PZFS.OpenWrite(FileName.str());
      obj->Write(PZFS,1);
    }
     {
      TPZFileStream PZFS;
      PZFS.OpenRead(FileName.str());
      TPZSaveable *test =  TPZSaveable::Restore(PZFS,0);
      TPZFileStream PZFS2;
      PZFS2.OpenWrite(FileName2.str());
      test->Write(PZFS2,1);
      delete test;
    }
    command << "diff --brief " << FileName.str() << " " << FileName2.str() << endl;
    FILE *pipe = popen(command.str().c_str(),"r");
#ifdef DEBUGALL
    cout << "Command executed " << command.str() << endl;
#endif
    char *compare = new char[256];
    compare[0] = '\0';
    char **compptr = &compare;
    size_t size = 256;
    getline(compptr,&size,pipe);
//    fscanf(pipe,"%s",compare);
    pclose(pipe);
    if(strlen(compare))
    {
      cout << __PRETTY_FUNCTION__ << compare << endl;
    }
    delete []compare;    
  }
#endif

	OOPObjectId id = DM->GenerateId ();
	OOPMetaData *dat = new OOPMetaData (obj, id, fProcessor, ver);
	
	dat->SetTrace (trace);	// Erico
	pthread_mutex_lock(&fDataMutex);
	fSubmittedObjects.push_back(dat);
	pthread_mutex_unlock(&fDataMutex);
	return id;
}
void OOPDataManager::DeleteObject (OOPObjectId & ObjId)
{
	map<OOPObjectId, OOPMetaData * >::iterator i;
	i = fObjects.find(ObjId);
	if(i!=fObjects.end()){
		delete (*i).second;
		fObjects.erase (i);
	}else{
#ifdef LOG4CXX
      LOG4CXX_ERROR(loggerOOPDataManager,"OOPDataManager::DeleteObject Inconsistent object deletion");
#else        
		// Issue a server warning message !!!
		cerr << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
#endif
// 	}
}
void OOPDataManager::RequestDeleteObject (OOPObjectId & ObjId)
{
	map < OOPObjectId, OOPMetaData * >::iterator i;
    i=fObjects.find(ObjId);
	if (i != fObjects.end ()) {
		(*i).second->RequestDelete ();
	}else{
#ifdef LOG4CXX
      LOG4CXX_ERROR(loggerOOPDataManager,"DeleteObject Inconsistent object deletion File");
#else        
		// Issue a sever warning message !!!
		cerr << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
#endif
	}
}
void OOPDataManager::TransferObject (OOPObjectId & ObjId, int ProcId)
{
	map < OOPObjectId, OOPMetaData * >::iterator i;
	OOPMetaData *dat = 0;
    i=fObjects.find(ObjId);
	if(i != fObjects.end ()) {
		dat = (*i).second;
      	dat->TransferObject (ProcId);
	}else{
            return;
    }
}
void OOPDataManager::GetUpdate (OOPDMOwnerTask * task)
{
	DataManLog << GLogMsgCounter << endl;
	GLogMsgCounter++; 
	DataManLog << "Calling GetUpdate(OOPDMOwnerTask)\n"; 
	OOPMetaData *dat = Data (task->fObjId);
	if (!dat) {
#ifdef LOG4CXX
      LOG4CXX_FATAL(loggerOOPDataManager,"GetUpdate called with invalid ojbid");
#endif        
		DataManLog << "TDataManager:GetUpdate called with invalid ojbid:";
		cout << "TDataManager:GetUpdate called with invalid ojbid:";
		task->fObjId.Print (DataManLog);
		task->fObjId.Print (cout);
		exit (-1);
		return;
	}
	if (task->fType == ENotifyDeleteObject) {
		DataManLog << "TDataManager:GetUpdate calling DeleteObject:";
		task->fObjId.Print(DataManLog);
		dat->DeleteObject ();
	}
	else {
		DataManLog << "TDataManager:GetUpdate Message Handled:";
		task->fObjId.Print(DataManLog);
		dat->HandleMessage (*task);
	}
}
void OOPDataManager::GetUpdate (OOPDMRequestTask * task)
{
	DataManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	DataManLog << "Calling GetUpdate(OOPDMRequestTask):\n";
	OOPObjectId id = task->fDepend.Id ();
	map <OOPObjectId, OOPMetaData * >::iterator i;
      i=fObjects.find(id);
	if (i == fObjects.end ()) {
		if (id.GetProcId () == this->GetProcID ()) {
			DataManLog << "OOPDataManager::GetUpdate send a delete object message to the original processor\n";
		}
		else {
			OOPDMRequestTask *ntask =
				new OOPDMRequestTask (*task);
			DataManLog << "OOPDataManager::GetUpdate Submitting received task\n";
			ntask->SetProcID (id.GetProcId ());
			TM->SubmitDaemon(ntask);
		}
	}
	else {
		DataManLog << "OOPDataManager::GetUpdate fDepend.Id() found in this processor:" << id << endl;
		if(!(*i).second->IamOwner()) {
			OOPDMRequestTask *ntask = new OOPDMRequestTask(*task);
			ntask->SetProcID((*i).second->Proc());
			TM->SubmitDaemon(ntask);
		} else if((*i).second->IamOwner() && task->fProcOrigin == (*i).second->Proc()) {
#ifdef LOG4CXX
      LOG4CXX_WARN(loggerOOPDataManager,"Task request ignored");
#else        
  		cout << "Task request ignored\n";
#endif
		} else {
			(*i).second->SubmitAccessRequest (OOPObjectId(), task->fDepend,
					   task->fProcOrigin);
		}
	}
}
OOPObjectId OOPDataManager::GenerateId ()
{
	fLastCreated++;
//	if (fLastCreated >= fMaxId)
//		exit (-1);	// the program ceases to function
	OOPObjectId *obj = new OOPObjectId (GetProcID (), fLastCreated);
	return *obj;	// fLastCreated;
}
OOPMetaData *OOPDataManager::Data (OOPObjectId ObjId)
{
	map <OOPObjectId,  OOPMetaData * >::iterator i;
      i=fObjects.find(ObjId);
	if(i != fObjects.end ()) {
		return (*i).second;
	}
	return 0;
}
void OOPDataManager::PrintDataQueues(char * msg, ostream & out){
	out << "Printing Data Queues on processor :" << fProcessor << msg << endl;
	map <OOPObjectId, OOPMetaData * >::iterator i;
	OOPAccessInfoList auxlist;
	for(i=fObjects.begin();i!=fObjects.end();i++){
		(*i).second->PrintLog(out);
	}
	
}
OOPDMOwnerTask::OOPDMOwnerTask() :OOPDaemonTask(-1) {
	fObjPtr = 0;
	fState = ENoAccess;
	fProcOrigin = DM->GetProcID ();
//      fObjId = 0;
	fTrace = 0;	// Erico

}
OOPDMOwnerTask::OOPDMOwnerTask (OOPMDMOwnerMessageType t, int proc):OOPDaemonTask
	(proc),fVersion(), fObjId()
	/* , fAccessProcessors(0), fBlockingReadProcesses(0) */
{
	fType = t;
	fObjPtr = 0;
	fState = ENoAccess;
	fProcOrigin = DM->GetProcID ();
//      fObjId = 0;
	fTrace = 0;	// Erico
}
/*
	ENoMessage,
	ECancelReadAccess,
	ECancelReadAccessConfirmation,
	ESuspendAccess,
	ESuspendAccessConfirmation,
	ESuspendSuspendAccess,
	ETransferOwnership,
	EGrantReadAccess,
	EGrantVersionAccess,
	ENotifyDeleteObject,
*/
OOPDMOwnerTask::~OOPDMOwnerTask() {
	switch(this->fType) {
	case ETransferOwnership:
		if(this->fObjPtr) delete fObjPtr;
		break;
	
	default:
		return;
	}
}
//***********************************************************************
OOPDMRequestTask::OOPDMRequestTask (int proc,
				    const OOPMDataDepend &
				    depend):OOPDaemonTask (proc),
fDepend (depend)
{
	fProcOrigin = DM->GetProcID ();
}
OOPDMRequestTask::
OOPDMRequestTask (const OOPDMRequestTask & task):OOPDaemonTask (task),
fProcOrigin (task.fProcOrigin), fDepend (task.fDepend)
{
}
OOPDMRequestTask::OOPDMRequestTask ():OOPDaemonTask (-1)
{
	fProcOrigin = -1;
}
void OOPDMOwnerTask::Read (TPZStream & buf, void * context)
{
	OOPDaemonTask::Read (buf, context);
	char type;
	buf.Read (&type);
	fType = (OOPMDMOwnerMessageType) type;
	int access;
	buf.Read (&access);
	fState = (OOPMDataState) access;
//      buf->UpkLong(&fVersion);
	fVersion.Read (buf);
	//fObjPtr = buf->Restore ();
#ifdef LOG4CXX
  LOG4CXX_DEBUG(loggerOOPDataManager,"Restoring fObjPtr");
#else        
#ifdef DEBUGALL
	cout << "Restoring fObjPtr\n";
#endif
#endif
	fObjPtr = TPZSaveable::Restore (buf,0);//, 0);
	// buf->UpkLong(&fTaskId);
	buf.Read (&fTrace);
	buf.Read (&fProcOrigin);
	// N�o faz sentido !!!
	fObjId.Read(buf);
	DataLog << "Unpacking Owner task for Obj " << fObjId << " message type " <<
		fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion <<
		endl;

}
void OOPDMOwnerTask::Write (TPZStream& buf, int withclassid)
{
	DataLog << "Packing Owner task for Obj " << fObjId << " message type " <<
		fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion <<
		endl;
	OOPDaemonTask::Write (buf, withclassid);
	char type = fType;
	buf.Write (&type);
	int access = fState;
	buf.Write (&access);
	fVersion.Write (buf);	// buf->PkLong(&fVersion);
	if (fObjPtr) {
#ifdef LOG4CXX
  std::strign aux = "writing object of type";
  aux += itoa (fObjPtr->ClassId());
  LOG4CXX_DEBUG(loggerOOPDataManager,aux);
#else        
#ifdef DEBUGALL
    cout << __PRETTY_FUNCTION__ << " writing object of type " << fObjPtr->ClassId() << endl;
#endif
#endif
		fObjPtr->Write (buf,1);
	}
	else {
		int zero = -1;
		buf.Write (&zero);
	}
	buf.Write (&fTrace);
	buf.Write (&fProcOrigin);
	fObjId.Write (buf);	// buf->PkLong(&fObjId);
	
}
void OOPDMOwnerTask::LogMe(ostream & out){
	out << fProc;
	out << "\tId "<< fObjId;
	out << "\tSending ";
	switch (fType)
	{
		case  ENoMessage:
			out << "ENoMessage\t";
			break;
		case  ECancelReadAccess:
			out << "ECancelReadAccess\t";
			break;
		case  ECancelReadAccessConfirmation:
			out << "ECancelReadAccessConfirmation\t";
			break;
		case  ESuspendAccess:
			out << "ESuspendAccess\t";
			break;
		case  ESuspendAccessConfirmation:
			out << "ESuspendAccessConfirmation\t";
			break;
		case  ESuspendSuspendAccess:
			out << "ESuspendSuspendAccess\t";
			break;
		case  ETransferOwnership:
			out << "ETransferOwnership\t";
			break;
		case  EGrantReadAccess:
			out << "EGrantReadAccess\t";
			break;
		case  EGrantVersionAccess:
			out << "EGrantVersionAccess\t";
			break;
		case  ENotifyDeleteObject:
			out << "ENotifyDeleteObject\t";
			break;
		default:
			out << "Uninitialized fType property\t";
			break;
	}
	out << "State ";
	switch (fState )
	{
		case  ENoAccess:
			out << "ENoAccess\t";
			break;
		case  EReadAccess:
			out << "EReadAccess\t";
			break;
		case  EWriteAccess:
			out << "EWriteAccess\t";
			break;
		case  EVersionAccess:
			out << "EVersionAccess\t";
			break;
	}
	
	out << "Version " << fVersion << "\t";
	out << "\tTo Processor " << fProc;

	out.flush();


	
}
void OOPDMOwnerTask::LogMeReceived(ostream & out){
	out << fProc;
	out << "\tId "<< fObjId;
	out << "\tReceiving ";
	switch (fType)
	{
		case  ENoMessage:
			out << "ENoMessage\t";
			break;
		case  ECancelReadAccess:
			out << "ECancelReadAccess\t";
			break;
		case  ECancelReadAccessConfirmation:
			out << "ECancelReadAccessConfirmation\t";
			break;
		case  ESuspendAccess:
			out << "ESuspendAccess\t";
			break;
		case  ESuspendAccessConfirmation:
			out << "ESuspendAccessConfirmation\t";
			break;
		case  ESuspendSuspendAccess:
			out << "ESuspendSuspendAccess\t";
			break;
		case  ETransferOwnership:
			out << "ETransferOwnership\t";
			break;
		case  EGrantReadAccess:
			out << "EGrantReadAccess\t";
			break;
		case  EGrantVersionAccess:
			out << "EGrantVersionAccess\t";
			break;
		case  ENotifyDeleteObject:
			out << "ENotifyDeleteObject\t";
			break;
		default:
			out << "Uninitialized fType property\t";
			break;
	}
	out << "State ";
	switch (fState )
	{
		case  ENoAccess:
			out << "ENoAccess\t";
			break;
		case  EReadAccess:
			out << "EReadAccess\t";
			break;
		case  EWriteAccess:
			out << "EWriteAccess\t";
			break;
		case  EVersionAccess:
			out << "EVersionAccess\t";
			break;
	}
	
	out << "Version " << fVersion << "\t";
	out << "\tFrom Processor " << fProcOrigin;
	out.flush();
}
OOPMReturnType OOPDMOwnerTask::Execute ()
{
	DM->GetUpdate (this);
	return ESuccess;
}
OOPMReturnType OOPDMRequestTask::Execute ()
{
	DM->GetUpdate (this);
	return ESuccess;
}
void OOPDMRequestTask::Read(TPZStream & buf, void * context)
{
	cout.flush();
	OOPDaemonTask::Read(buf, context);
	buf.Read (&fProcOrigin);
	fDepend.Read (buf);

}
TPZSaveable *OOPDMRequestTask::Restore (TPZStream & buf, void * context)
{
	OOPDMRequestTask *t = new OOPDMRequestTask ();
	t->Read (buf);
	return t;
}
void OOPDMRequestTask::Write (TPZStream & buf, int withclassid)
{
	cout.flush();
	OOPDaemonTask::Write (buf, withclassid);
	buf.Write (&fProcOrigin);
	fDepend.Write (buf);

}

void OOPDMRequestTask::LogMe(ostream & out){
	out << fProc << "\t";
	out << "Id " << fDepend.Id();
	out << "\tRequesting ";
	switch (fDepend.State())
	{
		case  ENoAccess:
			out << "ENoAccess To processor " << fProc ;
			break;
		case  EReadAccess:
			out << "EReadAccess To processor " << fProc ;
			break;
		case  EWriteAccess:
			out << "EWriteAccess To processor " << fProc ;
			break;
		case  EVersionAccess:
			out << "EVersionAccess To processor " << fProc ;
			break;
		default:
			out << "Uninitialized fNeed attribute\t";
			break;
	}

	out << "\t\tVersion " << fDepend.Version();
	out << "\tTo processor " << fProc << "\n";
	out.flush();
}
