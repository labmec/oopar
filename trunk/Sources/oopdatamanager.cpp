// -*- c++ -*-
#include "oopdatamanager.h"
#include "oopmetadata.h"
#include "ooptaskmanager.h"
#include "ooppardefs.h"
#include "oopcommmanager.h"
#include <stdlib.h>

//#include "../gnu/gnudefs.h"

//Includes for testing
//#include "tmultidata.h"
//#include "tmultitask.h"
class OOPSendStorage;
class OOPReceiveStorage;
class OOPDataVersion;
class OOPMetaData;
class OOPDMRequestTask;
class OOPDMOwnerTask;
class OOPSaveable;


using namespace std;

class OOPObjectId;
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
		cout << "Data Id set to :" ;
		id[i].Print(cout);
		cout.flush();
	}
	
	tid.Print(cout);
	
	for(i=0;i<40;i++){		
		DM->SubmitAccessRequest(tid, id[i], ver, st, 0);
	}
	
	cout << "Deleting Object " << endl;
	id[20].Print(cout);
	DM->DeleteObject(id[20]);
	DM->DeleteObject(id[20]);
	
	if(DM->FindObject(id[20])) cout << "Object found" << endl;
		
	OOPSaveable * ptr = (TMultiData *) DM->GetObjPtr(id[30]);
	int proc;
	proc = DM->GetProcID();
	
	TDataVersion ver2 = dat[10].Version();
	ver2.Print(cout);
	for(i=0;i<30;i++){
		DM->IncrementVersion(id[10]);
	}
	
	cout << DM->HasAccess(id[10], tid, st, ver) << endl; 
	
	TMultiData dat_m;
	OOPObjectId id_m;
	id_m.SetProcId(2);
	id_m.SetId(20);

		
	cout << DM->HasAccess(id_m, tid, st, ver) << endl; 
	
	TMultiTask tm_a(0);
	TMultiData md_a ;
	
	OOPObjectId id_da, id_ta;
	id_da = DM->SubmitObject(&md_a,0);
	id_da.Print(cout);
	id_ta = TM->Submit(&tm_a);
	id_ta.Print(cout);
	
	TDataVersion versao;
	versao.SetLevelVersion(0,5);
	tm_a.AddDependentData(id_da, st, versao);
	DM->SubmitAccessRequest(id_ta, id_da, versao, st, 0);
	
	//OOPMetaData * md_ad = DM->Data(id_da);

	DM->Data(id_da)->SetCardinality(0,10);
	DM->Data(id_da)->IncrementVersionLevel(12);
	versao = DM->Data(id_da)->Version();
	
	versao.Print(cout);
	DM->Data(id_da)->IncrementVersionLevel(25);
	versao = DM->Data(id_da)->Version();
	versao.Print(cout);
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
	versao.Print(cout);
	DM->Data(id_da)->DecreaseVersionLevel();
	versao = DM->Data(id_da)->Version();
	versao.Print(cout);
	DM->Data(id_da)->DecreaseVersionLevel();
	versao = DM->Data(id_da)->Version();
	versao.Print(cout);
	DM->Data(id_da)->DecreaseVersionLevel();
	versao = DM->Data(id_da)->Version();
	versao.Print(cout);
	
}
*/

OOPDataManager::~OOPDataManager(){
	deque<OOPMetaData *>::iterator i;
	for(i=fObjects.begin();i!=fObjects.end();i++){
		OOPSaveable * dead = (*i)->Ptr();
		delete dead;
		delete *i;
	}
	fObjects.clear();
}
	


bool OOPDataManager::HasObject(OOPObjectId id){
	//Implement a search on the fObjects vector
	deque<OOPMetaData *>::iterator i;
	for(i=fObjects.begin();i!=fObjects.end();i++){
		//OOPMetaData * dat = (OOPMetaData *)(*i);
		if (id == (*i)->Id()) return true;
	}
	return false;
}

void OOPDataManager::ReleaseAccessRequest(const OOPObjectId & TaskId, const OOPMDataDepend &depend) //, long ProcId){
{
	deque<OOPMetaData *>::iterator i;
	bool found=false;
	
	for(i=fObjects.begin();i!=fObjects.end();i++){
		if (depend.Id() == (*i)->Id()){
			found = true;
			(*i)->ReleaseAccess(TaskId, depend);
			
			break;
		}
	}
	if(!found){
		//Erro, alguma coisa errada, dado supostamente deveria estar aqui.
		//Submeter pedido de acesso ao processador que criou o dado.
		
		return;
	}
	
	

}


int OOPDataManager::SubmitAccessRequest(const OOPObjectId & TaskId, const OOPMDataDepend &depend, const long ProcId){
	
	deque<OOPMetaData *>::iterator i;
	bool found=false;
#ifndef WIN32
#warning "Wrong logical  sequence in OOPDataManager::SubmitAccessRequest"
#endif
	for(i=fObjects.begin();i!=fObjects.end();i++){
		//OOPMetaData * dat = (OOPMetaData *)(*i);
		if (depend.Id() == (*i)->Id()){
			found = true;
			if(! depend.Version().AmICompatible((*i)->Version())) return 0;
			(*i)->SubmitAccessRequest(TaskId, depend);
			//				cout << "Access request submitted" << endl;
			//				(*i)->Print(cout);
			break;
		}
	}
	if(!found) {
		cout << "Some appropriate action should be taken\n";
		cout << "Create a metadata object on the fly\n";
		return 0;
	}
	return 1;
			
}

OOPDataManager::OOPDataManager(int Procid) : fObjects(0) {
	 fProcessor = Procid;
	 fObjId.SetProcId(Procid);
	
	 fLastCreated = 0;//NUMOBJECTS * Procid;
	 fMaxId = 1000;//fLastCreated + NUMOBJECTS;
	 DM = this;
}


/**
 * HELP ....
 */
//Precisamos conversar !!!
// Nao sei porque, esta tudo certo
//.........
OOPObjectId OOPDataManager::SubmitObject(OOPSaveable *obj, int trace) {
	//como fazer ?? 
	OOPObjectId id = DM->GenerateId();
	OOPMetaData * dat = new OOPMetaData(obj, id, fProcessor);
	dat->SetTrace(trace);//Erico
	fObjects.push_back(dat);//[id] = dat;
	/*
	TDMOwnerTask ms(ENotifyCreateObject,-1);
	//ms.fTaskId = //0;
	ms.fTrace = trace;//Erico
	ms.fProcDestination = -1;
	ms.fProcOrigin = fProcessor;
	ms.fObjId = id;
	CM->SendTask(&ms);*/
	//cout << "Object submitted." << endl;
	return id;
}

void OOPDataManager::DeleteObject(OOPObjectId & ObjId) {
	deque<OOPMetaData *>::iterator i;
	//OOPMetaData *dat=0;
	bool found = false;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		//dat = 0;
		//dat = (OOPMetaData*) (*i);
		if ((*i)->Id() == ObjId) {
			found = true;
			delete (*i);
			fObjects.erase(i);
			break;
		}
	}
	if(!found) {
		// Issue a sever warning message !!!
		cerr << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
	}
}

void OOPDataManager::RequestDeleteObject(OOPObjectId & ObjId) {
	deque<OOPMetaData *>::iterator i;
	//OOPMetaData *dat=0;
	bool found = false;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		//dat = 0;
		//dat = (OOPMetaData*) (*i);
		if ((*i)->Id() == ObjId) {
			found = true;
			(*i)->RequestDelete();
			break;
		}
	}
	if(!found) {
		// Issue a sever warning message !!!
		cerr << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
	}
			
}

void OOPDataManager::TransferObject(OOPObjectId & ObjId, int ProcId) {
	deque<OOPMetaData *>::iterator i;
	OOPMetaData *dat=0;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		dat = (*i);
		if (dat->Id() == ObjId) break;
		dat = 0;
	}
	

	// Something very strange is happening!
	if(!dat) return;  // issue a sever warning message

	/**
		Identificar se o objeto foi criado neste nó, se sim alterar
		a informação de sua localização
	 */
	dat->TransferObject(ProcId);
}



void OOPDataManager::GetUpdate(OOPDMOwnerTask *task){

	if(task->fType == ENotifyCreateObject) {
		if(Data(task->fObjId)) {
			// severe warning message
			delete Data(task->fObjId);
		}
		OOPMetaData *d  = new OOPMetaData(0,task->fObjId,task->fProcOrigin);//Erico
		d->SetTrace(task->fTrace);
		fObjects.push_back(d);//[task->fObjId] = d;

	}
	OOPMetaData *dat = Data(task->fObjId);
	if(!dat) {
		cout << "TDataManager:GetUpdate called with invalid ojbid:";
		task->fObjId.Print(cout);
		exit(-1);
		return;
	}
	if(task->fType == ENotifyDeleteObject) {
		dat->DeleteObject();
	} else {
		dat->HandleMessage(*task);
	}
}

void OOPDataManager::GetUpdate(OOPDMRequestTask *task){

	switch(task->fType) {
		case ENoRequest :
			break;
		case ERequestReadAccess :
		case ERequestWriteAccess :
			TransferObject(task->fObjId,task->fProcOrigin);
			break;
		case ERequestDelete :
			DeleteObject(task->fObjId);
			break;
		default:
			break;
	}
}



OOPObjectId OOPDataManager::GenerateId() {
	fLastCreated++;
	if(fLastCreated >= fMaxId) exit(-1);	// the program ceases to function
	OOPObjectId * obj = new OOPObjectId(GetProcID(), fLastCreated);
	return *obj;//fLastCreated;
}


OOPMetaData *OOPDataManager::Data(OOPObjectId ObjId) {
	deque<OOPMetaData *>::iterator i;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		if ((*i)->Id() == ObjId) {
			return (*i);
		}
	}
	return 0;
}



OOPDMOwnerTask::OOPDMOwnerTask(OOPMDMOwnerMessageType t, int proc) : OOPDaemonTask(proc)
			/*, fAccessProcessors(0), fBlockingReadProcesses(0)*/ {
	fVersion = 0;
	fType = t;
	fObjPtr = 0;
	fProcDestination = proc;
	fProcOrigin = DM->GetProcID();
//	fObjId = 0;
	fTrace = 0;//Erico
}




//***********************************************************************

OOPDMRequestTask::OOPDMRequestTask(OOPDMRequestMessageType t, int proc) : OOPDaemonTask(proc) {
	fType = t;
	//fTaskId = 0;		// id of the task which originated this message
	fProcDestination = proc;
	fProcOrigin = DM->GetProcID();
	//fObjId = 0;
}

int OOPDMOwnerTask::Unpack( OOPReceiveStorage *buf) {
	OOPDaemonTask::Unpack(buf);
	int numitens;
	char type;
	buf->UpkByte(&type);
	fType = (OOPMDMOwnerMessageType) type;
	int access;
	buf->UpkInt(&access);
	fState = (OOPMDataState) access;
//	buf->UpkLong(&fVersion);
	fVersion.Unpack(buf);
	fObjPtr = buf->Restore();
	//buf->UpkLong(&fTaskId);
	fTaskId.Unpack(buf);
	buf->UpkInt(&fTrace);
	buf->UpkInt(&fProcDestination);
	buf->UpkInt(&fProcOrigin);
	//Não faz sentido !!!
	
//	numitens = buf->UpkLong(&fObjId);
	long clid;

#define LONGVEC_ID 123456
	numitens = buf->UpkLong(&clid);
	if(clid != LONGVEC_ID) {
		cout << "TDMOwnerTask.UnPack1 clid = " << clid << endl;
		exit(-1);
	}
	//fAccessProcessors.Unpack(buf);
	long sz=0;
	buf->UpkLong(&sz);
	long procs=0;
	int i;
	for(i=0;i<sz;i++) {
		buf->UpkLong(&procs);
		fAccessProcessors.push_back(procs);
	}
	buf->UpkLong(&clid);
	if(clid != LONGVEC_ID) {
		cout << "TDMOwnerTask.UnPack2 clid = " << clid << endl;
		exit(-1);
	}
//	fBlockingReadProcesses.Unpack(buf);
	return 1;
}

OOPSaveable *OOPDMOwnerTask::Restore(OOPReceiveStorage *buf){
	OOPDMOwnerTask *t = new OOPDMOwnerTask(ENoMessage,0);
	t->Unpack(buf);
	return t;
}

int OOPDMOwnerTask::Pack( OOPSendStorage *buf ){
	OOPDaemonTask::Pack(buf);
	char type = fType;
	buf->PkByte(&type);
	int access = fState;
	buf->PkInt(&access);
	fVersion.Pack(buf);//buf->PkLong(&fVersion);
	if(fObjPtr) {
		fObjPtr->Pack(buf);
	} else {
		long zero = 0;
		buf->PkLong(&zero);
	}
	fTaskId.Pack(buf);//buf->PkLong(&fTaskId);
	buf->PkInt(&fTrace);
	buf->PkInt(&fProcDestination);
	buf->PkInt(&fProcOrigin);
	fObjId.Pack(buf);//buf->PkLong(&fObjId);
	long sz = fAccessProcessors.size();
	buf->PkLong(&sz);
	int i;
	for(i=0;i<sz;i++) buf->PkLong(&fAccessProcessors[i]);
	//fAccessProcessors.Pack(buf);
//	fBlockingReadProcesses.Pack(buf);
	return 1;
}


OOPMReturnType OOPDMOwnerTask::Execute() {
	//DM->GetUpdate(this);
	return ESuccess;
}

OOPMReturnType OOPDMRequestTask::Execute() {
	//DM->GetUpdate(this);
	return ESuccess;
}

int OOPDMRequestTask::Unpack( OOPReceiveStorage *buf) {
	OOPDaemonTask::Unpack(buf);
	int type;
	buf->UpkInt(&type);
	fType = (OOPDMRequestMessageType) type;
	int access;
	buf->UpkInt(&access);
	fAccessState = (OOPMDataState) access;
	fVersion.Unpack(buf);//buf->UpkLong(&fVersion);
	fTaskId.Unpack(buf);//buf->UpkLong(&fTaskId);
	buf->UpkInt(&fProcDestination);
	buf->UpkInt(&fProcOrigin);
	fObjId.Unpack(buf);//buf->UpkLong(&fObjId);
	return 1;
}

OOPSaveable *OOPDMRequestTask::Restore(OOPReceiveStorage *buf){
	OOPDMRequestTask *t = new OOPDMRequestTask(ENoRequest,0);
	t->Unpack(buf);
	return t;
}
  
int OOPDMRequestTask::Pack( OOPSendStorage *buf ){
	OOPDaemonTask::Pack(buf);
	int type = fType;
	buf->PkInt(&type);
	int access = fAccessState;
	buf->PkInt(&access);
	fVersion.Pack(buf);//buf->PkLong(&fVersion);
	fTaskId.Pack(buf);//buf->PkLong(&fTaskId);
	buf->PkInt(&fProcDestination);
	buf->PkInt(&fProcOrigin);
	fObjId.Pack(buf);//buf->PkLong(&fObjId);
	return 1;
}

