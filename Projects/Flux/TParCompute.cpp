#include "TParCompute.h"
#include "TLocalCompute.h"
#include "oopdatamanager.h"
#include "pzsave.h"

#include "ooptaskmanager.h"

class TPZStream;
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
		int processor = GetProcID();
		ltask->AddDependentData (OOPAccessTag
								 (fPartRelationId, EReadAccess,
								  fPartRelationVersion,processor));
		ltask->AddDependentData (OOPAccessTag
								 (fMeshIds[i], EReadAccess,
								  fMeshVersions,processor));
		ltask->AddDependentData (OOPAccessTag
								 (fStateIds[i], EWriteAccess,
								  fDataVersions,processor));
		ltask->AddDependentData (OOPAccessTag
								 (fRhsIds[i], EWriteAccess,
								  fDataVersions,processor));
		ltask->SetRhsIds (fRhsIds, fDataVersions);
		ltask->SetRecurrence (true);
		TM()->Submit(ltask);
		//		ltask->Submit ();
		ltask->PrintLog(TaskLog,"Submitted local task");
	}
	PrintLog(TaskLog,"TParCompute::Execute\n");
	TaskLog << "TParCompute::Execute";
	TaskLog << "Submiting TLocalCompute tasks\n";
	TaskLog << "fPartVersion " <<	fPartRelationVersion << endl;
	TaskLog << "Mesh Version "<<	fMeshVersions << endl;
	TaskLog << "State and Rhs Version " << fDataVersions << endl;
#ifdef VERBOSE
	cout << "TParCompute::Execute\n";
	cout << "Submiting TLocalCompute tasks\n";
	cout << "fPartVersion " <<	fPartRelationVersion << endl;
	cout << "Mesh Version "<<	fMeshVersions << endl;
	cout << "State and Rhs Version " << fDataVersions << endl;
	cout.flush ();
#endif
	return ESuccess;
}
void TParCompute::SetPartitionRelationId (OOPObjectId & Id,
										  OOPDataVersion & version)
{
	fPartRelationId = Id;
	fPartRelationVersion = version;
}
TParCompute::TParCompute ():OOPTask(),fNPartitions(0){}
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
 * @param buf A pointer to TSendStorage class to be packed.
 */
void TParCompute::Write (TPZStream & buf, int withclassid)
{
	PrintLog(TaskLog, "Packing TParCompute object");
	OOPTask::Write(buf,withclassid);
	int i;
	int sz = fRhsIds.size();
	buf.Write(&sz);
	for(i=0; i<sz; i++) fRhsIds[i].Write(buf);
	sz = fStateIds.size();
	buf.Write(&sz);
	for(i=0; i<sz; i++) fStateIds[i].Write(buf);
	sz = fMeshIds.size();
	buf.Write(&sz);
	for(i=0; i<sz; i++) fMeshIds[i].Write(buf);
	fPartRelationId.Write(buf);
	fPartRelationVersion.Write(buf);
	sz = fTaskIds.size();
	buf.Write(&sz);
	for(i=0; i<sz; i++) fTaskIds[i].Write(buf);
	fDataVersions.Write(buf);
	//fDataVersions.Print(cout);
	fMeshVersions.Write(buf);
	//fMeshVersions.Print(cout);
	buf.Write(&fNPartitions);
}
/**
 * Unpacks the object class_id
 * @param buf A pointer to TSendStorage class to be unpacked.
 */
void TParCompute::Read (TPZStream & buf, void * context)
{
	PrintLog(TaskLog, "Unpacking TParCompute object");
	OOPTask::Read (buf,context);
	int i;
	int sz;
	buf.Read(&sz);
	fRhsIds.resize(sz);
	for(i=0; i<sz; i++) fRhsIds[i].Read(buf);
	buf.Read(&sz);
	fStateIds.resize(sz);
	for(i=0; i<sz; i++) fStateIds[i].Read(buf);
	buf.Read(&sz);
	fMeshIds.resize(sz);
	for(i=0; i<sz; i++) fMeshIds[i].Read(buf);
	fPartRelationId.Read(buf);
	fPartRelationVersion.Read(buf);
	buf.Read(&sz);
	fTaskIds.resize(sz);
	for(i=0; i<sz; i++) fTaskIds[i].Read(buf);
	fDataVersions.Read(buf);
#ifdef VERBOSE
	fDataVersions.Print(cout);
#endif
	fMeshVersions.Read(buf);
#ifdef VERBOSE
	fMeshVersions.Print(cout);
#endif
	buf.Read(&fNPartitions);
}

TPZSaveable *TParCompute::Restore (TPZStream & buf, void * context)
{
	TParCompute *par = new TParCompute;// (0,0);
	par->Read (buf);
	return par;
}

/**
 * Within the Execute Method we should be able to get the data pointer
 */
void TParCompute::InitializePartitionRelationPointer () {
	fPartRelationPtr = dynamic_cast<TPartitionRelation *> (GetDepObjPtr(0));
}

template class TPZRestoreClass<TParCompute, TPARCOMPUTE_ID>;
