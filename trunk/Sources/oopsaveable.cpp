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

class   OOPStorageBuffer;
class   OOPStorageBuffer;
class   OOPDataVersion;


#include <sstream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

static LoggerPtr logger(Logger::getLogger("OOPAR.OOPSaveable"));


/************/
/*** Pack ***/
int OOPSaveable::Pack (OOPStorageBuffer * buf)
{
  stringstream sout;
#ifdef DEBUG
  sout << "PID" << getpid() << " Packing class ID " << GetClassID() << endl;
  LOG4CXX_DEBUG(logger,sout);
  sout.clear();
#endif
	long class_id = GetClassID ();
  if(class_id == -1) {
    sout << "Packing class with uninitialized classid expect trouble\n";
    LOG4CXX_DEBUG(logger,sout);
    sout.clear();
  }
	buf->PkLong (&class_id);
	return 1;
}
/**************/
/*** Unpack ***/
int OOPSaveable::Unpack (OOPStorageBuffer * buf)
{
#ifndef WIN32
#warning "Unpack on the OOPSaveable class is empty ! Should it exist ?"
#warning "Shouldn't it be virtual ?"
#endif
	return 1;
}

OOPSaveable *OOPSaveable::Restore(OOPStorageBuffer *buf)
{
  return 0;
}
