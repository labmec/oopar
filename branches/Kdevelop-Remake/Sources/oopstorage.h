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
#ifndef _STORAGEHH_
#define _STORAGEHH_
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <pzfilebuffer.h>

using namespace std;
class   OOPSaveable;
class   OOPStorageBuffer;
typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef OOPSaveable * ( *TRestFunction ) ( OOPStorageBuffer * );
class   OOPSaveable;
/**
 * Base class for for input buffers used on data transmition
 */
class   OOPSendStorage:public TPZStream
{
	public:
		/**
		 * Simple constructor
		 */
		OOPSendStorage ()
		{}
		virtual ~ OOPSendStorage ()
		{}

		/**
		 * Returns class name.
		 */
		virtual char *ClassName ()
		{
			return ( "OOPSendStorage::" );
		}
};
typedef OOPSendStorage *PTSendStorage;
/**
 * Base class for for output buffers used on data transmition
 */
class   OOPStorageBuffer:public TPZStream
{
	public:
		/**
		 * Simple constructor.
		 */
		OOPStorageBuffer ()
		{}
		virtual ~OOPStorageBuffer ();	// {}
		/**
		Methods for unpacking data transmitted.
		@param p Pointer to the buffer which holds the data to be packed
		@param n Number of elements on the buffer
		*/
		virtual int UpkByte ( char *p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkInt ( int *p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkShort ( short *p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkLong ( long *p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkUint ( u_int * p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkUshort ( u_short * p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkUlong ( u_long * p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkFloat ( float *p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkDouble ( double *p, int n = 1 )
		{
			return 0;
		}
		virtual int UpkStr ( char *str )
		{
			return 0;
		}
		static void AddClassRestore ( long classid, TRestFunction f );
		/**
		 * Receives the data to he buffer
		 */
		virtual int Receive ( int msg_id, int tid )
		{
			return 0;
		}
		/**
		 * Restores next object in the buffer
		 */
		OOPSaveable *Restore ();

		/**
		* 	Methods for packing data to be transmitted.
		* 	@param p Pointer to the buffer which holds the data to be packed
		* 	@param n Number of elements on the buffer
		*/
		virtual int PkByte ( char *p, int n = 1 )
		{
			return 0;
		}

		virtual int PkInt ( int *p, int n = 1 )
		{
			return 0;
		}

		virtual int PkShort ( short *p, int n = 1 )
		{
			return 0;
		}

		virtual int PkLong ( long *p, int n = 1 )
		{
			return 0;
		}

		virtual int PkUint ( u_int * p, int n = 1 )
		{
			return 0;
		}

		virtual int PkUshort ( u_short * p, int n = 1 )
		{
			return 0;
		}

		virtual int PkUlong ( u_long * p, int n = 1 )
		{
			return 0;
		}

		virtual int PkFloat ( float *p, int n = 1 )
		{
			return 0;
		}

		virtual int PkDouble ( double *p, int n = 1 )
		{
			return 0;
		}

		virtual int PkStr ( char *str )
		{
			return 0;
		}

		/**
		 * Undocumented
		 */
		virtual int Send ( int msg_id, int tid )
		{
			return 0;
		}

		/**
		 * Returns class name
		 */
		virtual char *ClassName ()
		{
			return ( "OOPStorageBuffer" );
		}
	private:
		/**
		 * Pointer to restore functions of all classes present on the environment.
		 */
		static  map < long, TRestFunction >gFuncTree;
};
typedef OOPStorageBuffer *PTReceiveStorage;
#endif // _STORAGEHH_
