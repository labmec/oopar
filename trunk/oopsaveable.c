
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
  buf->PkLong( &fDMId);
  return 1;
}


/**************/
/*** Unpack ***/
int 
OOPSaveable::Unpack( OOPReceiveStorage *buf ) 
{
  return buf->UpkLong(&fDMId);
}


/*********************/
/*** Set Global ID ***/
long
OOPSaveable::SetGlobalId(long id)
{
  return( fDMId = id );
}


/*****************/
/*** Global ID ***/
long
OOPSaveable::GlobalId()
{
  return fDMId;
}


/********************/
/*** Derived From ***/
int 
OOPSaveable::DerivedFrom(long id) 
{
  return (id == (long) (CID_TSAVEABLE));
}


/********************/
/*** Derived From ***/
int 
OOPSaveable::DerivedFrom(char *classname) 
{
  return (!strcmp(OOPSaveable::ClassName(),classname));
}


/***************/
/*** Version ***/
OOPDataVersion 
OOPSaveable::Version() 
{
  return -1L;
}


/*************************/
/*** Increment Version ***/
void
OOPSaveable::IncrementVersion()
{
  printf( "OOPSaveable::IncrementVersion <not implemented>.\n" );
  exit(-1);
  //return 0;
}


