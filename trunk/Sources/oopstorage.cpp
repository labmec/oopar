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
class   OOPSaveable;
/************************ TSendStorage ************************/
/************************ TReceiveStorage ************************/
//TVoidPtrMap TReceiveStorage::gFuncTree(0);
//void * TReceiveStorage::gFuncTree(long class_id);
map < long, void *>OOPReceiveStorage::gFuncTree;
/******************/
/*** Destructor ***/
OOPReceiveStorage::~OOPReceiveStorage ()
{
}
/***************/
/*** Restore ***/
OOPSaveable *OOPReceiveStorage::Restore ()
{
	long class_id;
	UpkLong (&class_id);
	if (!class_id) {
		return (0);
	}
	map < long, void *>::iterator i;
	i = gFuncTree.find (class_id);
	// i = gFuncTree.find(class_id);
	if (i == gFuncTree.end ()) {
		char str[20];
		sprintf (str, "%d", (int) class_id);
		Error (1, "Restore <class_id %s not recognized>\n", str);
	}
	TRestFunction f;
	// void *c = i;//gFuncTree.contents(i);
	f = (TRestFunction) gFuncTree[class_id];	// i;//c;
	return f (this);
}
void OOPReceiveStorage::AddClassRestore (long classid, TRestFunction f)
{
	gFuncTree[classid] = (void *) f;
}
