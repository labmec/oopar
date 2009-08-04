// -*- c++ -*-
#include "oopdatamanager.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"
#include "ooppardefs.h"
#include "oopcommmanager.h"

//#include "oopdmlock.h"
#include "ooplock.h"

#include <map>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>
class   OOPStorageBuffer;
class   OOPStorageBuffer;
class   OOPDataVersion;
class   OOPDMRequestTask;
class   OOPDMOwnerTask;
class   OOPSaveable;
using namespace std;
class   OOPObjectId;

#include "oopdelobjecttask.h"
#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
extern OOPGenericLockService gMutex;
#endif


#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPar.OOPDataManager"));
/*static LoggerPtr taglogger(Logger::getLogger("OOPar.OOPDataManager.OOPAccessTag"));*/
static LoggerPtr HandleMsglogger(Logger::getLogger("OOPar.OOPDataManager.DMHandleMessages"));
static LoggerPtr AccessLogger(Logger::getLogger("OOPar.OOPDataManager.OOPAccessTag"));
static LoggerPtr ServiceLogger(Logger::getLogger("OOPar.OOPDataManager.ServiceLogger"));
static LoggerPtr DaemonLogger(Logger::getLogger("OOPar.OOPTaskManager.DaemonTask"));
static LoggerPtr MetaLogger(Logger::getLogger("OOPar.OOPDataManager.MetaData"));
#endif


OOPDataManager::OOPDataManager(int Procid, TPZAutoPointer<OOPTaskManager> TM) : fDM(this)
{
#ifdef LOG4CXX
  stringstream sout;
  sout << "Creating DM";
  LOGPZ_INFO(logger,sout.str().c_str());
#endif
  fProcessor = Procid;
  fTM = TM;
  fObjId.SetProcId (Procid);
  fLastCreated = 0;	// NUMOBJECTS * Procid;
  fKeepGoing = true;
  //sem_init(&fServiceSemaphore, 0, 0);
	fServiceSemaphore = new boost::interprocess::interprocess_semaphore(0);
  fServiceThread = 0;
	pthread_mutex_init (&fMutex, 0);

}

OOPDataManager::~OOPDataManager ()
{
#ifdef LOG4CXX
	LOGPZ_DEBUG(logger,"before deleting the metadata objects");
#endif
  fObjects.clear ();
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << GetProcID() << " Destructor Terminating DM Service Thread";
    LOGPZ_INFO(logger,sout.str().c_str());
  }
#endif
  WakeUpCall();
  if(fServiceThread != NULL)
  {
    pthread_join(fServiceThread, NULL);
  }
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << "Terminating DM ";
    LOGPZ_INFO(logger,sout.str().c_str());
  }
#endif
	delete fServiceSemaphore;
	pthread_mutex_destroy (&fMutex);
}

void OOPDataManager::PostAccessRequest(OOPAccessTag & depend)
{
#ifdef LOG4CXX
  stringstream sout;
  sout << "Posting Access request for Object Id:" << depend.Id() << " from Task T:" << depend.TaskId() << " To Processor " << depend.Proc() << " with Tag ";
  depend.ShortPrint( sout);
  LOGPZ_DEBUG(AccessLogger,sout.str().c_str());
#endif
  std::pair<int, OOPAccessTag> item(EDMRequest, depend);
  {
    OOPLock<OOPDataManager> lock(this);
    fMessages.push_back(item);
  }
  WakeUpCall();
}
void OOPDataManager::PostForeignAccessRequest(OOPAccessTag & depend)
{
#ifdef LOG4CXX
  stringstream sout;
  sout << "Posting Foreign Access request for Object Id:" << depend.Id() << " from Task T:" << depend.TaskId() << " To Processor " << depend.Proc() << " with Tag ";
  depend.ShortPrint(sout);
  LOGPZ_DEBUG(AccessLogger,sout.str().c_str());
#endif
  std::pair<int, OOPAccessTag> item(EDMForeignRequest, depend);
  {
    OOPLock<OOPDataManager> lock(this);
    fMessages.push_back(item);
  }
  WakeUpCall();
}


void OOPDataManager::PostOwnerMessage(OOPAccessTag & tag)
{
  std::pair<int, OOPAccessTag> item(EDMOwner, tag);
#ifdef LOG4CXX
  stringstream sout;
  sout << "Posting OwnerMessage for Object Id:" << tag.Id() << " from Task T:" << tag.TaskId()
    << " according to Tag ";
  tag.ShortPrint( sout);
  LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
  {
    OOPLock<OOPDataManager> lock(this);
    fMessages.push_back(item);
  }
  WakeUpCall();
}
void OOPDataManager::RequestDelete(OOPObjectId & Id)
{
#ifdef LOG4CXX
  stringstream sout;
  sout << "Submitting Delete Request for Object Id:" << Id;
  LOGPZ_DEBUG(MetaLogger,sout.str().c_str());
#endif

  OOPDataVersion ver;
  ver.SetLevelVersion( 0,-1);
  OOPAccessTag tag(Id, EDelete, ver, -1);
  PostAccessRequest( tag);
}
void OOPDataManager::PostRequestDelete(OOPObjectId & Id)
{
  #ifdef LOG4CXX
  {
    stringstream sout;
    sout << "Submitting OOPDelObjectTask for " << Id;
    LOGPZ_DEBUG(MetaLogger,sout.str());
  }
  #endif
  OOPDelObjectTask * delTask = new OOPDelObjectTask(Id, TM()->DM()->GetProcID());
  TM()->Submit(delTask);

}
OOPMetaData OOPDataManager::Data (OOPObjectId ObjId)
{
  map<OOPObjectId, OOPMetaData>::iterator it = fObjects.end();
  it = fObjects.find(ObjId);
  if (it!=fObjects.end())
  {
    return it->second;
  }
  return OOPMetaData();
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
  {
    std::pair<int, OOPAccessTag> item(EDMData, tag);
    tag.ClearPointer();
#ifdef LOG4CXX
    stringstream sout;
    sout << "Posting Data for Object Id:" << tag.Id();
    sout << " with Version " << tag.Version() << " In processor:" << tag.Proc() << " with Counter " << item.second.Count()
    << " item " ;
    item.second.ShortPrint(sout);
    LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
    {
      OOPLock<OOPDataManager> lock(this);
      fMessages.push_back(item);
    }
  }
  WakeUpCall();
}
//void OOPDataManager::
OOPObjectId OOPDataManager::SubmitObject (TPZSaveable * obj)
{
  OOPObjectId id = GenerateId ();
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
#ifdef LOG4CXX
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
#ifdef LOG4CXX
    {
      stringstream sout;
      sout << "Creating metadata with object of classid " << obj->ClassId() << " and object id " << id;
      LOGPZ_DEBUG(logger,sout.str().c_str());
    }
#endif
  return id;
}
void OOPDataManager::GetUpdate (OOPDMOwnerTask * task)
{
  {
#ifdef LOG4CXX
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Posting Owner Task";
  LOGPZ_DEBUG(logger,sout.str().c_str());
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
#ifdef LOG4CXX
  stringstream sout;
  sout << __PRETTY_FUNCTION__ << " Posting AccessRequest Originated in processor " << task->fDepend.Proc();
  LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
  PostForeignAccessRequest(task->fDepend);
}
OOPObjectId OOPDataManager::GenerateId ()
{
  int localValue = 0;
  {
    OOPLock<OOPDataManager> lock(this);
    fLastCreated++;
    localValue = fLastCreated;
  }
  OOPObjectId obj(GetProcID (), localValue);
  return obj;
}

void OOPDataManager::ObjectChanged(std::set<OOPObjectId> & set)
{

#ifdef LOG4CXX
  stringstream sout;
  sout << "Changed Objects for the following IDs\n";
#endif
  std::set<OOPObjectId>::iterator it;
  for(it = fChangedObjects.begin();it != fChangedObjects.end(); it ++)
  {
#ifdef LOG4CXX
    stringstream sout;
    sout << "Id:" << *it;
#endif
    ObjectChanged(*it);
  }
#ifdef LOG4CXX
  LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
}

void OOPDataManager::ObjectChanged(const OOPObjectId & Id)
{
  {
    OOPLock<OOPDataManager> lock(this);
    fChangedObjects.insert(Id);
  }
  WakeUpCall();
}
void OOPDataManager::ExtractObjectFromTag(OOPAccessTag & tag)
{
#ifdef LOG4CXX
  std::stringstream sout;
  sout << "Submitting Object Id:" << tag.Id() << " with Tag ";
  tag.ShortPrint(sout);
  LOGPZ_INFO(MetaLogger,sout.str().c_str());
#endif
  fObjects[tag.Id()].SubmitTag(tag,fTM->DM());
}
void OOPDataManager::ExtractOwnerTaskFromTag(OOPAccessTag & tag)
{
#ifdef LOG4CXX
  std::stringstream sout;
  sout << "Extracting OwnerTask from tag = ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(HandleMsglogger,sout.str().c_str());
#endif
  fObjects[tag.Id()].HandleOwnerMessage(tag,fTM->DM());
}
void OOPDataManager::ExtractRequestFromTag(OOPAccessTag & tag)
{
  std::map<OOPObjectId, OOPMetaData>::iterator it;
  it = fObjects.find(tag.Id());
#ifdef LOG4CXX
  stringstream sout;
  sout << "Extracting Request From Tag ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(logger,sout.str().c_str());
#endif

  if(it == fObjects.end())
  {
    int proc = tag.Id().GetProcId();
    OOPMetaData meta(tag.Id(),proc);
    fObjects[tag.Id()] = meta;
  }
  fObjects[tag.Id()].SubmitAccessRequest(tag,fTM->DM());

}
void OOPDataManager::ExtractForeignRequestFromTag(OOPAccessTag & tag)
{
  std::map<OOPObjectId, OOPMetaData>::iterator it;
  it = fObjects.find(tag.Id());
#ifdef LOG4CXX
  stringstream sout;
  sout << "Extracting Request From Tag ";
  tag.ShortPrint(sout);
  LOGPZ_DEBUG(logger,sout.str().c_str());
#endif

  if(it == fObjects.end())
  {
    int proc = tag.Id().GetProcId();
    OOPMetaData meta(tag.Id(),proc);
    fObjects[tag.Id()] = meta;
  }
  fObjects[tag.Id()].SubmitAccessRequest(tag,fTM->DM());
}
void OOPDataManager::HandleMessages()
{
  SubmitAllObjects();
}
void OOPDataManager::SubmitAllObjects()
{
  std::list< std::pair<int, OOPAccessTag> > tempList;
  {
    OOPLock<OOPDataManager> lock(this);
    tempList = fMessages;
    fMessages.clear();
  }
#ifdef LOG4CXX
  {
	  std::stringstream sout;
	  sout << __PRETTY_FUNCTION__ << " called with " << tempList.size() << " messages";
	  LOGPZ_DEBUG(logger,sout.str())
  }
#endif
  int tempSize = 0;
  tempSize = tempList.size();
  std::list< std::pair<int, OOPAccessTag> >::iterator it;
  it = tempList.begin();
  while(it != tempList.end())
  {
    switch(it->first)
    {
      case EDMData:
      {
        ExtractObjectFromTag(it->second);
      }
      break;
      case EDMOwner:
      {
        ExtractOwnerTaskFromTag(it->second);
      }
      break;
      case EDMRequest:
      {
        ExtractRequestFromTag(it->second);
      }
      break;
      case EDMForeignRequest:
      {
        ExtractForeignRequestFromTag(it->second);
      }
      break;
      default:
      {
#ifdef LOG4CXX
        stringstream sout;
        sout << "Message Submitted with wrong message type, expect trouble";
        LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
      }
    }
    tempList.erase(it);
    it=tempList.begin();
  }
  if(tempSize)
  {
    WakeUpCall();
  }
}
void OOPDataManager::Wait()
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Joining DM ServiceThread !";
		LOGPZ_DEBUG(logger,sout.str().c_str());
	}
#endif
	if(pthread_join(fServiceThread, NULL)!=0)
	{
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << "pthread_join failed on " << __PRETTY_FUNCTION__ << "\nBailing out";
			LOGPZ_ERROR(logger,sout.str().c_str());
		}
#endif
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "DM ServiceThread Joined!";
		LOGPZ_DEBUG(logger,sout.str().c_str());
	}
#endif
}
int OOPDataManager::StartService()
{
  int res = -1;
  res = pthread_create(&fServiceThread, NULL, ServiceThread, this);
  if(res)
  {
#ifdef LOG4CXX
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Fail to create DM Service THREAD";
    LOGPZ_DEBUG(logger,sout.str().c_str());
    cout << sout.str().c_str() << endl;
#endif
  }
  return res;
}

void * OOPDataManager::ServiceThread(void * data)
{
#ifdef LOG4CXX
	{
		stringstream sout;
		sout << "Starting service thread for DM";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	OOPDataManager * lDMStar = static_cast<OOPDataManager *> (data);
	TPZAutoPointer<OOPTaskManager> TM = lDMStar->TM();
	TPZAutoPointer<OOPDataManager> lDM = TM->DM();
	//  lDM->fKeepGoing = true;
	while (lDM->fKeepGoing)
	{
		lDM->WaitWakeUpCall();
#ifdef STEP_MUTEX
		OOPLock<OOPGenericLockService> lock(&gMutex);
#endif
		{
			stringstream sout;
			sout << lDM->GetProcID()
					<< " Woke Up | One more round ------------------------------------------------------------------"
					<< endl;
#ifdef LOG4CXX
			LOGPZ_DEBUG(ServiceLogger, sout.str().c_str());
#endif
		}
		{
			stringstream sout;
			sout << "Calling DM->HandleMessages()";
#ifdef LOG4CXX
			LOGPZ_DEBUG(ServiceLogger, sout.str().c_str());
#endif
		}
		lDM->HandleMessages();
		{
			stringstream sout;
			sout << "Called DM->HandleMessages() | Calling lDM->FlushData()";
#ifdef LOG4CXX
			LOGPZ_DEBUG(ServiceLogger, sout.str().c_str());
#endif
		}
		lDM->FlushData();
		{
			stringstream sout;
			sout
					<< "Called DM->FlushData() | Going to Sleep on WaitWakeUpCall()";
#ifdef LOG4CXX
			LOGPZ_DEBUG(ServiceLogger, sout.str().c_str());
#endif
		}
		lock.Unlock();
	}
	{
#ifdef LOG4CXX
		stringstream sout;
		sout << "Leaving DM ServiceThread calling HandleMessages and FlushData";
		LOGPZ_DEBUG(ServiceLogger, sout.str().c_str());
		std::cout << sout.str().c_str() << " for Proc " << lDM->GetProcID()
				<< endl;
#endif
	}
	lDM->HandleMessages();
	lDM->FlushData();
	{
#ifdef LOG4CXX
		stringstream sout;
		sout
				<< "HandleMessages and FlushData called, DM->ServiceThread Finished";
		LOGPZ_DEBUG(ServiceLogger, sout.str().c_str());
		std::cout << sout.str().c_str() << " for Proc " << lDM->GetProcID()
				<< endl;
#endif
	}
	return NULL;
}
void OOPDataManager::FlushData()
{
  std::set<OOPObjectId> tmpList;
  {
    OOPLock<OOPDataManager> lock(this);
    tmpList = fChangedObjects;
    fChangedObjects.clear();
  }
  std::set< OOPObjectId >::iterator itlst;
  itlst = tmpList.begin();
  while(itlst != tmpList.end())
  {
    std::map<OOPObjectId, OOPMetaData>::iterator it;
    it = fObjects.find(*itlst);
    if(it!=fObjects.end())
    {
      if(it->second.ShouldDelete())
      {
#ifdef LOG4CXX
        stringstream sout;
        sout << "Deleting Object Id:" << *itlst << " which was marked for Deletion";
        LOGPZ_DEBUG(MetaLogger, sout.str());
#endif
        fObjects.erase(*itlst);
      }else
      {
        it->second.VerifyAccessRequests(fTM->DM());
      }
    }
    tmpList.erase(itlst);
    itlst = tmpList.begin();
  }
}

void OOPDataManager::SnapShotMe(std::ostream & out)
{
  out << "DM SnapShot for processor " << GetProcID() << endl;
  out << "fObjects\n";
  {
    map < OOPObjectId, OOPMetaData >::iterator it = fObjects.begin();
    for(;it!= fObjects.end();it++)
    {
      it->second.Print(out);
    }
  }
  out << "--------------------------------------\n";
  {
    out << "fChangedObjects\n";
    std::set<OOPObjectId>::iterator it = fChangedObjects.begin();
    for(;it!=fChangedObjects.end();it++)
    {
      it->Print(out);
    }
  }
}
void OOPDataManager::SetKeepGoing(bool go)
{
#ifdef LOG4CXX
  stringstream sout;
  sout << "Setting DM KeepGoing flag to " << go;
  LOGPZ_DEBUG (ServiceLogger, sout.str().c_str());
#endif
  {
  OOPLock<OOPDataManager> lock(this);
  LOGPZ_DEBUG(logger,"acquired the lock")
  fKeepGoing = go;
  }
	WakeUpCall();
	LOGPZ_DEBUG(logger,"Leaving SetKeepGoing")
}

TPZAutoPointer<OOPTaskManager> OOPDataManager::TM()
{
	  return fTM;
}

/**
 * the autopointer to the data manager corresponding to himself
 */
TPZAutoPointer<OOPDataManager> OOPDataManager::DM()
{
	return fDM;
}
/**
 * Clear the pointer so the object can be deleted
 */
void OOPDataManager::ClearPointer()
{
	fDM = TPZAutoPointer<OOPDataManager>(0);
	fTM = TPZAutoPointer<OOPTaskManager>(0);
}

/**
 * Terminate the execution thread
 */
void OOPDataManager::JoinThread()
{
	  void *execptr;
	  void **executorresultptr = &execptr;
	  int result = pthread_join(fServiceThread,executorresultptr);
	  if(result)
	  {
#ifdef LOG4CXX
	    stringstream sout;
	    sout << __FUNCTION__ << __LINE__ << " join operation failed with result " << result;
	    LOGPZ_ERROR(logger,sout.str())
#endif
	  }
	  fServiceThread = 0;
	  LOGPZ_DEBUG(logger,"DataManager Jointhread succeeded")

}


//////////////////////OOPDMOwnerTask////////////////////////////////////////////
template class TPZRestoreClass<OOPDMOwnerTask,TDMOWNERTASK_ID>;


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
    fTM->DM()->ObjectChanged(id);
  }
}
//***********************************************************************

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
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << "Tag Received ";
    fTag.ShortPrint( sout);
    sout << "TransferReqTask size " << size << "\n";
    LOGPZ_DEBUG(DaemonLogger, sout.str().c_str());
  }
#endif
  int i;
  for(i=0; i<size; i++)
  {
    OOPAccessTag tag;
    tag.Read(buf,0);
    fTransferRequests.insert(tag);
  }
#ifdef LOG4CXX
  {
    stringstream sout;
    sout << "<--Receiveing OwnerTask with Tag:";
    fTag.ShortPrint( sout);
    LOGPZ_DEBUG(DaemonLogger, sout.str().c_str());
  }
#endif
}
void OOPDMOwnerTask::Write (TPZStream& buf, int withclassid)
{
  {
#ifdef LOG4CXX
    stringstream sout;
    sout << "Packing Owner task for Obj " << fTag.Id() << " message type "
	 << fTag.AccessMode() << " with objptr " << (fTag.AutoPointer() != 0) << " version " << fTag.Version()
	 << " To Proc " << fTag.Proc();
    if(fTag.AutoPointer()) sout << " class id " << fTag.AutoPointer()->ClassId();
    sout << "\nTransferReqTask size " << fTransferRequests.size() << endl;
    LOGPZ_DEBUG(logger,sout.str().c_str());
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
  fTM->DM()->GetUpdate (this);
#ifdef LOG4CXX
  stringstream sout;
  sout << "Executting OwnerTask";
  LOGPZ_DEBUG(DaemonLogger,sout.str().c_str());
#endif
  fTM->DM()->WakeUpCall();
  return ESuccess;
}

template class TPZRestoreClass<OOPDMRequestTask, TDMREQUESTTASK_ID>;

OOPMReturnType OOPDMRequestTask::Execute ()
{
  fTM->DM()->GetUpdate (this);
#ifdef LOG4CXX
  stringstream sout;
  sout << "Executting RequestTask";
  LOGPZ_DEBUG(DaemonLogger,sout.str().c_str());
#endif
  fTM->DM()->WakeUpCall();
  return ESuccess;
}
void OOPDMRequestTask::Read(TPZStream & buf, void * context)
{
  OOPDaemonTask::Read(buf, context);
  fDepend.Read( buf, context);
#ifdef LOG4CXX
  {
    std::stringstream sout;
    sout <<  "<--Receiving RequestTask with Tag:";
    fDepend.ShortPrint( sout);
    LOGPZ_DEBUG(DaemonLogger,sout.str().c_str());
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
#ifdef LOG4CXX
 sout << __PRETTY_FUNCTION__ << " Writing request task proc origin " << fDepend.Proc() << " fProc " << fProc;
 LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
  OOPDaemonTask::Write (buf, withclassid);
  fDepend.Write (buf, withclassid);

}

void OOPDMRequestTask::LogMe(std::ostream & out){
  out << " Depend " << fDepend;
  out << " fProc " << fProc;
}

