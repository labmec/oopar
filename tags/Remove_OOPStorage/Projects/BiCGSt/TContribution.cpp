/* Generated by Together */
#include "TContribution.h"
#include <stdlib.h>
bool TContribution::operator == (const TContribution & contr)
{
	if (!(fDestinationMesh == contr.fDestinationMesh))
		return false;
	if (fFrom.size () != contr.fFrom.size ())
		return false;
	if (fLocalIndices.size () != contr.fLocalIndices.size ())
		return false;
	if (fNContributions != contr.fNContributions)
		return false;
	if (fTo.size () != contr.fTo.size ())
		return false;
	return true;
}
TContribution & TContribution::operator= (const TContribution & contr)
{
	fDestinationMesh = contr.fDestinationMesh;
	fFrom = contr.fFrom;
	fLocalIndices = contr.fLocalIndices;
	fNContributions = contr.fNContributions;
	fTo = contr.fTo;
	return *this;
}
TContribution::TContribution ()
{
	fNContributions = 0;
}
int TContribution::IsEmpty ()
{
	return fNContributions == 0;
}
vector < int >&TContribution::DestinationVector (int index)
{
	if (index < (int) fLocalIndices.size ()) {
		return fLocalIndices[index];
	}
	else {
		cout << "TContribution::DestinationVector index out of range index " << index << endl;
		return fLocalIndices[0];
	}
}
void TContribution::InitializeRandom ()
{
	int rs = rand ();
	fNContributions = 1+(int) (5*rand()/(RAND_MAX+1.0));//rs % 5;
	fLocalIndices.resize (fNContributions);
	fFrom.resize(fNContributions);
	fTo.resize(fNContributions);
	int ic;
	for (ic = 0; ic < fNContributions; ic++) {
		fFrom[ic] = 1+(int) (7*rand()/(RAND_MAX+1));//rand()%7;
		fTo[ic] = 1+(int) (6*rand()/(RAND_MAX+1));//rand()%6;
		fLocalIndices[ic].resize (1+(int) (20*rand()/(RAND_MAX+1)));//rand () % 10);
		int nl = fLocalIndices[ic].size ();
		int ind;
		for (ind = 0; ind < nl; ind++) {
			fLocalIndices[ic][ind] = ind + 10;
		}
	}
}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
int TContribution::Pack (OOPSendStorage * buf){
	fDestinationMesh.Pack(buf);
	int i,sz;
	sz = fFrom.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) buf->PkInt(&fFrom[i]);
	sz = fTo.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) buf->PkInt(&fTo[i]);
	sz = fLocalIndices.size();
	buf->PkInt(&sz);
	for(i=0; i<sz; i++) {
		int lsz = fLocalIndices[i].size();
		int il;
		buf->PkInt(&lsz);
		for(il=0; il<lsz; il++) {
			buf->PkInt(&fLocalIndices[i][il]);
		}
	}
	buf->PkInt(&fNContributions);
	return 0;
}
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
int TContribution::Unpack (OOPReceiveStorage * buf){
	fDestinationMesh.Unpack(buf);
	int i,sz;
	buf->UpkInt(&sz);
	fFrom.resize(sz);
	for(i=0; i<sz; i++) buf->UpkInt(&fFrom[i]);
	buf->UpkInt(&sz);
	fTo.resize(sz);
	for(i=0; i<sz; i++) buf->UpkInt(&fTo[i]);
	buf->UpkInt(&sz);
	fLocalIndices.resize(sz);
	for(i=0; i<sz; i++) {
		int lsz,il;
		buf->UpkInt(&lsz);
		fLocalIndices[i].resize(lsz);
		for(il=0; il<lsz; il++) {
			buf->UpkInt(&fLocalIndices[i][il]);
		}
	}
	buf->UpkInt(&fNContributions);
	return 0;
}
