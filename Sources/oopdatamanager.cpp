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
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPDataManager"));
#endif


OOPDataManager::OOPDataManager(int Procid)
{
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
  sout << "Posting Access request for Object " << depend.Id() << " from Task " << depend.TaskId();
  sout << " Version " << depend.Version();
  LOGPZ_INFO(logger,sout.str().c_str());
  cout << sout.str().c_str() << endl;
#endif  
  std::pair<int, OOPAccessTag> item(EDMRequest, depend);
  OOPDMLock lock;
  fMessages.push_back(item);
}


void OOPDataManager::PostOwnerMessage(OOPAccessTag & tag)
{
  std::pair<int, OOPAccessTag> item(EDMOwner, tag);
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
  OOPDMLock lock;
  fMessages.push_back(item);
}
//void OOPDataManager::
OOPObjectId OOPDataManager::SubmitObject (TPZSaveable * obj)
{
  TPZAutoPointer<TPZSaveable> ptr(obj);
  OOPObjectId id = DM->GenerateId ();

  OOPAccessTag tag(id,ptr);
  PostData(tag);
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
  //cout.flush();
  //cout << __PRETTY_FUNCTION__ << " generate id\n";
#ifdef LOGPZ      
    {
      stringstream sout;
      sout << "creating metadata with object of classid  " << obj->ClassId() << " and object id " << id;
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
    sout << "Calling GetUpdate(OOPDMOwnerTask)"; 
    LOGPZ_INFO(logger,sout.str());
#endif    
  }
  PostOwnerMessage( task->fTag);
}
void OOPDataManager::GetUpdate (OOPDMRequestTask * task)
{
    PostAccessRequest (task->fDepend);
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

  std::list<OOPObjectId>::iterator it;
  for(it = fChangedObjects.begin();it != fChangedObjects.end(); it ++)
  {
    ObjectChanged(*it);
  }
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
  sout << __PRETTY_FUNCTION__ << " tag = ";
  tag.Print(sout);
  LOGPZ_DEBUG(logger,sout.str());
#endif
  fObjects[tag.Id()].SubmitTag(tag);
  cout << endl << " Called " << __PRETTY_FUNCTION__ <<  __LINE__ << endl;
  cout << "fObjects size " << fObjects.size() << endl;
}
void OOPDataManager::ExtractOwnerTaskFromTag(OOPAccessTag & tag)
{
#warning "Take the intended action for that OwnerTask, according to the OOPMetaData"
  OOPDMOwnerTask otask(tag);
  fObjects[tag.Id()].HandleMessage(otask);
}
void OOPDataManager::ExtractRequestFromTag(OOPAccessTag & tag)
{
  std::map<OOPObjectId, OOPMetaData>::iterator it;
  it = fObjects.find(tag.Id());
#ifdef LOGPZ    
  stringstream sout;
  sout << "Extracting Request From Tag to Object Id " << tag.Id() <<
    " From TaskId " << tag.TaskId() << " with Version " << tag.Version() <<
    " and AccessMode " << tag.AccessMode();
  LOGPZ_DEBUG(logger,sout.str().c_str());
  cout << sout.str() << endl;

#endif  
  
  if(it != fObjects.end())
  {
    it->second.SubmitAccessRequest(tag);
  }else
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Request submitted to non existent object Id " << tag.Id();
    LOGPZ_DEBUG(logger,sout.str().c_str());
    cout << sout.str() << endl;
    cout << "Available objects " << endl;
    cout << fObjects.size() << endl;
    for(it = fObjects.begin();it!=fObjects.end();it++)
    {
      cout << it->second.Id() << endl;
    }
#endif
  }
}
void OOPDataManager::SubmitAllObjects()
{
  std::list< std::pair<int, OOPAccessTag> > tempList;
  {
    OOPDMLock lock;
#ifdef LOG4CXX
    if(fMessages.size())
    {
      std::stringstream sout;
      sout << "Copying the following objects to a temporary list\n";
      std::list< std::pair<int, OOPAccessTag> >::iterator it;
      for(it=fMessages.begin(); it!= fMessages.end(); it++)
      {
        sout << "Message type " << it->first << std::endl;
        sout << "AccessTag ";
        it->second.Print(sout);
        LOGPZ_DEBUG(logger,sout.str());
      }
    }
#endif
    tempList = fMessages;
    fMessages.clear();
  }
  std::list< std::pair<int, OOPAccessTag> >::iterator it;
  it = fMessages.begin();
  while(it != fMessages.end())
  {
    switch(it->first)
    {
      case EDMData:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract Object From Tag ";
        it->second.Print(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractObjectFromTag(it->second);
        fMessages.erase(it);
        it = fMessages.begin();
      }
      break;
      case EDMOwner:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract OwnerMessage From Tag ";
        it->second.Print(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractOwnerTaskFromTag(it->second); 
        fMessages.erase(it);
        it = fMessages.begin();
      }
      break;
      case EDMRequest:
      {
#ifdef LOG4CXX
        std::stringstream sout;
        sout << "Extract Request message From Tag ";
        it->second.Print(sout);
        LOGPZ_DEBUG(logger,sout.str());
#endif
        ExtractRequestFromTag(it->second);
        fMessages.erase(it);
        it = fMessages.begin();
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
  }
}
void OOPDataManager::VerifyAccessRequests()
{
  std::map<OOPObjectId, OOPMetaData>::iterator it;
  for(it = fObjects.begin(); it!=fObjects.end();it++)
  {
    it->second.VerifyAccessRequests();
  }
}

void OOPDataManager::SnapShotMe()
{
#warning "Still requires implementation"
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
OOPDMRequestTask::OOPDMRequestTask (const OOPAccessTag & depend)
:OOPDaemonTask (depend.Proc()), fDepend(depend)
{
  //fProcOrigin = DM->GetProcId();

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
  fTag.Write( buf, withclassid);
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

