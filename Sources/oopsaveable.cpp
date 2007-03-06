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
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPSaveable"));
#endif

/************/
/*** Pack ***/
int OOPSaveable::Pack (OOPStorageBuffer * buf)
{
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "PID" << getpid() << " Packing class ID " << GetClassID() ;
    LOGPZ_DEBUG(logger,sout.str());
#endif    
  }
	long class_id = GetClassID ();
  if(class_id == -1) {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Packing class with uninitialized classid expect trouble";
    LOGPZ_ERROR(logger,sout.str());
#endif    
  }
	buf->PkLong (&class_id);
	return 1;
}
/**************/
/*** Unpack ***/
int OOPSaveable::Unpack (OOPStorageBuffer * buf)
{
	return 1;
}

OOPSaveable *OOPSaveable::Restore(OOPStorageBuffer *buf)
{
  return 0;
}
