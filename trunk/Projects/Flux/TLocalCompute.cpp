#include "TLocalCompute.h"
#include "TContribution.h"
#include "TPartitionRelation.h"
#include "TTaskComm.h"
#include "oopmetadata.h"
class OOPStorageBuffer;
void TLocalCompute::InitializePartitionRelationPointer ()
{
	OOPMetaData *objptr = fDataDepend.Dep (0).ObjPtr ();
	fPartRelationPtr =
		dynamic_cast < TPartitionRelation * >(objptr->Ptr ());
}
void TLocalCompute::ComputeLocalFluxes ()
{
	OOPMetaData *ptr;
//	ptr = fDataDepend.Dep (2).ObjPtr ();
	OOPDataVersion ver;
//	ver = ptr->Version();
//	ver.Increment();
//	ptr->SetVersion(ver,Id());
	
	ptr = fDataDepend.Dep (3).ObjPtr ();
	PrintLog(TaskLog, "TLocalCompute contributes to object id ");
	ptr->Id ().ShortPrint (TaskLog);
	TaskLog << endl;
#ifdef VERBOSE		
	cout << "TLocalCompute contributes to object id " << ptr->Id();
#endif
//	ptr->Id ().Print (cout);
	ver = ptr->Version ();
	// int nlevel = ver.GetNLevels();
	int ncontr = fPartRelationPtr->IncomingContribution (fPartition);
	ver.IncrementLevel (ncontr);
	TaskLog << "After increment level ";
	ver.Print(TaskLog);
	
#ifdef VERBOSE		
	cout << "After increment level ";
	ver.Print (cout);
#endif
	++ver;
	
	TaskLog << "After increment ";
	TaskLog << "TLocalCompute number of contributions " << ncontr <<
		" new version " << endl;
	ver.Print (TaskLog);
#ifdef VERBOSE		
	cout << "After increment ";
	cout << "TLocalCompute number of contributions " << ncontr <<
		" new version " << ver << endl;
#endif
//	ver.Print (cout);
	ptr->SetVersion (ver, Id ());
}
void TLocalCompute::SetRhsIds (vector < OOPObjectId > &rhsids,
			       OOPDataVersion & rhsversion)
{
	fRhsIds = rhsids;
	fRhsVersion = rhsversion;
}
void TLocalCompute::TransmitFLuxes ()
{
	int npartitions = fPartRelationPtr->GetNPartitions ();
	int i = 0;
	OOPMetaData *ptr = fDataDepend.Dep (3).ObjPtr ();
	OOPDataVersion rhsver = ptr->Version ();
	rhsver.IncrementLevel (-1);
	int nver = rhsver.GetNLevels ();
	rhsver.SetLevelVersion (nver - 1, -1);
	for (i = 0; i < npartitions; i++) {
		if (i == fPartition)
			continue;
		TContribution *cont =
			&fPartRelationPtr->GetRelation (fPartition, i);
		if (cont->IsEmpty ())
			continue;
		int procid = fPartRelationPtr->Processor(i);
		TTaskComm *task = new TTaskComm (procid);
		OOPMDataDepend depend (fRhsIds[i], EWriteAccess, rhsver);
		PrintLog(TaskLog,"TLocalCompute::TransmitFluxes targets ");
		fRhsIds[i].ShortPrint (TaskLog);
		TaskLog << " and depends on version " << rhsver <<  endl;
#ifdef VERBOSE
		cout << "TLocalCompute::TransmitFluxes targets ";
		fRhsIds[i].Print (cout);
		cout << " and depends on version " << endl;
		rhsver.Print (cout);
#endif
		//		rhsver.Print (TaskLog);
		task->AddDependentData (depend);
		task->Submit ();
		TaskLog << "Id of TTaskComm is " << task->Id() << endl;
	}
}
void TLocalCompute::ComputeFrontierFluxes ()
{
#ifndef WIN32
#warning "ComputeFrontierFluxes is empty"
#endif
	PrintLog(TaskLog,"Nothing Implemented in ComputeFrontierFluxes\n");
#ifdef VERBOSE
	cout << "Nothing Implemented in ComputeFrontierFluxes\n";
	cout.flush ();
#endif
}
OOPMReturnType TLocalCompute::Execute ()
{
#ifdef VERBOSE
	cout << "Executing TLocalCompute task\n";
#endif
	PrintLog(TaskLog,"Executing TLocalCompute task\n");
	this->InitializePartitionRelationPointer ();
	// message #1.1 to this:TLocalCompute
	this->ComputeFrontierFluxes ();
	this->TransmitFLuxes ();
	// message #1.3 to this:TLocalCompute
	this->ComputeLocalFluxes ();
	return ESuccess;
}
TLocalCompute::TLocalCompute (int ProcId, int partition):OOPTask (ProcId),
fPartition (partition)
{
}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
int TLocalCompute::Pack (OOPStorageBuffer * buf){
	OOPTask::Pack (buf);
	buf->PkInt(&fPartition);
	int i,sz = fRhsIds.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) fRhsIds[i].Pack(buf);
	fRhsVersion.Pack(buf);
	return 0;
}
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
int TLocalCompute::Unpack (OOPStorageBuffer * buf){
	OOPTask::Unpack(buf);
	buf->UpkInt(&fPartition);
	int i,sz;
	buf->UpkInt(&sz);
	fRhsIds.resize(sz);
	for(i=0; i<sz; i++) fRhsIds[i].Unpack(buf);
	fRhsVersion.Unpack(buf);
	return 0;
}
OOPSaveable *TLocalCompute::Restore (OOPStorageBuffer * buf) {
	TLocalCompute *loc = new TLocalCompute(0,0);
	loc->Unpack(buf);
	return loc;
}
