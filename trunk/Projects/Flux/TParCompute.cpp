#include "TParCompute.h"
#include "TLocalCompute.h"
#include "oopdatamanager.h"


OOPMReturnType TParCompute::Execute ()
{

	// submit subtasks to the Task Manager
	int i;
	// increment the level of the state and rhs objects with appropriate
	// cardinality
	for (i = 0; i < fNPartitions; i++) {
		TLocalCompute *ltask = new TLocalCompute (GetProcID (), i);
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
	}
	cout << "TParCompute::Execute\n";
	cout << "Submiting TLocalCompute tasks\n";
	cout << "fPartVersion ";
	fPartRelationVersion.Print (cout);
	cout << "Mesh Version ";
	fMeshVersions.Print (cout);
	cout << "State and Rhs Version ";
	fDataVersions.Print (cout);
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
	fMeshVersions.Pack(buf);
	buf->PkInt(&fNPartitions);
	return 0;
}

  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
int TParCompute::Unpack (OOPReceiveStorage * buf)
{
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
	fMeshVersions.Unpack(buf);
	buf->UpkInt(&fNPartitions);

	
	return 0;
}

OOPSaveable *TParCompute::Restore (OOPReceiveStorage * buf)
{
	TParCompute *par = new TParCompute (0,0);
	par->Unpack (buf);
	return par;
}
