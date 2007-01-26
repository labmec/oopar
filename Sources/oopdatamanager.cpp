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

OOPDataManager::OOPDataManager(int Procid)
{
  fProcessor = Procid;
  fObjId.SetProcId (Procid);
  fLastCreated = 0;	// NUMOBJECTS * Procid;
  pthread_mutex_init(&fDataMutex, NULL);
}

OOPDataManager::~OOPDataManager ()
{
  map< OOPObjectId,  OOPMetaData * >::iterator i=fObjects.begin ();
  for(;i!=fObjects.end();i++){
#ifdef LOGPZ    
  stringstream sout;
  sout << "Deleting object " << i->first << " meta data ";
  i->second->Print(sout);
  LOGPZ_DEBUG(logger,sout.str().c_str());
#endif  
//    i->second->ClearAllVersions();
    delete (i->second);
  }
  fObjects.clear ();
#ifdef LOGPZ    
  stringstream sout;
  sout << "Terminating DM";
  LOGPZ_INFO(logger,sout.str().c_str());
#endif  
}

void OOPDataManager::SubmitAccessRequest(OOPAccessTag & depend)
{
#warning "Still needs implementation"

// vamos colocar o objeto numa pilhaCM
// retornaCM

// outro metodo : o que fazer com esses objetosCM

// identificar o objectid

  map <OOPObjectId, OOPMetaData * >::iterator i;
  i=fObjects.find(depend.Id());
  if(i!=fObjects.end()){}
  // achei o objeto
  // submeter o pedido ao objeto, fim de papo
  
  
  // nao achei o objeto metadataCMCM
  // cria o objeto metadataCM1 
  // submeta o tag
  // fim de papo 
}
void OOPDataManager::SubmitData(OOPAccessTag & tag)
{
#warning "Still needs implementation"
  //fChangedObjects

}
//void OOPDataManager::
OOPObjectId OOPDataManager::SubmitObject (TPZSaveable * obj)
{
  TPZAutoPointer<TPZSaveable> ptr(obj);
  OOPObjectId id = DM->GenerateId ();

  OOPAccessTag tag(id,ptr);
  SubmitData(tag);
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
  SubmitOwnerMessage( task->fTag);
}
void OOPDataManager::GetUpdate (OOPDMRequestTask * task)
{
    SubmitAccessRequest (task->fDepend);
}
OOPObjectId OOPDataManager::GenerateId ()
{
#warning "colocar mutex"
  fLastCreated++;
  OOPObjectId obj(GetProcID (), fLastCreated);
  return obj;
}

void OOPDataManager::ObjectChanged(std::set<OOPObjectId> & set)
{
#warning "Not implemented"
}

void OOPDataManager::ObjectChanged(OOPObjectId & Id)
{
#warning "Still needs implementation"
  //fChangedObjects.push(Id);
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
  if(fTag.GetPointer())
  {
   fTag.ClearPointer();
   OOPObjectId id = fTag.Id();
    DM->ObjectChanged(id);
  }
}
//***********************************************************************
OOPDMRequestTask::OOPDMRequestTask (int proc,
				    const OOPAccessTag & depend):OOPDaemonTask (proc), fDepend(depend)
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
#warning "Implement me"
  OOPDaemonTask::Read (buf, context);
  char type;
}
void OOPDMOwnerTask::Write (TPZStream& buf, int withclassid)
{
#warning "Implement me"
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Packing Owner task for Obj " << fObjId << " message type " 
	 << fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion
	 << " To Proc " << this->fProc;
    if(fObjPtr) sout << " class id " << fObjPtr->ClassId();
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
  OOPDaemonTask::Write (buf, withclassid);
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
#warning "Implement me"
#ifdef LOGPZ
  {
    std::stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Reading request task proc origin " << fProcOrigin << " fProc " << fProc;
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
 sout << __PRETTY_FUNCTION__ << " Writing request task proc origin " << fProcOrigin << " fProc " << fProc;
  LOG4CXX_DEBUG(logger,sout.str());
#endif
  OOPDaemonTask::Write (buf, withclassid);
  fDepend.Write (buf);

}

void OOPDMRequestTask::LogMe(std::ostream & out){
  out << " Depend " << fDepend;
  out << " fProc " << fProc;
}
