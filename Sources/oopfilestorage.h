#ifdef FILE_COMM
// -*- c++ -*-
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: str_file.hh
//
// Classe:  TSendStorageFile, TReceiveStorageFile.
//
// Descr.:  TSendStorageFile:
//            Classe derivada de TSendStorage que implementa o buffer
//            usando arquivos para a comunicacao entre processos.
//
//          TReceiveStorageFile:
//            Classe derivada de TReceiveStorage que implementa o buffer
//            usando arquivos para a comunicacao entre processos.
//
// Versao:  09 / 08 / 95.
//
#ifndef _STR_FILEHH_
#define _STR_FILEHH_
#include <oopstorage.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#define FILE_NAME_SIZE  16
/*********************/
class OOPSendStorageFile:public OOPSendStorage
{
      public:
	// 'prefix' = prefixo a ser usado na geracao dos nomes dos
	// arquivos que serao usados como buffers.
	// 
	// nome_arq_buffer = prefix + XX.
	// 
	OOPSendStorageFile (char *prefix, int my_id);
	       ~OOPSendStorageFile ();

	// Metodos para EMPACOTAR os dados a serem enviados.
	// p : Ponteiro para o buffer que contem os dados a serem
	// empacotados.
	// n : Numero de elementos no buffer (default: um unico dado).
	// 
	// Retorna != 0 se houver escrito algo no arquivo em que esta
	// ligado. Neste caso, finaliza o arquivo, retorna seu nome
	// em 'file_to_send' e procura outro arquivo para se ligar.
	// Obs.: Nao esquecer de acrescentar um '0' no final do arquivo!
	// 
      private:
	// Procura por um novo arquivo a ser usado como buffer.
	// Se nao encontrar retorna 0.
	// 

// Prefixo dos arquivos que podem ser usados
	// como buffer.
// 2 ultimos digitos do nome do arq ligado
	// (XX)
	// onde: arquivo_ligado = prefixo + XX.
	FILE   *f_file;
};
typedef OOPSendStorageFile *PTSendStorageFile;
/**
 * Same attributions as its base class.
 * Works with File Communication
 */
class   OOPFileStorageBuffer:public OOPStorageBuffer
{
      public:
	OOPFileStorageBuffer (char *prefix, int my_id);
	OOPFileStorageBuffer (char *fname);
	       ~OOPFileStorageBuffer ();
	int     GetSender ()
	{
		return f_sender;
	}
	int     UpkByte (char *p, int n = 1);
	int     UpkInt (int *p, int n = 1);
	int     UpkShort (short *p, int n = 1);
	int     UpkLong (long *p, int n = 1);
	int     UpkUint (u_int * p, int n = 1);
	int     UpkUshort (u_short * p, int n = 1);
	int     UpkUlong (u_long * p, int n = 1);
	int     UpkFloat (float *p, int n = 1);
	int     UpkDouble (double *p, int n = 1);
	int     UpkStr (char *str);

	int     PkByte (char *p, int n = 1);

	int     PkInt (int *p, int n = 1);

	int     PkShort (short *p, int n = 1);

	int     PkLong (long *p, int n = 1);

	int     PkUint (u_int * p, int n = 1);

	int     PkUshort (u_short * p, int n = 1);

	int     PkUlong (u_long * p, int n = 1);

	int     PkFloat (float *p, int n = 1);

	int     PkDouble (double *p, int n = 1);

	int     PkStr (char *str);

	int     HasSomeThingToSend (char *file_to_send);

	int     Open ();

	int     Close ();

	void    FileName (char *);

	char   *ClassName1 ()
	{
		return ("TSendStorageFile::");
	}

private:

	int     FindNewBuffer ();

public:
	virtual char *ClassName ()
	{
		return ("TReceiveStorageFile::");
	}

    virtual void Write(int *p, int size);

    virtual void Write(double *p, int size);

    virtual void Write(const char *p, int size);

    virtual void Write(string *p, int size);

    virtual void Read(int *p, int size);

    virtual void Read(double *p, int size);

    virtual void Read(char *p, int size);

    virtual void Read(string *p, int size);

      private:
	int     f_myID;
	char    f_wrote;
	char   *f_prefix;
	int     f_file_num;
	char    f_file_name[FILE_NAME_SIZE];
	int     f_sender;
	FILE   *f_file;
};

typedef OOPFileStorageBuffer *PTFileStorageBuffer;

#endif // _STR_FILEHH_

#endif //FILE_COMM