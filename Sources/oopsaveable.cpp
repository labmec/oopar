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
       #include <sys/types.h>
       #include <unistd.h>

class   OOPSendStorage;
class   OOPReceiveStorage;
class   OOPDataVersion;
/********************/
/*** Get Class ID ***/
long OOPSaveable::GetClassID ()
{
	return 0;
}
/************/
/*** Pack ***/
int OOPSaveable::Pack (OOPSendStorage * buf)
{
#ifdef VERBOSE
	cout << "PID" << getpid() << " Packing class ID " << GetClassID() << endl;
	cout.flush();
#endif
	long class_id = GetClassID ();
	buf->PkLong (&class_id);
	return 1;
}
/**************/
/*** Unpack ***/
int OOPSaveable::Unpack (OOPReceiveStorage * buf)
{
#ifndef WIN32
#warning "Unpack on the OOPSaveable class is empty ! Should it exist ?"
#warning "Shouldn't it be virtual ?"
#endif
	return 1;
}
