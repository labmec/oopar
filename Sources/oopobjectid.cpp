/* Generated by Together */
#include <deque>
#include "oopobjectid.h"
#include <algorithm>
class   OOPStorageBuffer;
class   OOPStorageBuffer;
//class OOPObjectId;
 
OOPObjectId::OOPObjectId ()
{
	fProcId = 0;
	fId = 0;
}
OOPObjectId::OOPObjectId (long ProcId, long Id)
{
	fProcId = ProcId;
	fId = Id;
}
void OOPObjectId::Print (ostream & out) const
{
	out << "ObjectId: " << fId << endl;
	out << "ProcessorId: " << GetProcId () << endl;
}
ostream &OOPObjectId::ShortPrint (ostream & out) const
{
	out << fProcId << ':' << fId;
	return out;
}
int OOPObjectId::main ()
{
	deque < OOPObjectId * >TestDeQue;
	OOPObjectId id (20, 1);
	OOPObjectId idteste (id);
	TestDeQue.push_back (&id);
	OOPObjectId id2 (21, 2);
	TestDeQue.push_back (&id2);
	idteste.SetProcId (34);
	TestDeQue.push_back (&idteste);
	OOPObjectId *ptrid = new OOPObjectId;
	ptrid->SetId (30);
	ptrid->SetProcId (2);
	TestDeQue.push_back (ptrid);
	cout << TestDeQue.size () << endl;
	deque < OOPObjectId * >::iterator i;
	for (i = TestDeQue.begin (); i != TestDeQue.end (); i++) {
		(*i)->Print ();
	}
	for (i = TestDeQue.begin (); i != TestDeQue.end (); i++) {
		if ((*i)->operator== (idteste)) {
			TestDeQue.erase (i);
		}
	}
	cout << TestDeQue.size () << endl;
	for (i = TestDeQue.begin (); i != TestDeQue.end (); i++) {
		(*i)->Print ();
	}
	OOPObjectId *pt2 = new OOPObjectId;
	pt2->SetId (30);
	pt2->SetProcId (2);
	deque < OOPObjectId * >::iterator fd;
	fd = find (TestDeQue.begin (), TestDeQue.end (), pt2);
	
      i = TestDeQue.end ();
	delete *i;
	return 0;
}
void OOPObjectId::Zero ()
{
	fId = 0;
	fProcId = 0;
}
bool OOPObjectId::operator == (const OOPObjectId & obj) const
{
	if ((fId == obj.GetId ()) && (fProcId == obj.GetProcId ()))
		return true;
	return false;
}
bool OOPObjectId::operator >= (const OOPObjectId & obj) const
{
	if ((fId >= obj.GetId ()) && (fProcId >= obj.GetProcId ()))
		return true;
	return false;
}
bool OOPObjectId::operator >= (int val) const
{
	if (fId >= val)
		return true;
	return false;
}
bool OOPObjectId::operator < (int val) const
{
	if (fId < val)
		return true;
	return false;
}
bool OOPObjectId::operator < (const OOPObjectId & id) const
{
	if(fProcId < id.GetProcId()){
		return true;
	}
	if(fProcId == id.GetProcId()){
		if(fId < id.GetId()){
			return true;
		}
	}
	return false;
}
OOPObjectId & OOPObjectId::operator= (const OOPObjectId & obj)
{
	fId = obj.GetId ();
	fProcId = obj.GetProcId ();
	return *this;
}
/*OOPObjectId & OOPObjectId::operator=(int val){
	if (val >= 0)  exit(-1);
    fId=val;
    fProcId=val;
    return *this;
}*/
void OOPObjectId::SetData (int id, int procid)
{
	fId = id;
	fProcId = procid;
}
int OOPObjectId::GetProcId () const
{
	return fProcId;
}
OOPObjectId OOPObjectId::Id ()
{
	return *this;
}
int OOPObjectId::GetId () const
{
	return fId;
}
void OOPObjectId::SetProcId (int id)
{
	fProcId = id;
}
void OOPObjectId::SetId (int id)
{
	fId = id;
}
OOPObjectId::OOPObjectId (const::OOPObjectId & obj)
{
	fId = obj.GetId ();
	fProcId = obj.GetProcId ();
}
int OOPObjectId::Pack (OOPStorageBuffer * buf)
{
	int aux = 0;
	aux = GetId ();
	buf->PkInt (&aux);
	aux = GetProcId ();
	buf->PkInt (&aux);
	return 1;
}
int OOPObjectId::Unpack (OOPStorageBuffer * buf)
{
	int aux = 0;
	buf->UpkInt (&aux);
	SetId (aux);
	buf->UpkInt (&aux);
	SetProcId (aux);
	return 1;
}

int OOPObjectId::IsZeroOOP() const
{
	return (fId == 0 && fProcId == 0);
}
