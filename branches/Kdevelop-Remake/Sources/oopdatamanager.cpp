// -*- c++ -*-
#include "oopdatamanager.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"
#include "ooppardefs.h"
#include "oopcommmanager.h"

#include "oopdmlock.h"

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
//class   OOPMetaData;
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
static LoggerPtr logger(Logger::getLogger("OOPar.OOPDataManager"));
static LoggerPtr taglogger(Logger::getLogger("OOPar.OOPDataManager.OOPAccessTag"));
static LoggerPtr HandleMsglogger(Logger::getLogger("OOPar.OOPDataManager.DMHandleMessages"));
static LoggerPtr PostMsglogger(Logger::getLogger("OOPar.OOPDataManager.DMPostMessages"));

#endif


OOPDataManager::OOPDataManager(int Procid)
{
#ifdef LOGPZ    
  stringstream sout;
  sout << "Creating DM";
  LOGPZ_INFO(logger,sout.str());
#endif  
  fProcessor = Procid;
  fObjId.SetProcId (Procid);
  fLastCreated = 0;	// NUMOBJECTS * Procid;
}

OOPDataManager::~OOPDataManager ()
{
  fObjects.clear ();
#ifdef LOGPZ    
  stringstream sout;
  sout << "Terminating DM";
  LOGPZ_INFO(logger,sout.str().c_str());
#endif  
}

void OOPDataManager::PostAccessRequest(OOPAccessTag & depend)
{
#ifdef LOGPZ    
  stringstream sout;
  sout << "Posting Access request for Object " << depend.Id() << " from Task " << depend.TaskId() << " To Processor " << depend.Proc();
  sout << " Version " << depend.Version();
  LOGPZ_DEBUG(PostMsglogger,sout.str());
#endif  
  std::pair<int, OOPAccessTag> item(EDMRequest, depend);
  OOPDMLock lock;
  fMessages.push_back(item);
}
void OOPDataManager::PostForeignAccessRequest(OOPAccessTag & depend)
{
#ifdef LOGPZ    
  stringstream sout;
  sout << "Posting Foreign Access request for Object " << depend.Id() << " from Task " << depend.TaskId() << " To Processor " << depend.Proc();
  sout << " Version " << depend.Version();
  LOGPZ_DEBUG(PostMsglogger,sout.str());
#endif  
  std::pair<int, OOPAccessTag> item(EDMForeignRequest, depend);
  OOPDMLock lock;
  fMessages.push_back(item);
  TM->WakeUpCall();
}


void OOPDataManager::PostOwnerMessage(OOPAccessTag & tag)
{
  std::pair<int, OOPAccessTag> item(EDMOwner, tag);
#ifdef LOGPZ    
  stringstream sout;
  sout << "Posting OwnerMessage for Object " << tag.Id() << " from Task " << tag.TaskId();
  sout << " Version " << tag.Version() << " AccessMode:" << tag.AccessMode() << " Pointer " << tag.AutoPointer();
  LOGPZ_DEBUG(PostMsglogger,sout.str());
#endif  
  OOPDMLock lock;
  fMessages.push_back(item);
}

// vamos colocar o objeto numa pilha
// retorna

// outro metodo : o que fazer com esses objetos

// identificar o objectid

/*  map <OOPObjectId, OOPMetaData * >::iterator i;
  i=fObjects.find(depend.Id());
  if(i!=fObjects.end()){}*/
  // achei o objeto
  // submeter o pedido ao objeto, fim de papo
  
  
  // nao achei o objeto metadataCMCM
  // cria o objeto metadataCM1 
  // submeta o tag
  // fim de papo 
//}
void OOPDataManager::PostData(OOPAccessTag & tag)
{
  std::pair<int, OOPAccessTag> item(EDMData, tag);
#ifdef LOGPZ    
  stringstream sout;
  sout << "Posting Data for Object " << tag.Id();
  sout << " with Version " << tag.Version() << " In processor:" << tag.Proc();
  LOGPZ_DEBUG(PostMsglogger,sout.str());
#endif  
  OOPDMLock lock;
  fMessages.push_back(item);
}
//void OOPDataManager::
OOPObjectId OOPDataManager::SubmitObject (TPZSaveable * obj)
{
  OOPObjectId id = DM->GenerateId ();
  TPZAutoPointer<TPZSaveable> ptr(obj);
  OOPAccessTag tag(id,ptr);
  PostData(tag);
#ifdef DEBUGPARANOID
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
#ifdef LOGPZ      
    {
      stringstream sout;
      sout << "Creating metadata with object of classid " << obj->ClassId() << " and object id " << id;
      LOGPZ_DEBUG(logger,sout.str());
    }
#endif
  return id;
}
void OOPDataManager::GetUpdate (OOPDMOwnerTask * task)
{
  {
#ifdef LOGPZ    
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Posting Owner Task"; 
  LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  PostOwnerMessage( task->fTag);
  std::set<OOPAccessTag>::iterator it;
  for(it = task->fTransferRequests.begin(); it!= task->fTransferRequests.end(); it++)
  {
    OOPAccessTag tag(*it);
    this->PostAccessRequest(tag);
  }  
}
void OOPDataManager::GetUpdate (OOPDMRequestTask * task)
{
#ifdef LOGPZ
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Posting AccessRequest Originated in processor " << task->fDepend.Proc(); 
  LOGPZ_DEBUG(logger,sout.str());
#endif    
  PostForeignAccessRequest(task->fDepend);
}
OOPObjectId OOPDataManager::GenerateId ()
{
  int localValue = 0;
  {
    OOPDMLock lock;
    fLastCreated++;
    localValue = fLastCreated;
  }
  OOPObjectId obj(GetProcID (), localValue);
  return obj;
}

void OOPDataManager::ObjectChanged(std::set<OOPObjectId> & set)
{

#ifdef LOGPZ
  stringstream sout;
  sout << "Changed Objects for the following IDs\n"; 
#endif    
  std::list<OOPObjectId>::iterator it;
  for(it = fChangedObjects.begin();it != fChangedObjects.end(); it ++)
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "Id:" << *it; 
#endif    
    ObjectChanged(*it);
  }
#ifdef LOGPZ
  LOGPZ_DEBUG(logger,sout.str());
#endif    
}

void OOPDataManager::ObjectChanged(const OOPObjectId & Id)
{
  OOPDMLock lock;
  fChangedObjects.push_back(Id);
}
void OOPDataManager::ExtractObjectFromTag(OOPAccessTag & tag)
{
#ifdef LOGPZ
  std::stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Extracting object from tag = ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(HandleMsglogger,sout.str());
#endif
  fObjects[tag.Id()].SubmitTag(tag);
}
void OOPDataManager::ExtractOwnerTaskFromTag(OOPAccessTag & tag)
{
#ifdef LOGPZ
  std::stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Extracting OwnerTask from tag = ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(HandleMsglogger,sout.str());
#endif
  fObjects[tag.Id()].HandleOwnerMessage(tag);
  
/*  OOPDMOwnerTask otask(tag);
#warning "HERE IS THE POINT"
  fObjects[tag.Id()].HandleMessage(otask);*/
}
void OOPDataManager::ExtractRequestFromTag(OOPAccessTag & tag)
{
  std::map<OOPObjectId, OOPMetaData>::iterator it;
  it = fObjects.find(tag.Id());
#ifdef LOGPZ    
  stringstream sout;
  sout << "Extracting Request From Tag ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(logger,sout.str());
#endif  

  if(it == fObjects.end())
  {
    int proc = tag.Id().GetProcId();
    OOPMetaData meta(tag.Id(),proc);
    fObjects[tag.Id()] = meta;
  }
  fObjects[tag.Id()].SubmitAccessRequest(tag);

}
void OOPDataManager::ExtractForeignRequestFromTag(OOPAccessTag & tag)
{
  std::map<OOPObjectId, OOPMetaData>::iterator it;
  it = fObjects.find(tag.Id());
#ifdef LOGPZ    
  stringstream sout;
  sout << "Extracting Request From Tag ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(logger,sout.str());
#endif  

  if(it == fObjects.end())
  {
    int proc = tag.Id().GetProcId();
    OOPMetaData meta(tag.Id(),proc);
    fObjects[tag.Id()] = meta;
  }
  fObjects[tag.Id()].SubmitAccessRequest(tag);
}
void OOPDataManager::HandleMessages()
{
  SubmitAllObjects();
}
void OOPDataManager::SubmitAllObjects()
{
  std::list< std::pair<int, OOPAccessTag> > tempList;
  {
    OOPDMLock lock;
    tempList = fMessages;
    fMessages.clear();
  }
  std::list< std::pair<int, OOPAccessTag> >::iterator it;
  it = tempList.begin();
  while(it != tempList.end())
  {
    switch(it->first)
    {
      case EDMData:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract Object From Tag ";
        it->second.ShortPrint(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractObjectFromTag(it->second);
      }
      break;
      case EDMOwner:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract OwnerMessage From Tag ";
        it->second.ShortPrint(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractOwnerTaskFromTag(it->second);
         
      }
      break;
      case EDMRequest:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract Request message From Tag ";
        it->second.ShortPrint(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractRequestFromTag(it->second);
      }
      break;
      case EDMForeignRequest:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract Foreign Request message From Tag ";
        it->second.ShortPrint(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractForeignRequestFromTag(it->second);
      }
      break;
      default:
      {
#ifdef LOGPZ    
        stringstream sout;
        sout << "Message Submitted with wrong message type, expect trouble";
        LOGPZ_DEBUG(logger,sout.str().c_str());
#endif  
      }
    }
    tempList.erase(it);
    it=tempList.begin();
  }
  if (tempList.size())
  {
    //Notify TM that there are new messages for the DM
    TM->WakeUpCall();
  }
}
void OOPDataManager::FlushData()
{
  std::list<OOPObjectId> tmpList;
  {
    OOPDMLock lock;
    tmpList = fChangedObjects;
    fChangedObjects.clear();
  }
  std::list< OOPObjectId >::iterator itlst;
  itlst = tmpList.begin();
  while(itlst != tmpList.end())
  {
    std::map<OOPObjectId, OOPMetaData>::iterator it;
    it = fObjects.find(*itlst);
    if(it!=fObjects.end())
    {
      it->second.VerifyAccessRequests();
    }
    tmpList.erase(itlst);
    itlst = tmpList.begin();
  }
}

void OOPDataManager::SnapShotMe()
{
#warning "Implementation still incomplete"
  map < OOPObjectId, OOPMetaData >::iterator it = fObjects.begin();
  for(;it!= fObjects.end();it++)
  {
    it->second.Print(cout);
  }
}

//////////////////////OOPDMOwnerTask////////////////////////////////////////////

OOPDMOwnerTask::OOPDMOwnerTask() :OOPDaemonTask(-1) {

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
OOPDMOwnerTask::~OOPDMOwnerTask() 
{
  if(fTag.AutoPointer())
  {
    fTag.ClearPointer();
    OOPObjectId id = fTag.Id();
    DM->ObjectChanged(id);
  }
}
//***********************************************************************
#warning "Verify if this is the case for the target processor"
OOPDMRequestTask::OOPDMRequestTask (int processor, const OOPAccessTag & depend)
:OOPDaemonTask (processor), fDepend(depend)
{
	
}
OOPDMRequestTask::
OOPDMRequestTask (const OOPDMRequestTask & task):OOPDaemonTask (task), fDepend (task.fDepend)
{
}
OOPDMRequestTask::OOPDMRequestTask ():OOPDaemonTask (-1)
{
}
void OOPDMOwnerTask::Read (TPZStream & buf, void * context)
{
  OOPDaemonTask::Read (buf, context);
  fTag.Read( buf, context);
  int size;
  buf.Read(&size,1);
  int i;
  for(i=0; i<size; i++)
  {
    OOPAccessTag tag;
    tag.Read(buf,0);
    fTransferRequests.insert(tag);
  }
}
void OOPDMOwnerTask::Write (TPZStream& buf, int withclassid)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Packing Owner task for Obj " << fTag.Id() << " message type " 
	 << fTag.AccessMode() << " with objptr " << (fTag.AutoPointer() != 0) << " version " << fTag.Version()
	 << " To Proc " << fTag.Proc();
    if(fTag.AutoPointer()) sout << " class id " << fTag.AutoPointer()->ClassId();
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  OOPDaemonTask::Write (buf, withclassid);
  fTag.Write( buf, 0);
  int size = fTransferRequests.size();
  buf.Write(&size,1);
  std::set<OOPAccessTag>::iterator it;
  for(it=fTransferRequests.begin(); it!=fTransferRequests.end(); it++)
  {
    OOPAccessTag tag(*it);
    tag.Write(buf,0);
  }
}
OOPMReturnType OOPDMOwnerTask::Execute ()
{
  DM->GetUpdate (this);
#ifdef LOGPZ    
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Called ";
  LOGPZ_DEBUG(logger,sout.str());
#endif    
  TM->WakeUpCall();
  return ESuccess;
}
OOPMReturnType OOPDMRequestTask::Execute ()
{
  DM->GetUpdate (this);
#ifdef LOGPZ    
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Called ";
  LOGPZ_DEBUG(logger,sout.str());
#endif    
  //TM->WakeUpCall();
  return ESuccess;
}
void OOPDMRequestTask::Read(TPZStream & buf, void * context)
{
  OOPDaemonTask::Read(buf, context);
  fDepend.Read( buf, context);
#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Reading request task proc origin " << fDepend.Proc() << " fProc " << fProc;
    LOG4CXX_DEBUG(logger,sout.str());
  }
#endif
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
#ifdef LOGPZ
 sout << __PRETTY_FUNCTION__ << " Writing request task proc origin " << fDepend.Proc() << " fProc " << fProc;
  LOG4CXX_DEBUG(logger,sout.str());
#endif
  OOPDaemonTask::Write (buf, withclassid);
  fDepend.Write (buf, withclassid); 

}

void OOPDMRequestTask::LogMe(std::ostream & out){
  out << " Depend " << fDepend;
  out << " fProc " << fProc;
}

