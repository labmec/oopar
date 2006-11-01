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

#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPDataManager"));
#endif

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
#ifdef LOGPZ    
  stringstream sout;
  sout << "Terminating DM";
  LOGPZ_INFO(logger,sout.str().c_str());
#endif  
  if (LogDM) delete LogDM;
  LogDM = 0;
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
#ifdef LOGPZ    
    stringstream sout;
    sout << "Object not found";
//#ifdef LOGPZ
    LOGPZ_ERROR(logger,sout.str());
#endif
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
#ifdef LOGPZ      
      stringstream sout;
      sout << "AmICompatible returned false ";
      LOGPZ_ERROR(logger,sout.str());
#endif      
      return 0;
    }
#ifdef LOGPZ      
    stringstream sout;
    sout << "Access request submitted" << endl;
#endif    
    (*i).second->SubmitAccessRequest (TaskId, depend, GetProcID ());
#ifdef LOGPZ      
    LOGPZ_DEBUG(logger,sout.str());    
    (*i).second->Print(sout);
#endif
  }else{
    if (depend.Id ().GetProcId () == fProcessor) {
#ifdef LOGPZ      
      stringstream sout;
      sout << "SubmitAccessRequest for deleted object, returning 0 size of submitted list " << fSubmittedObjects.size();
      LOGPZ_WARN(logger,sout);
#endif      
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
#ifdef LOGPZ  
    stringstream sout;
    sout << "Command executed " << command.str();
    LOGPZ_INFO(logger,sout);
#endif    
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
      LOGPZ_WARN(logger,sout);
    }
    delete []compare;    
  }
#endif
  //cout << __PRETTY_FUNCTION__ << " ENTERING\n";
  //cout.flush();
  OOPObjectId id = DM->GenerateId ();
  //cout.flush();
  //cout << __PRETTY_FUNCTION__ << " generate id\n";
	OOPMetaData *dat = new OOPMetaData (obj, id, fProcessor, ver);
 //cout << __PRETTY_FUNCTION__ << " new meta data\n";
 
	dat->SetTrace (trace);	// Erico
 //cout << __PRETTY_FUNCTION__ << " set trace\n";
	pthread_mutex_lock(&fDataMutex);
 //cout << __PRETTY_FUNCTION__ << " mutex_lock\n";
	fSubmittedObjects.push_back(dat);
 //cout << __PRETTY_FUNCTION__ << " fSubmittedObjects.push_back\n";
	pthread_mutex_unlock(&fDataMutex);
 //cout << __PRETTY_FUNCTION__ << " mutex unlock\n";
 //cout.flush();
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
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ ;
    LOGPZ_ERROR(logger,sout.str());
#endif    
  }
}
void OOPDataManager::RequestDeleteObject (OOPObjectId & ObjId)
{
	map < OOPObjectId, OOPMetaData * >::iterator i;
    i=fObjects.find(ObjId);
	if (i != fObjects.end ()) {
		(*i).second->RequestDelete ();
	}else{
#ifdef LOGPZ    
    stringstream sout;
    sout << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ ;
    LOGPZ_ERROR(logger,sout.str());
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
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Calling GetUpdate(OOPDMOwnerTask)"; 
    LOGPZ_INFO(logger,sout.str());
#endif    
  }
  OOPMetaData *dat = Data (task->fObjId);
  if (!dat)
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "TDataManager:GetUpdate called with invalid ojbid:";    
    task->fObjId.Print (sout);
    LOGPZ_FATAL(logger,"GetUpdate called with invalid ojbid");
#endif    
    exit (-1);
    return;
  }
  if (task->fType == ENotifyDeleteObject)
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "TDataManager:GetUpdate calling DeleteObject: obj " << task->fObjId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
    dat->DeleteObject ();
  }
  else
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "TDataManager:GetUpdate Message Handled: obj " << task->fObjId;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
    dat->HandleMessage (*task);
  }
}
void OOPDataManager::GetUpdate (OOPDMRequestTask * task)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Calling GetUpdate(OOPDMRequestTask):";
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }

  OOPObjectId id = task->fDepend.Id ();
  map <OOPObjectId, OOPMetaData * >::iterator i;
  i=fObjects.find(id);
  if (i == fObjects.end ())
  {
    if (id.GetProcId () == this->GetProcID ())
    {
#ifdef LOGPZ      
        stringstream sout;
        sout << "OOPDataManager::GetUpdate send a delete object message to the original processor";
        LOGPZ_DEBUG(logger,sout.str());
#endif        
    }
    else
    {
      OOPDMRequestTask *ntask = new OOPDMRequestTask (*task);
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << "OOPDataManager::GetUpdate Submitting received task";
        LOGPZ_DEBUG(logger,sout.str());
#endif        
      }
      ntask->SetProcID (id.GetProcId ());
      TM->SubmitDaemon(ntask);
    }
  }
  else
  {
    {
#ifdef LOGPZ      
      stringstream sout;
      sout << "OOPDataManager::GetUpdate fDepend.Id() found in this processor:" << id;
      LOGPZ_DEBUG(logger,sout.str());
#endif      
    }
    
    if(!(*i).second->IamOwner() && task->fProcOrigin != (*i).second->Proc())
    {
      // I will reroute the task only if the processor to which the request refers
      // is not the owner of the object
      {
        stringstream sout;
        sout << "OOPDataManager::GetUpdate rerouting request task:";
        task->LogMe(sout);
        LOGPZ_WARN(logger,sout.str());
      }
      OOPDMRequestTask *ntask = new OOPDMRequestTask(*task);
      ntask->SetProcID((*i).second->Proc());
      TM->SubmitDaemon(ntask);

    } else if(task->fProcOrigin == (*i).second->Proc())
    {
      
      {
#ifdef LOGPZ        
        stringstream sout;
        sout << __PRETTY_FUNCTION__ << " Ignoring the request, not rerouting the task to its owning object";
        task->LogMe(sout);
        LOGPZ_WARN(logger,sout.str());
#endif        
      }
    } else {
#ifdef LOGPZ      
      stringstream sout;
      sout << " dependency " << task->fDepend << " processor origin " << task->fProcOrigin;
      LOGPZ_DEBUG(logger,sout.str());
#endif      
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
	OOPObjectId obj(GetProcID (), fLastCreated);
	return obj;	// fLastCreated;
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
  const OOPMDataDepend & depend):OOPDaemonTask (proc), fDepend (depend)
{
  fProcOrigin = DM->GetProcID ();

  {
    std::stringstream sout;
    sout << __PRETTY_FUNCTION__ << " RequestTask constructed with depend " << depend << " target proc " << proc <<
        " origin proc " << fProcOrigin;
    LOG4CXX_DEBUG(logger,sout.str());
  }
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
#ifdef LOGPZ    
  LOGPZ_DEBUG(logger,"Restoring fObjPtr");
#endif  
  
  fObjPtr = TPZSaveable::Restore (buf,0);//, 0);
  // buf->UpkLong(&fTaskId);
  buf.Read (&fTrace);
  buf.Read (&fProcOrigin);
  // N� faz sentido !!!
  fObjId.Read(buf);
#ifdef LOGPZ    
  stringstream sout;
  sout << "Unpacking Owner task for Obj " << fObjId << " message type " 
       <<  fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion;       
  if(fObjPtr)
  {
    sout << "Class id " << fObjPtr->ClassId();
  }
  LOGPZ_DEBUG(logger,sout.str());
#endif  
}
void OOPDMOwnerTask::Write (TPZStream& buf, int withclassid)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Packing Owner task for Obj " << fObjId << " message type " 
       << fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion ;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  OOPDaemonTask::Write (buf, withclassid);
  char type = fType;
  buf.Write (&type);
  int access = fState;
  buf.Write (&access);
  fVersion.Write (buf);	// buf->PkLong(&fVersion);
  if (fObjPtr) {
#ifdef LOGPZ    
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " writing object of type " << fObjPtr->ClassId();
    LOGPZ_DEBUG(logger,sout.str());
#endif    
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
		case  ETransferOwnership:
			out << "ETransferOwnership\t";
			break;
		case  EGrantReadAccess:
			out << "EGrantReadAccess\t";
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
		case  ETransferOwnership:
			out << "ETransferOwnership\t";
			break;
		case  EGrantReadAccess:
			out << "EGrantReadAccess\t";
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
	OOPDaemonTask::Read(buf, context);
	buf.Read (&fProcOrigin);
	fDepend.Read (buf);
        {
          std::stringstream sout;
          sout << __PRETTY_FUNCTION__ << " Reading request task proc origin " << fProcOrigin << " depend " << fDepend << " fProc " << fProc;
          LOG4CXX_DEBUG(logger,sout.str());
        }
}
TPZSaveable *OOPDMRequestTask::Restore (TPZStream & buf, void * context)
{
	OOPDMRequestTask *t = new OOPDMRequestTask ();
	t->Read (buf);
	return t;
}
void OOPDMRequestTask::Write (TPZStream & buf, int withclassid)
{
  std::stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Writing request task proc origin " << fProcOrigin << " depend " << fDepend << " fProc " << fProc;
  LOG4CXX_DEBUG(logger,sout.str());
	OOPDaemonTask::Write (buf, withclassid);
	buf.Write (&fProcOrigin);
	fDepend.Write (buf);

}

void OOPDMRequestTask::LogMe(std::ostream & out){
  out << "OOPDMRequestTask fProcOrigin " << fProcOrigin;
	out << " Depend " << fDepend;
	out << " fProc " << fProc;
}
