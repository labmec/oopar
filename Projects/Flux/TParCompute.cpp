#include "TParCompute.h"
#include "TLocalCompute.h"
#include "oopdatamanager.h"
OOPMReturnType TParCompute::Execute ()
{
	// submit subtasks to the Task Manager
	this->InitializePartitionRelationPointer();
	int i;
	// increment the level of the state and rhs objects with appropriate
	// cardinality
	for (i = 0; i < fNPartitions; i++) {
		int procid = this->fPartRelationPtr->Processor(i);
		TLocalCompute *ltask = new TLocalCompute (procid, i);
		ltask->AddDependentData (OOPMDataDepend
					 (fPartRelationId, EReadAccess,
					  fPartRelationVersion));
		ltask->AddDependentData (OOPMDataDepend
					 (fMeshIds[i], EReadAccess,
					  fMeshVersions));
		ltask->AddDependentData (OOPMDataDepend
					 (fStateIds[i], EWriteAccess,
					  fDataVersions));
		ltask->AddDependentData (OOPMDataDepend
					 (fRhsIds[i], EWriteAccess,
					  fDataVersions));
		ltask->SetRhsIds (fRhsIds, fDataVersions);
		ltask->SetRecurrence (true);
		ltask->Submit ();
		ltask->PrintLog(TaskLog,"Submitted local task");
	}
	PrintLog(TaskLog,"TParCompute::Execute\n");
	cout << "TParCompute::Execute\n";
	TaskLog << "TParCompute::Execute";
	cout << "Submiting TLocalCompute tasks\n";
	TaskLog << "Submiting TLocalCompute tasks\n";
	cout << "fPartVersion " <<	fPartRelationVersion << endl;
	TaskLog << "fPartVersion " <<	fPartRelationVersion << endl;
	cout << "Mesh Version "<<	fMeshVersions << endl;
	TaskLog << "Mesh Version "<<	fMeshVersions << endl;
	cout << "State and Rhs Version " << fDataVersions << endl;
	TaskLog << "State and Rhs Version " << fDataVersions << endl;
	cout.flush ();
	return ESuccess;
}
void TParCompute::SetPartitionRelationId (OOPObjectId & Id,
					  OOPDataVersion & version)
{
	fPartRelationId = Id;
	fPartRelationVersion = version;
}
TParCompute::TParCompute (int procid, int numpartitions):OOPTask (procid),
fNPartitions (numpartitions)
{
}
void TParCompute::SetStateId (vector < OOPObjectId > &Id,
			      OOPDataVersion & stateversion)
{
	// Check if Id was already inserted
	fStateIds = Id;
	fDataVersions = stateversion;
}
void TParCompute::SetRhsId (vector < OOPObjectId > &Id,
			    OOPDataVersion & rhsversion)
{
	fRhsIds = Id;
	fDataVersions = rhsversion;
}
void TParCompute::SetMeshId (vector < OOPObjectId > &Id,
			     OOPDataVersion & meshversion)
{
	fMeshIds = Id;
	fMeshVersions = meshversion;
}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
int TParCompute::Pack (OOPSendStorage * buf)
{
	PrintLog(TaskLog, "Packing TParCompute object");
	OOPTask::Pack (buf);
	int i;
	int sz = fRhsIds.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) fRhsIds[i].Pack(buf);
	sz = fStateIds.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) fStateIds[i].Pack(buf);
	sz = fMeshIds.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) fMeshIds[i].Pack(buf);
	fPartRelationId.Pack(buf);
	fPartRelationVersion.Pack(buf);
	sz = fTaskIds.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) fTaskIds[i].Pack(buf);
	fDataVersions.Pack(buf);
	//fDataVersions.Print(cout);
	fMeshVersions.Pack(buf);
	//fMeshVersions.Print(cout);
	buf->PkInt(&fNPartitions);
	return 0;
}
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
int TParCompute::Unpack (OOPReceiveStorage * buf)
{
	PrintLog(TaskLog, "Unpacking TParCompute object");
	OOPTask::Unpack (buf);
	int i;
	int sz;
	buf->UpkInt(&sz);
	fRhsIds.resize(sz);
	for(i=0; i<sz; i++) fRhsIds[i].Unpack(buf);
	buf->UpkInt(&sz);
	fStateIds.resize(sz);
	for(i=0; i<sz; i++) fStateIds[i].Unpack(buf);
	buf->UpkInt(&sz);
	fMeshIds.resize(sz);
	for(i=0; i<sz; i++) fMeshIds[i].Unpack(buf);
	fPartRelationId.Unpack(buf);
	fPartRelationVersion.Unpack(buf);
	buf->UpkInt(&sz);
	fTaskIds.resize(sz);
	for(i=0; i<sz; i++) fTaskIds[i].Unpack(buf);
	fDataVersions.Unpack(buf);
	fDataVersions.Print(cout);
	fMeshVersions.Unpack(buf);
	fMeshVersions.Print(cout);
	buf->UpkInt(&fNPartitions);
	
	return 0;
}
OOPSaveable *TParCompute::Restore (OOPReceiveStorage * buf)
{
	TParCompute *par = new TParCompute (0,0);
	par->Unpack (buf);
	return par;
}
/**
 * Within the Execute Method we should be able to get the data pointer
 */
void TParCompute::InitializePartitionRelationPointer () {
	fPartRelationPtr = dynamic_cast<TPartitionRelation *> (this->Depend().Dep(0).ObjPtr()->Ptr());
}
