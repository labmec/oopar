
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: saveable.cc
//
// Classe:  OOPSaveable.
//
// Descr.:  Classe base para todos objetos que quiserem usufruir do
//          Communication Manager (ver "communic.hh")
//
// Versao:  02 / 04 / 96.
//


#include "oopsaveable.h"
#include "oopdataversion.h"
class OOPSendStorage;
class OOPReceiveStorage;
class OOPDataVersion;



/********************/
/*** Get Class ID ***/
long
OOPSaveable::GetClassID()
{
  return 0;
}

/************/
/*** Pack ***/
int
OOPSaveable::Pack(OOPSendStorage * buf)
{
  long class_id = GetClassID();
  buf->PkLong( &class_id );
  fDMId.Pack(buf);
  return 1;
}


/**************/
/*** Unpack ***/
int 
OOPSaveable::Unpack( OOPReceiveStorage *buf ) 
{
  return fDMId.Unpack(buf);
}


/*********************/
/*** Set Global ID ***/
void
OOPSaveable::SetGlobalId(const OOPObjectId &id)
{
  fDMId = id;
}


/*****************/
/*** Global ID ***/
OOPObjectId
OOPSaveable::GlobalId()
{
  return fDMId;
}

int OOPSaveable::DerivedFrom(char * classname){
	#warning "Not implemented OOPSaveable::DerivedFrom(char *)";
}
int OOPSaveable::DerivedFrom(long class_id){
	#warning "Not implemented OOPSaveable::DerivedFrom(long)";
}
