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
class OOPCurrentLocation;


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
	

void OOPDataManager::CheckAccessRequests(){
	deque<OOPMetaData *>::iterator i;
	for(i=fObjects.begin();i!=fObjects.end();i++){
		(*i)->VerifyAccessRequests();
	}
}

bool OOPDataManager::FindObject(OOPObjectId id){
	//Implement a search on the fObjects vector
	deque<OOPMetaData *>::iterator i;
	for(i=fObjects.begin();i!=fObjects.end();i++){
		//OOPMetaData * dat = (OOPMetaData *)(*i);
		if (id == (*i)->Id()) return true;
	}
	return false;
}

void OOPDataManager::ReleaseAccessRequest(OOPObjectId & TaskId, OOPObjectId & dataId, OOPDataVersion & version, OOPMDataState access) //, long ProcId){
{
	deque<OOPMetaData *>::iterator i;
	bool found=false;
	
	for(i=fObjects.begin();i!=fObjects.end();i++){
		if (dataId == (*i)->Id()){
			found = true;
			(*i)->ReleaseAccess(TaskId, access, version);
			//Talvez aqui!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			break;
		}
	}
	if(!found){
		//Erro, alguma coisa errada, dado supostamente deveria estar aqui.
		//Submeter pedido de acesso ao processador que criou o dado.
		
		return;
	}
	
	

}

void OOPDataManager::IncrementLevel(OOPObjectId & Id, int depth, long ProcId){
	
	deque<OOPMetaData *>::iterator i;
	bool found=false;
	if (fProcessor == ProcId) {
		for(i=fObjects.begin();i!=fObjects.end();i++){
			//OOPMetaData * dat = (OOPMetaData *)(*i);
			if (Id == (*i)->Id()){
				found = true;
				(*i)->IncrementVersionLevel(depth);
				break;
			}
		}
		if(!found){
			//Erro, alguma coisa errada, dado supostamente deveria estar aqui.
			//Submeter pedido de acesso ao processador que criou o dado.
			
			return;
		}
	}else{
		/*ENoAccess,
		EReadAccess,
		EWriteAccess*/
		//Encontrar onde se encontra o OOPMetaData especificado.
		//Enviar recado para quem o criou !
		//Enviar mensagem pedindo a altera��o do cardinalidade no processador owner do metadata.
	}
			
}

void OOPDataManager::SubmitAccessRequest(OOPObjectId & TaskId, OOPObjectId & dataId, OOPDataVersion & version, OOPMDataState access, long ProcId){
	//Access the specified data object and append an willing task.
	OOPDMRequestMessageType req;
	switch (access)
	{
		case  EReadAccess:
		{
			req = ERequestReadAccess;
			break;
		}
		case  EWriteAccess:
		{
			req = ERequestWriteAccess;
			break;
		}
		case  EVersionAccess:
		{
			req = ERequestWriteAccess;// Aten��o EVersionAccess;
			break;
		}
		default:
			req = ENoRequest;
			break;
	}
	
	deque<OOPMetaData *>::iterator i;
	bool found=false;
	if (fProcessor == ProcId) {
		for(i=fObjects.begin();i!=fObjects.end();i++){
			//OOPMetaData * dat = (OOPMetaData *)(*i);
			if (dataId == (*i)->Id()){
				found = true;
				(*i)->SubmitAccessRequest(TaskId, version, access, ProcId);
				cout << "Access request submitted" << endl;
				(*i)->Print(cout);
				break;
			}
		}
		if(!found){
			//Erro, alguma coisa errada, dado supostamente deveria estar aqui.
			//Submeter pedido de acesso ao processador que criou o dado.
			OOPObjectId id;
			id = dataId.Id();
			
			OOPDMRequestTask ms(req,id.GetProcId());
			ms.fProcDestination = id.GetProcId();
			ms.fProcOrigin = fProcessor;
			//Id , como passar ?
	//		ms.fObjId = id;
			CM->SendTask(&ms);
			
			return;
		}
	}else{
		/*ENoAccess,
		EReadAccess,
		EWriteAccess*/
		//Encontrar onde se encontra o OOPMetaData especificado.
		//Enviar recado para quem o criou !
		OOPDMRequestTask ms(req,ProcId);
		ms.fProcDestination = ProcId;
		ms.fProcOrigin = fProcessor;
		//Id , como passar ?
//		ms.fObjId = id;
		CM->SendTask(&ms);
		
		
	}
			
}

OOPDataManager::OOPDataManager(int Procid) : fObjects(0) {
	 fProcessor = Procid;
	 fObjId.SetProcId(Procid);
	
	 fLastCreated = 0;//NUMOBJECTS * Procid;
	 fMaxId = 1000;//fLastCreated + NUMOBJECTS;
	 DM = this;
}

OOPSaveable *OOPDataManager::GetObjPtr(OOPObjectId ObjId) {
	return Data(ObjId)->Ptr();
}
/**
 * HELP ....
 */
//Precisamos conversar !!!
//.........
OOPObjectId OOPDataManager::SubmitObject(OOPSaveable *obj, int trace) {
	//como fazer ?? 
	OOPObjectId id = GenerateId();
	OOPMetaData * dat = new OOPMetaData(obj, id, fProcessor);
	dat->SetTrace(trace);//Erico
	fObjects.push_back(dat);//[id] = dat;
	SetCurrentLocation(fProcessor, id);
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
/**
 * Novamente opera��es com fObjects ... n�o tenho certeza
 */
void OOPDataManager::IncrementVersion(OOPObjectId & ObjId) {
	deque<OOPMetaData *>::iterator i;
	//	OOPMetaData *dat;
	bool found = false;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		//dat = (OOPMetaData*) (*i);
		if ((*i)->Id() == ObjId){
			found = true;
			(*i)->IncrementVersion();
			#ifdef DEBUG
			cout << "Data version incremented" << endl;
			#endif
			break;
		}
	}
	if(!found){
		//Enviar mensagem para datamanager que contem o metadado
		//Buscar no DataId o processador no qual foi criado e viabilizar a busca
		//do objeto.
	}
}
int OOPDataManager::DeleteObject(OOPObjectId & ObjId) {
	deque<OOPMetaData *>::iterator i;
	//OOPMetaData *dat=0;
	bool found = false;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		//dat = 0;
		//dat = (OOPMetaData*) (*i);
		if ((*i)->Id() == ObjId) {
			found = true;
			if((*i)->DeleteObject(ObjId)) {
				fObjects.erase(i);
				delete (*i);
				return 1;
			} else {
				return 0;
			}
			break;
		}
	}
	if(!found) {
		// Issue a sever warning message !!!
		cerr << "Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
		return 0;
	}
			
		
/*	 Pix i = fObjects.seek(ObjId);
	 if(i) {
		OOPMetaData *dat = (OOPMetaData *) fObjects.contents(i);
		if(dat) {
			if(dat->DeleteObject(ObjId)) {
				fObjects.del(ObjId);
				delete dat;
				return 1;
			} else {
				return 0;
			}
		} else {
			fObjects.del(fObjects.key(i));
			// Issue a sever warning message !!!
			return 1;
		}
	}*/
	return 1;
}

int OOPDataManager::TransferObject(OOPObjectId & ObjId, int ProcId, OOPObjectId & TaskId,
				OOPMDataState AccessRequest, OOPDataVersion & version) {
	deque<OOPMetaData *>::iterator i;
	OOPMetaData *dat=0;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		dat = (*i);
		if (dat->Id() == ObjId) break;
		dat = 0;
	}
	

	// Something very strange is happening!
	if(!dat) return 0;  // issue a sever warning message

	/**
		Identificar se o objeto foi criado neste n�, se sim alterar
		a informa��o de sua localiza��o
	 */
	OOPObjectId auxId = dat->Id();
	if(auxId.GetProcId()==fProcessor){
		SetCurrentLocation(ProcId, auxId);
	}else{
		//Criar uma mensagem ?
	}
	/* Retirar isso ?
	if(dat->HasAccess(ProcId,TaskId,AccessRequest,version)) return 1;*/
	return dat->TransferObject(ProcId, TaskId, AccessRequest, version);
}

int OOPDataManager::HasAccess(OOPObjectId & ObjId, OOPObjectId & TaskId, OOPMDataState AccessType, OOPDataVersion & version){
	deque<OOPMetaData *>::iterator i;
	//	OOPMetaData *dat=0;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		//dat = (OOPMetaData*) (*i);
		if ((*i)->Id() == ObjId) {
			return (*i)->HasAccess(TaskId, AccessType, version);
		}
		//dat = 0;
	}
	return 0;
	//if(!dat) return 0;  // issue a sever warning message
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
		dat->DeleteObject(task->fObjId);
		delete dat;
		deque<OOPMetaData *>::iterator i;
		OOPMetaData *dat=0;
		for(i = fObjects.begin(); i!=fObjects.end();i++){
			dat = (*i);
			if (dat->Id() == task->fObjId) {
				fObjects.erase(i);
				break;
			}
		}
		
		
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
			TransferObject(task->fObjId,task->fProcOrigin,task->fTaskId,
													task->fAccessState,task->fVersion);
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
	OOPObjectId tmp(GetProcID(), fLastCreated);
	return tmp;//fLastCreated;
}

OOPMetaData *OOPDataManager::MetaData(OOPObjectId ObjId) {
	deque<OOPMetaData *>::iterator i;
	for(i = fObjects.begin(); i!=fObjects.end();i++){
		if ((*i)->Id() == ObjId) {
			return (*i);
		}
	}
	return 0;
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

long OOPDataManager::CurrentLocation(OOPObjectId & Id){
	deque<OOPCurrentLocation>::iterator i;
	for(i=fCurrLocation.begin();i!=fCurrLocation.end();i++){
		if (i->fObjectId==Id) {
			return i->fProcessor;
		}
	}
	return -1;
}
void OOPDataManager::SetCurrentLocation(long Processor, OOPObjectId & Id){
	if (fProcessor != Processor) {
		/**
			Criar uma mensagem para atualiza��o de localidade do objeto ?
		 */
		
		return;
	}
	bool found = false;
	deque<OOPCurrentLocation>::iterator i;
	for(i=fCurrLocation.begin();i!=fCurrLocation.end();i++){
		if (i->fObjectId==Id) {
			found = true;
			i->fProcessor = Processor;
		}
	}
	if(!found){
		OOPCurrentLocation location(Processor, Id);
		fCurrLocation.push_back(location);
	}
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
	//N�o faz sentido !!!
	
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

int OOPDMOwnerTask::DerivedFrom(long Classid) {
	if(Classid == GetClassID()) return 1;
	return OOPDaemonTask::DerivedFrom(Classid);
}

int OOPDMOwnerTask::DerivedFrom(char *classname) {
	if(!strcmp(ClassName(),classname)) return 1;
	return OOPDaemonTask::DerivedFrom(classname);
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

int OOPDMRequestTask::DerivedFrom(long Classid) {
	if(Classid == GetClassID()) return 1;
	return OOPDaemonTask::DerivedFrom(Classid);
}

int OOPDMRequestTask::DerivedFrom(char *classname) {
	if(!strcmp(ClassName(),classname)) return 1;
	return OOPDaemonTask::DerivedFrom(classname);
}
void OOPDataManager::IncrementLevel(OOPObjectId & TaskId, OOPObjectId & Id, int depth, long ProcId){
	
	deque<OOPMetaData *>::iterator i;
	bool found=false;
	if (fProcessor == ProcId) {
		for(i=fObjects.begin();i!=fObjects.end();i++){
			//OOPMetaData * dat = (OOPMetaData *)(*i);
			if (Id == (*i)->Id()){
				found = true;
				(*i)->IncrementVersionLevel(depth);
				break;
			}
		}
		if(!found){
			//Erro, alguma coisa errada, dado supostamente deveria estar aqui.
			//Submeter pedido de acesso ao processador que criou o dado.
			
			return;
		}
	}else{
		/*ENoAccess,
		EReadAccess,
		EWriteAccess*/
		//Encontrar onde se encontra o OOPMetaData especificado.
		//Enviar recado para quem o criou !
		//Enviar mensagem pedindo a altera��o do cardinalidade no processador owner do metadata.
	}
			
}

