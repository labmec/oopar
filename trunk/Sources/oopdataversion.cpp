/* Generated by Together */
#include <iostream>
#include <typeinfo>
#include "oopdataversion.h"
class OOPSendStorage;
class OOPReceiveStorage;
class OOPDataVersion;
using namespace std;

int OOPDataVersion::Pack (OOPSendStorage * buf)
{
	int aux = fVersion.size ();
	buf->PkInt (&aux);
	vector < int >::iterator ivl;
	for (ivl = fVersion.begin (); ivl != fVersion.end (); ivl++)
		buf->PkInt (&*ivl);

	aux = fLevelCardinality.size ();
	buf->PkInt (&aux);
	for (ivl = fLevelCardinality.begin ();
	     ivl != fLevelCardinality.end (); ivl++)
		buf->PkInt (&*ivl);
	
	return 1;
}

int OOPDataVersion::Unpack (OOPReceiveStorage * buf)
{
	int aux = 0, size = 0;
	buf->UpkInt (&size);
	fVersion.resize (0);
	int i;
	for (i = 0; i < size; i++)
	{
		buf->UpkInt (&aux);
		fVersion.push_back (aux);
	}

	buf->UpkInt (&size);
	fLevelCardinality.resize (0);
	for (i = 0; i < size; i++)
	{
		buf->UpkInt (&aux);
		fLevelCardinality.push_back (aux);
	}

	// vector<int>::iterator ivc = aux_vc.begin();
	/* buf->UpkInt(&aux_vc[0],aux); i->fVersion.SetData(aux_vc,aux_vl); */
	return 1;
}

void OOPDataVersion::SetData (vector < int >&card,
			    vector < int >&version)
{
	fVersion = version;
	fLevelCardinality = card;
}

void OOPDataVersion::main ()
{
	int i = 0;
	OOPDataVersion version (4);
	version.Print ();
	version.IncrementLevel (8);
	version.Print ();
	version.IncrementLevel (9);
	version.Print ();

	cin >> i;
	OOPDataVersion v (version);

	v.DecreaseLevel ();
	v.Print ();
	v.IncrementLevel (17);
	// v.IncrementLevel(3);

	if (v.AmICompatible (version))
	{
		cout << " Versions compatible " << endl;
	}
	else
	{
		cout << "Versions are not compatible" << endl;
	}

	if (version.AmICompatible (v))
	{
		cout << " Versions compatible " << endl;
	}
	else
	{
		cout << "Versions are not compatible" << endl;
	}
	while (version.GetNLevels () != 1)
	{
		version.Increment ();
		version.Print ();
	}
	int j = 0;
	for (j = 0; j < version.GetLevelCardinality (0); j++)
	{
		version.Print ();
		++version;
	}

	version.IncrementLevel (20);
	version.Print ();

	OOPDataVersion vers2;
	vers2 = version;
	OOPDataVersion vers3 (vers2);
	vers2.Print ();
}


void OOPDataVersion::SetLevelVersion (int level,
				   int version)
{
	if (!(level < (int)fVersion.size ()))
		exit (-1);
	fVersion[level] = version;
}
void OOPDataVersion::SetLevelCardinality (int level,
					int depth)
{
	if (!(level < (int)fLevelCardinality.size ()))
		exit (-1);
	fLevelCardinality[level] = depth;
}

bool OOPDataVersion::AmICompatible (const OOPDataVersion & version) const
{
	// checks for level compatibility
	// assumes versions must have same depth
	// Is this true ?
	// if(fLevelCardinality[fLevelCardinality.size()-1] == -1) return
	// true;
	// if(version.GetLevelCardinality(fLevelCardinality.size()-1) == -1)
	// return true;
  //	if (GetNLevels () != version.GetNLevels ())
  //	{
  //		cerr << "Inconsistent version comparisson\n";
  //		cerr << "Different number of levels on OOPDataVersion objects\n";
  //		cerr << "File:" << __FILE__ << " Line:" << __LINE__ << endl;
		//return false;
  //	}
	unsigned int i = 0;
	for (i = 0; i < fVersion.size (); i++)
	{
		// Returns false if my version is older than the one
		// requested.
		// cout << "Local Version " << GetLevelVersion (i) << endl;
		// cout << "Checked Version " << version.GetLevelVersion (i) << endl;
		if (GetLevelVersion (i) != -1){
			if (GetLevelVersion (i) < version.GetLevelVersion (i)){
			  cout << "OOPDataVersion::AmICompatible returned false\n";
			  cout << "My version ";
			  Print(cout);
			  cout << "Other version ";
			  version.Print(cout);
				return false;
			}
		} else {
		  break;
		}
		// Returns false if in any common level, the cardinality is
		// not the same
		// if(GetLevelCardinality(i)!=version.GetLevelCardinality(i)) 
		// return false;
	}

	return true;
}


const char *OOPDataVersion::MyName ()
{
	return typeid (this).name ();
}


void OOPDataVersion::operator ++ ()
{
	this->Increment ();
}

bool OOPDataVersion::operator == (const OOPDataVersion & version) const
{
	if (GetNLevels () != version.GetNLevels ())
		return false;
	unsigned int i = 0;
	for (i = 0; i < fVersion.size (); i++)
	{
		// Returns false if my version is older than the one
		// requested.
		if (GetLevelVersion (i) != version.GetLevelVersion (i))
			return false;
		// Returns false if in any common level, the cardinality is
		// not the same
		if (GetLevelCardinality (i) !=
		    version.GetLevelCardinality (i))
			return false;
	}

	return true;

}

bool OOPDataVersion::operator < (const OOPDataVersion & version) 
{
	if (GetNLevels () != version.GetNLevels ())
		return false;
	unsigned int i = 0;
	for (i = 0; i < fVersion.size (); i++)
	{
		// Returns false if my version is older than the one
		// requested.
		if (GetLevelVersion (i) > version.GetLevelVersion (i))
			return false;
		// Returns false if in any common level, the cardinality is
		// not the same
		if (GetLevelCardinality (i) !=
		    version.GetLevelCardinality (i))
			return false;
	}

	return true;

}

bool OOPDataVersion::operator > (const OOPDataVersion & version) 
{
	if (GetNLevels () != version.GetNLevels ())
		return false;
	unsigned int i = 0;
	for (i = 0; i < fVersion.size (); i++)
	{
		// Returns false if my version is older than the one
		// requested.
		if (GetLevelVersion (i) < version.GetLevelVersion (i))
			return false;
		// Returns false if in any common level, the cardinality is
		// not the same
		if (GetLevelCardinality (i) !=
		    version.GetLevelCardinality (i))
			return false;
	}

	return true;

}

void OOPDataVersion::Print (ostream & out) const
{
	out << "Number of levels " << GetNLevels () << endl;
	int i = 0;
	for (i = 0; i < GetNLevels (); i++)
	{
		out << "Level:" << i << "\t Cardinality:" <<
			GetLevelCardinality (i) << "\t Version:" <<
			GetLevelVersion (i) << endl;
		out.flush ();
	}
}

OOPDataVersion & OOPDataVersion::operator= (const OOPDataVersion & version)
{
	fVersion = version.GetLevelVersion ();
	fLevelCardinality = version.GetLevelCardinality ();
	return *this;
}

OOPDataVersion::OOPDataVersion (const ::OOPDataVersion & version)
{
	fLevelCardinality = version.GetLevelCardinality ();
	fVersion = version.GetLevelVersion ();
}

void OOPDataVersion::IncrementLevel (int cardinality)
{
	fVersion.push_back (0);
	fLevelCardinality.push_back (cardinality);
}


void OOPDataVersion::DecreaseLevel ()
{
	fVersion.pop_back ();
	fLevelCardinality.pop_back ();
}

void OOPDataVersion::Increment ()
{
	if (!fVersion.size ())
	{
		cerr << "Algo errado" << endl;
		cerr << "fVersion.size() = 0" << endl;
		return;
	}
	fVersion[fVersion.size () - 1]++;

	// Checks whether fVersion grows indefinetely 

#ifdef DEBUG
	//	Print ();
#endif
	if (fLevelCardinality[fVersion.size () - 1] == -1)
		return;

	if (fVersion[fVersion.size () - 1] >
	    fLevelCardinality[fVersion.size () - 1])
	{
		cerr << "Inconsistent data version incrementation" << __FILE__
			<< __LINE__ << endl;
		return;
	}
	if (fVersion[fVersion.size () - 1] ==
	    fLevelCardinality[fVersion.size () - 1])
	{
		if (fVersion.size () > 1)
		{
			fVersion.pop_back ();
			fLevelCardinality.pop_back ();
			Increment ();
		}

	}
}

vector < int >OOPDataVersion::GetLevelCardinality () const
{
	return fLevelCardinality;
}
vector < int >OOPDataVersion::GetLevelVersion () const
{
	return fVersion;
}

int OOPDataVersion::GetLevelCardinality (int level) const
{
	if (!(level < (int)fVersion.size ()))
	{
		cerr << "FILE: " << __FILE__ << " LINE:" << __LINE__
			<< " Accessing level out of range" << endl;
		cerr << "Maximum:" << GetNLevels () -
			1 << " Trying:" << level << endl;
		//exit(-1);
		return -1;
	}

	return fLevelCardinality[level];
}

int OOPDataVersion::GetLevelVersion (int level) const
{
	if (!(level < (int)fVersion.size ()))
	{
		cerr << "FILE: " << __FILE__ << " LINE:" << __LINE__
			<< " Accessing level out of range" << endl;
		cerr << "Maximum:" << GetNLevels () -
			1 << " Trying:" << level << endl;
		//exit (-1);
		return -1;
	}

	return fVersion[level];
}

int OOPDataVersion::GetNLevels () const
{
	return fVersion.size ();
}

OOPDataVersion::~OOPDataVersion ()
{
	// Some necessary checks, still to be defined

}

OOPDataVersion::OOPDataVersion (int cardinality)
{
	fVersion.push_back (0);
	fLevelCardinality.push_back (cardinality);
}

OOPDataVersion::OOPDataVersion ()
{
	fVersion.resize (0);
	fVersion.push_back (0);
	fLevelCardinality.resize (0);
	fLevelCardinality.push_back (-1);

}
