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
	fNContributions = rs % 5;
	fLocalIndices.resize (fNContributions);
	int ic;
	for (ic = 0; ic < fNContributions; ic++) {
		fLocalIndices[ic].resize (rand () % 20);
		int nl = fLocalIndices[ic].size ();
		int ind;
		for (ind = 0; ind < nl; ind++) {
			fLocalIndices[ic][ind] = ind + 10;
		}
	}
}
