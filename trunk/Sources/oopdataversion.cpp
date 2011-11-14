
#include <iostream>
#include <typeinfo>
#include "oopdataversion.h"
//class   OOPStorageBuffer;
//class   OOPStorageBuffer;
class   OOPDataVersion;
using namespace std;

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPDataVersion"));
#endif


void OOPDataVersion::Write (TPZStream & buf, int)
{
	int aux = fVersion.size ();
	buf.Write (&aux);
	vector < int >::iterator ivl;
	for (ivl = fVersion.begin (); ivl != fVersion.end (); ivl++)
		buf.Write (&*ivl);
	aux = fLevelCardinality.size ();
	buf.Write (&aux);
	for (ivl = fLevelCardinality.begin ();ivl != fLevelCardinality.end (); ivl++)
		buf.Write (&*ivl);
}
void OOPDataVersion::Read (TPZStream & buf, void * context)
{
	int aux = 0, size = 0;
	buf.Read (&size);
	fVersion.resize (0);
	int i;
	for (i = 0; i < size; i++) {
		buf.Read (&aux);
		fVersion.push_back (aux);
	}
	buf.Read (&size);
	fLevelCardinality.resize (0);
	for (i = 0; i < size; i++) {
		buf.Read (&aux);
		fLevelCardinality.push_back (aux);
	}
	// vector<int>::iterator ivc = aux_vc.begin();
	/* buf->UpkInt(&aux_vc[0],aux); i->fVersion.SetData(aux_vc,aux_vl); */
}
void OOPDataVersion::SetData (vector < int >&card, vector < int >&version)
{
	fVersion = version;
	fLevelCardinality = card;
}
void OOPDataVersion::GetData (vector < int >&card, vector < int >&version)
{
	version = fVersion;
	card = fLevelCardinality;
}

void OOPDataVersion::main ()
{
	/*	int i = 0;
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
	 if (v.AmICompatible (version)) {
	 #ifdef LOG4CXX
	 stringstream sout;
	 sout << " Versions compatible ";
	 LOGPZ_DEBUG (logger,sout.str());
	 #endif
	 }
	 else {
	 #ifdef LOG4CXX
	 stringstream sout;
	 sout << "Versions are not compatible";
	 LOGPZ_DEBUG (logger,sout.str());
	 #endif
	 }
	 if (version.AmICompatible (v)) {
	 #ifdef LOG4CXX
	 stringstream sout;
	 sout << " Versions compatible ";
	 LOGPZ_DEBUG (logger,sout.str());
	 #endif
	 }
	 else {
	 #ifdef LOG4CXX
	 stringstream sout;
	 sout << "Versions are not compatible";
	 LOGPZ_DEBUG (logger,sout.str());
	 #endif
	 }
	 while (version.GetNLevels () != 1) {
	 version.Increment ();
	 version.Print ();
	 }
	 int j = 0;
	 for (j = 0; j < version.GetLevelCardinality (0); j++) {
	 version.Print ();
	 ++version;
	 }
	 version.IncrementLevel (20);
	 version.Print ();
	 OOPDataVersion vers2;
	 vers2 = version;
	 OOPDataVersion vers3 (vers2);
	 vers2.Print ();*/
}
void OOPDataVersion::SetLevelVersion (int level, int version)
{
	if (!(level < (int) fVersion.size ()))
		exit (-1);
	fVersion[level] = version;
}
void OOPDataVersion::SetLevelCardinality (int level, int depth)
{
	if (!(level < (int) fLevelCardinality.size ()))
		exit (-1);
	fLevelCardinality[level] = depth;
}
bool OOPDataVersion::AmICompatible (const OOPDataVersion & dataversion) const
{
	unsigned int i = 0;
	for (i = 0; i < fVersion.size (); i++)
	{
		if (GetLevelVersion (i) != -1)
		{
			if(i>=dataversion.fVersion.size()) return true;
			if (GetLevelVersion (i) < dataversion.GetLevelVersion (i))
			{
#ifdef LOG4CXX_PARANOID
				stringstream sout;
				LOGPZ_WARN(logger, "AmICompatible returned false");
				sout << "My version ";
				Print (sout);
				sout << "Other version ";
				version.Print (sout);
				LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
				return false;
			}
		}
		else
		{
			break;
		}
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
void OOPDataVersion::operator ++ (int)
{
	this->Increment ();
}

bool OOPDataVersion::operator == (const OOPDataVersion & version) const
{
	if (GetNLevels () != version.GetNLevels ())
	{
#ifdef LOG4CXX_PARANOID
		stringstream sout;
		sout << "Returning false for different number of levels";
		LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
		return false;
	}
	unsigned int i = 0;
	for (i = 0; i < fVersion.size (); i++)
	{
		// Returns false if my version is older than the one
		// requested.
		if (GetLevelVersion (i) != version.GetLevelVersion (i))
		{
#ifdef LOG4CXX_PARANOID
			stringstream sout;
			sout << "Returning false for different version on level " << i;
			LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
			return false;
		}
		// Returns false if in any common level, the
		// cardinality is
		// not the same
		if (GetLevelCardinality (i) !=
			version.GetLevelCardinality (i))
		{
#ifdef LOG4CXX_PARANOID
			stringstream sout;
			sout << "Returning false for different level cardinality on level " << i;
			LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
			return false;
		}
	}
#ifdef LOG4CXX_PARANOID
	stringstream sout;
	sout << __PRETTY_FUNCTION__ << " Returning true";
	LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
	return true;
}
bool OOPDataVersion::operator < (const OOPDataVersion & version) const
{
	unsigned int minlevel = GetNLevels() < version.GetNLevels() ? GetNLevels() : version.GetNLevels();
	unsigned int i = 0;
	for (i = 0; i < minlevel; i++) {
		// Returns false if my version is older than the one
		// requested.
		if (GetLevelVersion (i) < version.GetLevelVersion (i)) return true;
		if (GetLevelVersion (i) != version.GetLevelVersion (i)) return false;
	}
	for(i = 0; i< minlevel; i++)
	{
		// Returns false if in any common level, the cardinality is
		// not the same
		if (GetLevelCardinality (i) < version.GetLevelCardinality (i)) return true;
		if (GetLevelCardinality (i) != version.GetLevelCardinality (i)) return false;
	}
	if (GetNLevels () < version.GetNLevels ()) return true;
	return false;
}
bool OOPDataVersion::operator >= (const OOPDataVersion & version)
{
	if(this->operator >(version)) return true;
	if(! this->operator<(version)) return true;
	return false;
}

bool OOPDataVersion::operator > (const OOPDataVersion & version)
{
	unsigned int minlevel = GetNLevels() < version.GetNLevels() ? GetNLevels() : version.GetNLevels();
	unsigned int i = 0;
	for (i = 0; i < minlevel; i++) {
		// Returns false if my version is older than the one
		// requested.
		if (GetLevelVersion (i) > version.GetLevelVersion (i)) return true;
		if (GetLevelVersion (i) != version.GetLevelVersion (i)) return false;
	}
	for(i = 0; i< minlevel; i++)
	{
		// Returns false if in any common level, the cardinality is
		// not the same
		if (GetLevelCardinality (i) > version.GetLevelCardinality (i)) return true;
		if (GetLevelCardinality (i) != version.GetLevelCardinality (i)) return false;
	}
	if (GetNLevels () > version.GetNLevels ()) return true;
	return false;
}
void OOPDataVersion::Print (std::ostream & out) const
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
std::ostream &OOPDataVersion::ShortPrint(std::ostream & out) const {
	int nl = GetNLevels(), i;
	for(i=0; i<nl; i++) out << GetLevelVersion(i) << '/' <<
		GetLevelCardinality(i) << ':';
	return out;
}
OOPDataVersion & OOPDataVersion::operator= (const OOPDataVersion & version)
{
	fVersion = version.fVersion;
	fLevelCardinality = version.fLevelCardinality;
	return *this;
}
OOPDataVersion::OOPDataVersion (const::OOPDataVersion & version)
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
	if (!fVersion.size ()) {
		LOGPZ_ERROR(logger, "Something wrong - fVersion.size() = 0");
		return;
	}
	fVersion[fVersion.size () - 1]++;
	// Checks whether fVersion grows indefinetely
	if (fLevelCardinality[fVersion.size () - 1] == -1)
		return;
	if (fVersion[fVersion.size () - 1] > fLevelCardinality[fVersion.size () - 1])
	{
#ifdef LOG4CXX_PARANOID
		stringstream sout;
		sout << "Inconsistent data version incrementation" <<
		__FILE__ << __LINE__;
		LOGPZ_ERROR(logger,sout.str().c_str());
#endif
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
vector< int > OOPDataVersion::GetLevelCardinality () const
{
	return fLevelCardinality;
}
vector< int > OOPDataVersion::GetLevelVersion () const
{
	return fVersion;
}
int OOPDataVersion::GetLevelCardinality (int level) const
{
	if (!(level < (int) fVersion.size ()))
	{
#ifdef LOG4CXX_PARANOID
		stringstream sout;
		sout << "FILE: " << __FILE__ << " LINE:" << __LINE__
		<< " Accessing level out of range" << endl;
		sout << "Maximum:" << GetNLevels () -
		1 << " Trying:" << level;
		LOGPZ_ERROR(logger,sout.str().c_str());
#endif
		// exit(-1);
		return -1;
	}
	return fLevelCardinality[level];
}
int OOPDataVersion::GetLevelVersion (int level) const
{
	if (!(level < (int) fVersion.size ()))
	{
#ifdef LOG4CXX_PARANOID
		stringstream sout;
		sout << "FILE: " << __FILE__ << " LINE:" << __LINE__ << " Accessing level out of range" << endl;
		sout << "Maximum:" << GetNLevels () - 1 << " Trying:" << level;
		LOGPZ_ERROR(logger,sout.str().c_str());
#endif
		// exit (-1);
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
bool OOPDataVersion::CanExecute (const OOPDataVersion & dataversion) const
{
	if (fVersion.size () != dataversion.fVersion.size ()) return false;
	bool can_I = true;
	int i;
	for (i = 0; i < dataversion.GetNLevels (); i++)
	{
		if (GetLevelVersion (i) != dataversion.GetLevelVersion (i) && GetLevelVersion (i) != -1)
		{
#ifdef LOG4CXX
			stringstream sout;
			sout << "GetLevelVersion( i): " << GetLevelVersion( i) << " DataVersion " << dataversion.GetLevelVersion (i);
			LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
			can_I = false;
		}
	}
	return can_I;
}
