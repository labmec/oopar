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
class   OOPSendStorage;
class   OOPReceiveStorage;
class   OOPDataVersion;
class   OOPMetaData;
class   OOPDMRequestTask;
class   OOPDMOwnerTask;
class   OOPSaveable;
using namespace std;
class   OOPObjectId;
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
	deque < OOPMetaData * >::iterator i;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		OOPSaveable *dead = (*i)->Ptr ();
		delete  dead;
		delete *i;
	}
	fObjects.clear ();
	DataManLog << "Terminating DM\n";
	DataManLog.flush();
}
bool OOPDataManager::HasObject (OOPObjectId id)
{
	// Implement a search on the fObjects vector
	deque < OOPMetaData * >::iterator i;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		// OOPMetaData * dat = (OOPMetaData *)(*i);
		if (id == (*i)->Id ())
			return true;
	}
	return false;
}
void OOPDataManager::ReleaseAccessRequest (const OOPObjectId & TaskId, const OOPMDataDepend & depend)	// , 
													// long 
													// ProcId){
{
	deque < OOPMetaData * >::iterator i;
	bool found = false;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		if (depend.Id () == (*i)->Id ()) {
			found = true;
			(*i)->ReleaseAccess (TaskId, depend);
			break;
		}
	}
	if (!found) {
		// Erro, alguma coisa errada, dado supostamente deveria estar 
		// aqui.
		// Submeter pedido de acesso ao processador que criou o dado.
		return;
	}
}
int OOPDataManager::SubmitAccessRequest (const OOPObjectId & TaskId,
					 const OOPMDataDepend & depend,
					 const long ProcId)
{
	DataManLog << GLogMsgCounter << endl;
	GLogMsgCounter++;
	deque < OOPMetaData * >::iterator i;
	bool found = false;
#ifndef WIN32
#warning "Wrong logical  sequence in OOPDataManager::SubmitAccessRequest"
#endif
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		// OOPMetaData * dat = (OOPMetaData *)(*i);
		if (depend.Id () == (*i)->Id ()) {
			found = true;
			if (!depend.Version ().
			    AmICompatible ((*i)->Version ()))
				return 0;
			(*i)->SubmitAccessRequest (TaskId, depend,
						   GetProcID ());
			DataManLog << "Access request submitted" << endl;
			(*i)->Print(DataManLog);
			break;
		}
	}
	if (!found) {
		if (depend.Id ().GetProcId () == fProcessor) {
			DataManLog << "SubmitAccessRequest for deleted object, returning 0\n";
			return 0;
		}
		else {
			OOPMetaData *dat =
				new OOPMetaData (0, depend.Id (),
						 depend.Id ().GetProcId ());
			dat->SetTrace (true);	// Erico
			fObjects.push_back (dat);	// [id] = dat;
			dat->SubmitAccessRequest (TaskId, depend,
						  GetProcID ());
			// DataManLog << "Some appropriate action should be
			// taken\n";
			// DataManLog << "Create a metadata object on the fly\n";
			return 1;
		}
	}
	return 1;
}
OOPDataManager::OOPDataManager (int Procid):fObjects (0)
{
	fProcessor = Procid;
	fObjId.SetProcId (Procid);
	fLastCreated = 0;	// NUMOBJECTS * Procid;
	fMaxId = 1000;	// fLastCreated + NUMOBJECTS;
	//DM = this;
}
/**
 * HELP ....
 */
//Precisamos conversar !!!
// Nao sei porque, esta tudo certo
//.........
OOPObjectId OOPDataManager::SubmitObject (OOPSaveable * obj, int trace)
{
	// como fazer ?? 
	OOPObjectId id = DM->GenerateId ();
	OOPMetaData *dat = new OOPMetaData (obj, id, fProcessor);
	dat->SetTrace (trace);	// Erico
	fObjects.push_back (dat);	// [id] = dat;
	/* 
	 * TDMOwnerTask ms(ENotifyCreateObject,-1); //ms.fTaskId = //0;
	 * ms.fTrace = trace;//Erico ms.fProcDestination = -1; ms.fProcOrigin 
	 * = fProcessor; ms.fObjId = id; CM->SendTask(&ms); */
	// DataManLog << "Object submitted." << endl;
	return id;
}
void OOPDataManager::DeleteObject (OOPObjectId & ObjId)
{
	deque < OOPMetaData * >::iterator i;
	// OOPMetaData *dat=0;
	bool found = false;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		// dat = 0;
		// dat = (OOPMetaData*) (*i);
		if ((*i)->Id () == ObjId) {
			found = true;
			delete (*i);
			fObjects.erase (i);
			break;
		}
	}
	if (!found) {
		// Issue a sever warning message !!!
		cerr << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
	}
}
void OOPDataManager::RequestDeleteObject (OOPObjectId & ObjId)
{
	deque < OOPMetaData * >::iterator i;
	// OOPMetaData *dat=0;
	bool found = false;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		// dat = 0;
		// dat = (OOPMetaData*) (*i);
		if ((*i)->Id () == ObjId) {
			found = true;
			(*i)->RequestDelete ();
			break;
		}
	}
	if (!found) {
		// Issue a sever warning message !!!
		cerr << "OOPDataManager::DeleteObject Inconsistent object deletion File:" << __FILE__ << " Line:" << __LINE__ << endl;
	}
}
void OOPDataManager::TransferObject (OOPObjectId & ObjId, int ProcId)
{
	deque < OOPMetaData * >::iterator i;
	OOPMetaData *dat = 0;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		dat = (*i);
		if (dat->Id () == ObjId)
			break;
		dat = 0;
	}
	// Something very strange is happening!
	if (!dat)
		return;	// issue a sever warning message
	/**
		Identificar se o objeto foi criado neste n�, se sim alterar
		a informa��o de sua localiza��o
	 */
	dat->TransferObject (ProcId);
}
void OOPDataManager::GetUpdate (OOPDMOwnerTask * task)
{
	DataManLog << GLogMsgCounter << endl;
	GLogMsgCounter++; 
	DataManLog << "Calling GetUpdate(OOPDMOwnerTask)\n"; 
	OOPMetaData *dat = Data (task->fObjId);
	if (!dat) {
		DataManLog << "TDataManager:GetUpdate called with invalid ojbid:";
		task->fObjId.Print (DataManLog);
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
	deque < OOPMetaData * >::iterator i;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		if (id == (*i)->Id ())
			break;
	}
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
		if(!(*i)->IamOwner()) {
			OOPDMRequestTask *ntask = new OOPDMRequestTask(*task);
			ntask->SetProcID((*i)->Proc());
			TM->SubmitDaemon(ntask);
		} else if((*i)->IamOwner() && task->fProcOrigin == (*i)->Proc()) {
			cout << "Task request ignored\n";
		} else {
			(*i)->SubmitAccessRequest (OOPObjectId(), task->fDepend,
					   task->fProcOrigin);
		}
	}
}
OOPObjectId OOPDataManager::GenerateId ()
{
	fLastCreated++;
	if (fLastCreated >= fMaxId)
		exit (-1);	// the program ceases to function
	OOPObjectId *obj = new OOPObjectId (GetProcID (), fLastCreated);
	return *obj;	// fLastCreated;
}
OOPMetaData *OOPDataManager::Data (OOPObjectId ObjId)
{
	deque < OOPMetaData * >::iterator i;
	for (i = fObjects.begin (); i != fObjects.end (); i++) {
		if ((*i)->Id () == ObjId) {
			return (*i);
		}
	}
	return 0;
}
void OOPDataManager::PrintDataQueues(char * msg, ostream & out){
	out << "Printing Data Queues on processor :" << fProcessor << msg << endl;
	deque < OOPMetaData * >::iterator i;
	OOPAccessInfoList auxlist;
	for(i=fObjects.begin();i!=fObjects.end();i++){
		(*i)->PrintLog(out);
	}
	
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
int OOPDMOwnerTask::Unpack (OOPReceiveStorage * buf)
{
	OOPDaemonTask::Unpack (buf);
	int numitens;
	char type;
	buf->UpkByte (&type);
	fType = (OOPMDMOwnerMessageType) type;
	int access;
	buf->UpkInt (&access);
	fState = (OOPMDataState) access;
//      buf->UpkLong(&fVersion);
	fVersion.Unpack (buf);
	fObjPtr = buf->Restore ();
	// buf->UpkLong(&fTaskId);
	buf->UpkInt (&fTrace);
	buf->UpkInt (&fProcOrigin);
	// N�o faz sentido !!!
	fObjId.Unpack(buf);
	DataLog << "Unpacking Owner task for Obj " << fObjId << " message type " <<
		fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion <<
		endl;
	return 1;
}
OOPSaveable *OOPDMOwnerTask::Restore (OOPReceiveStorage * buf)
{
	OOPDMOwnerTask *t = new OOPDMOwnerTask (ENoMessage, 0);
	t->Unpack (buf);
	return t;
}
void OOPDMOwnerTask::LogMe(ostream & out){
	out << fProc;
	out << "\tId "<< fObjId;
	out << "\tSending ";
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
	out << "OwnerType ";	
	
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
		defaults:
			out << "Uninitialized fType property\t";
			break;
	}
	out << "To Processor " << fProc;

	out.flush();


	
}
void OOPDMOwnerTask::LogMeReceived(ostream & out){
	out << fProc;
	out << "\tId "<< fObjId;
	out << "\tReceiving ";
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
	out << "OwnerType ";	
	
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
		defaults:
			out << "Uninitialized fType property\t";
			break;
	}
	out << "From Processor " << fProcOrigin;
	out.flush();
}
int OOPDMOwnerTask::Pack (OOPSendStorage * buf)
{
	DataLog << "Packing Owner task for Obj " << fObjId << " message type " <<
		fType << " with objptr " << (fObjPtr != 0) << " version " << fVersion <<
		endl;
	OOPDaemonTask::Pack (buf);
	char type = fType;
	buf->PkByte (&type);
	int access = fState;
	buf->PkInt (&access);
	fVersion.Pack (buf);	// buf->PkLong(&fVersion);
	if (fObjPtr) {
		fObjPtr->Pack (buf);
	}
	else {
		long zero = 0;
		buf->PkLong (&zero);
	}
	buf->PkInt (&fTrace);
	buf->PkInt (&fProcOrigin);
	fObjId.Pack (buf);	// buf->PkLong(&fObjId);
	return 1;
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
int OOPDMRequestTask::Unpack (OOPReceiveStorage * buf)
{
	OOPDaemonTask::Unpack (buf);
	buf->UpkInt (&fProcOrigin);
	fDepend.Unpack (buf);
	return 1;
}
OOPSaveable *OOPDMRequestTask::Restore (OOPReceiveStorage * buf)
{
	OOPDMRequestTask *t = new OOPDMRequestTask ();
	t->Unpack (buf);
	return t;
}
int OOPDMRequestTask::Pack (OOPSendStorage * buf)
{
	OOPDaemonTask::Pack (buf);
	buf->PkInt (&fProcOrigin);
	fDepend.Pack (buf);
	return 1;
}

void OOPDMRequestTask::LogMe(ostream & out){
	out << fProc << "\t";
	out << "Id " << fDepend.Id();
	out << "\tRequesting ";
	switch (fDepend.State())
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
		defaults:
			out << "Uninitialized fNeed attribute\t";
			break;
	}

	out << "Version " << fDepend.Version();
	out << "\tTo processor " << fProc << "\n";
	out.flush();
}

