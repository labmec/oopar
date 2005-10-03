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

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPDataManager"));

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
  stringstream sout;
  sout << "Terminating DM";
  LOG4CXX_INFO(logger,sout.str());
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
    stringstream sout;
    sout << "Object not found";
//#ifdef LOG4CXX
    LOG4CXX_ERROR(logger,sout.str());
//#else        
//    cerr << sout;
//#endif
  }
}
int OOPDataManager::SubmitAccessRequest (const OOPObjectId & TaskId,
					 const OOPMDataDepend & depend,
					 const long ProcId)
{
  map <OOPObjectId, OOPMetaData * >::iterator i;
  i=fObjects.find(depend.Id());
  if(i!=fObjects.end()){
    if (!depend.Version ().AmICompatible ((*i).second->Version ()))
    {
      stringstream sout;
      sout << "AmICompatible returned false ";
      LOG4CXX_ERROR(logger,sout.str());
      return 0;
    }
    stringstream sout;
    sout << "Access request submitted" << endl;
    (*i).second->SubmitAccessRequest (TaskId, depend, GetProcID ());
    LOG4CXX_DEBUG(logger,sout.str());
    (*i).second->Print(sout);
  }else{
    if (depend.Id ().GetProcId () == fProcessor) {
      stringstream sout;
      sout << "SubmitAccessRequest for deleted object, returning 0 size of submitted list " << fSubmittedObjects.size();
      LOG4CXX_WARN(logger,sout);
      return 0;
    }
    else {
      OOPMetaData *dat =
      new OOPMetaData (0, depend.Id (),depend.Id ().GetProcId ());
      dat->SetTrace (true);	// Erico
      fObjects[depend.Id()]= dat;	// [id] = dat;
      dat->SubmitAccessRequest (TaskId, depend,GetProcID ());
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
  char filename[255];
  sprintf(filename,"datalogger%d", CM->GetProcID());
  LogDM = new OOPDataLogger(filename);

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
    stringstream sout;
    sout << "Command executed " << command.str();
    LOG4CXX_INFO(logger,sout);
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
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << compare;
      LOG4CXX_WARN(logger,sout);
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
    stringstream sout;
    sout << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ ;
    LOG4CXX_ERROR(logger,sout.str());
  }
}
void OOPDataManager::RequestDeleteObject (OOPObjectId & ObjId)
{
	map < OOPObjectId, OOPMetaData * >::iterator i;
    i=fObjects.find(ObjId);
	if (i != fObjects.end ()) {
		(*i).second->RequestDelete ();
	}else{
    stringstream sout;
    sout << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ ;
    LOG4CXX_ERROR(logger,sout.str());
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
  {
    stringstream sout;
    sout << "Calling GetUpdate(OOPDMOwnerTask)"; 
    LOG4CXX_INFO(logger,sout.str());
  }
  OOPMetaData *dat = Data (task->fObjId);
  if (!dat)
  {
    stringstream sout;
    sout << "TDataManager:GetUpdate called with invalid ojbid:";
    task->fObjId.Print (sout);
    LOG4CXX_FATAL(logger,"GetUpdate called with invalid ojbid");
    exit (-1);
    return;
  }
  if (task->fType == ENotifyDeleteObject)
  {
    stringstream sout;
    sout << "TDataManager:GetUpdate calling DeleteObject: obj " << task->fObjId;
    LOG4CXX_DEBUG(logger,sout.str());
    dat->DeleteObject ();
  }
  else
  {
    stringstream sout;
    sout << "TDataManager:GetUpdate Message Handled: obj " << task->fObjId;
    LOG4CXX_DEBUG(logger,sout.str());
    dat->HandleMessage (*task);
  }
}
void OOPDataManager::GetUpdate (OOPDMRequestTask * task)
{
  {
    stringstream sout;
    sout << "Calling GetUpdate(OOPDMRequestTask):";
    LOG4CXX_DEBUG(logger,sout.str());
  }

  OOPObjectId id = task->fDepend.Id ();
  map <OOPObjectId, OOPMetaData * >::iterator i;
  i=fObjects.find(id);
  if (i == fObjects.end ())
  {
    if (id.GetProcId () == this->GetProcID ())
    {
        stringstream sout;
        sout << "OOPDataManager::GetUpdate send a delete object message to the original processor";
        LOG4CXX_DEBUG(logger,sout.str());
    }
    else
    {
      OOPDMRequestTask *ntask = new OOPDMRequestTask (*task);
      {
        stringstream sout;
        sout << "OOPDataManager::GetUpdate Submitting received task";
        LOG4CXX_DEBUG(logger,sout.str());
      }
      ntask->SetProcID (id.GetProcId ());
      TM->SubmitDaemon(ntask);
    }
  }
  else
  {
    {
      stringstream sout;
      sout << "OOPDataManager::GetUpdate fDepend.Id() found in this processor:" << id;
      LOG4CXX_DEBUG(logger,sout.str());
    }
    if(!(*i).second->IamOwner())
    {
      OOPDMRequestTask *ntask = new OOPDMRequestTask(*task);
      ntask->SetProcID((*i).second->Proc());
      TM->SubmitDaemon(ntask);

    } else if((*i).second->IamOwner() && task->fProcOrigin == (*i).second->Proc())
    {
      {
        stringstream sout;
        sout << __PRETTY_FUNCTION__ << " Object Id " << id << "\nMeta data ";
        (*i).second->PrintLog(sout);
        sout << "Request task is ";
        task->LogMe(sout);
        LOG4CXX_WARN(logger,sout.str());
      }
    } else {
      stringstream sout;
      sout << " dependency " << task->fDepend << " processor origin " << task->fProcOrigin;
      LOG4CXX_DEBUG(logger,sout.str());      
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
void OOPDataManager::PrintDataQueues(char * msg, std::ostream & out){
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
  LOG4CXX_DEBUG(logger,"Restoring fObjPtr");
  
  fObjPtr = TPZSaveable::Restore (buf,0);//, 0);
  // buf->UpkLong(&fTaskId);
  buf.Read (&fTrace);
  buf.Read (&fProcOrigin);
  // N� faz sentido !!!
  fObjId.Read(buf);
  stringstream sout;
  sout << "Unpacking Owner task for Obj " << fObjId << " message type " 
       <<  fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion;
  if(fObjPtr)
  {
    sout << "Class id " << fObjPtr->ClassId();
  }
  LOG4CXX_DEBUG(logger,sout.str());
}
void OOPDMOwnerTask::Write (TPZStream& buf, int withclassid)
{
  {
    stringstream sout;
    sout << "Packing Owner task for Obj " << fObjId << " message type " 
       << fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion ;
    LOG4CXX_DEBUG(logger,sout.str());
  }
  OOPDaemonTask::Write (buf, withclassid);
  char type = fType;
  buf.Write (&type);
  int access = fState;
  buf.Write (&access);
  fVersion.Write (buf);	// buf->PkLong(&fVersion);
  if (fObjPtr) {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " writing object of type " << fObjPtr->ClassId();
    LOG4CXX_DEBUG(logger,sout.str());
    fObjPtr->Write (buf,1);
  }
  else
  {
    int zero = -1;
    buf.Write (&zero);
  }
  buf.Write (&fTrace);
  buf.Write (&fProcOrigin);
  fObjId.Write (buf);	// buf->PkLong(&fObjId);
	
}
void OOPDMOwnerTask::LogMe(std::ostream & out){
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
void OOPDMOwnerTask::LogMeReceived(std::ostream & out){
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

void OOPDMRequestTask::LogMe(std::ostream & out){
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
