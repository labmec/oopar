/* Generated by Together */
#include "TPartitionRelation.h"
#include "oopcommmanager.h"
class TPZStream;
//#include "pzvec.h"
TPartitionRelation::TPartitionRelation ():TPZSaveable (), fNumPartitions(0)
{
}
int TPartitionRelation::OutgoingContribution (int partition)
{
	int counter = 1;
	int i;
	for (i = 0; i < fNumPartitions; i++) {
		if (i == partition)
			continue;
		// Ainda precisa ser implementado com a funcionalidade
		// esperada
		if (fRelation[partition][i].NContributions ()) {
			counter++;
		}
	}
	return counter;
}
int TPartitionRelation::IncomingContribution (int partition)
{
	int i;
	int count = 1;
	for (i = 0; i < fNumPartitions; i++) {
		if (i == partition)
			continue;
		// Ainda precisa ser implementado com a funcionalidade
		// esperada
		if (fRelation[i][partition].NContributions ()) {
			count++;
		}
	}
	return count;
}
int TPartitionRelation::GetNPartitions ()
{
	return fNumPartitions;
}

TPartitionRelation::TPartitionRelation (int npart)
{
	fRelation.resize (npart);
	fProcessor.resize(npart);
	int     i;
	for (i = 0; i < npart; i++) {
		fProcessor[i] = i%CM->NumProcessors();
		fRelation[i].resize (npart);
	}
	fNumPartitions = npart;
}
TPartitionRelation *TPartitionRelation::CreateRandom (int npart)
{
	TPartitionRelation *part = new TPartitionRelation (npart);
	// part->fRelation.resize(npart);
	int i, j;
	TContribution cont;
	vector < TContribution > vec;
	vec.resize (npart);
	for (i = 0; i < npart; i++)
		part->fRelation[i] =  (vec);
	for (i = 0; i < npart; i++) {
		for (j = 0; j < npart; j++) {
			part->fRelation[i].resize (npart);
		}
	}
	for (i = 0; i < npart; i++) {
		for (j = 0; j < npart; j++) {
			part->fRelation[i][j].InitializeRandom ();
		}
	}
	return part;
}
void TPartitionRelation::Print (ostream & out)
{
	int i;
	for (i = 0; i < fNumPartitions; i++) {
		out << "Partition " << i << " contributes to " <<
			OutgoingContribution (i) << " partitions\n";
		out << "Partition " << i << " receives from " <<
			IncomingContribution (i) << " partitions\n";
	}
}
void TPartitionRelation::SetMeshId (int index, OOPObjectId & meshid)
{
	int ip;
	for (ip = 0; ip < fNumPartitions; ip++) {
		fRelation[ip][index].SetMeshId (meshid);
	}
}
TContribution & TPartitionRelation::GetRelation (int parfrom, int parto)
{
	return fRelation[parfrom][parto];
}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
void TPartitionRelation::Write (TPZStream & buf, int withclassid) {
	TPZSaveable::Write(buf);
	buf.Write(&fNumPartitions);
	int i,sz = fRelation.size();
	for(i=0; i<fNumPartitions;i++) {
		int iproc = fProcessor[i];
		buf.Write(&iproc);
	}
	buf.Write(&sz);
	for(i=0; i<sz; i++) {
		int il,lsz = fRelation[i].size();
		buf.Write(&lsz);
		for(il=0; il<lsz; il++) fRelation[i][il].Write(buf);
	}
}
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
void TPartitionRelation::Read (TPZStream & buf, void * context) {
	TPZSaveable::Read(buf);
	buf.Read(&fNumPartitions);
	int i,sz;
	fProcessor.resize(fNumPartitions);
	for(i=0; i<fNumPartitions; i++) {
		int iproc;
		buf.Read(&iproc);
		fProcessor[i] = iproc;
	}
	buf.Read(&sz);
	fRelation.resize(sz);
	for(i=0; i<sz; i++) {
		int il,lsz;
		buf.Read(&lsz);
		fRelation[i].resize(lsz);
		for(il=0; il<lsz; il++) fRelation[i][il].Read(buf);
	}
}
TPZSaveable *TPartitionRelation::Restore (TPZStream & buf, void * context) {
	TPartitionRelation *par = new TPartitionRelation;//(0);
	par->Read(buf);
	return par;
}