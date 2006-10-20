//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: storage.cc
//
// Classe:  TSendStorage, TReceiveStorage, TSendStoragePvm,
//          TReceiveStoragePvm.
//
// Descr.:  TSendStorage:
//            Classe base para buffers de escrita usados na transmissao
//            de dados.
//
//          TReceiveStorage:
//            Classe basse para buffers de leitura usados na recepcao
//            de dados.
//
//          TSendStoragePvm:
//            Classe derivada de TSendStorage que implementa o buffer
//            usando PVM3.3.7 para a comunicacao entre processos.
//
//          TReceiveStoragePvm:
//            Classe derivada de TReceiveStorage que implementa o buffer
//            usando PVM3.3.7 para a comunicacao entre processos.
//
// Versao:  27 / 05 / 95.
//
#include "oopstorage.h"
//#include "oopsaveable.h"
#include "cmdefs.h"
#include <iostream>
//       #include <sys/types.h>
//       #include <unistd.h>
using namespace std;
class   OOPSaveable;

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPStorageBuffer"));
#endif

/************************ TSendStorage ************************/
/************************ TReceiveStorage ************************/
//TVoidPtrMap TReceiveStorage::gFuncTree(0);
//void * TReceiveStorage::gFuncTree(long class_id);
map < long, TRestFunction>OOPStorageBuffer::gFuncTree;
/******************/
/*** Destructor ***/
OOPStorageBuffer::~OOPStorageBuffer ()
{
}
/***************/
/*** Restore ***/
OOPSaveable *OOPStorageBuffer::Restore ()
{
	long class_id = 0; 
	UpkLong (&class_id);
#ifdef DEBUG
  {
#ifdef LOGPZ    
    stringstream sout;
    sout << "PID" << getpid() << " Restorig object of class Id " << class_id;
    LOGPZ_DEBUG (logger,sout.str());
#endif    
  }
#endif
	if (!class_id) {
#ifdef LOGPZ    
    stringstream sout;
    sout << "Invalid class Id " << class_id << " Going out of OOPReceiveStorage::Restore";
    LOGPZ_DEBUG (logger,sout.str());
#endif    
		return (0);
	}
	map < long, TRestFunction >::iterator i;
	i = gFuncTree.find (class_id);
	// i = gFuncTree.find(class_id);
	if (i == gFuncTree.end ()) {
		char str[20];
		sprintf (str, "%d", (int) class_id);
		Error (1, "Restore <class_id %s not recognized>\n", str);
	}
	TRestFunction f;
	// void *c = i;//gFuncTree.contents(i);
	f = gFuncTree[class_id];	// i;//c;
	return f (this);
}
void OOPStorageBuffer::AddClassRestore (long classid, TRestFunction f)
{
	gFuncTree[classid] = f;
}
