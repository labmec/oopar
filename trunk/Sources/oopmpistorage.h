
//
// Autor:   Fabio Amaral Castro,  RA: 991722
//
// Arquivo: storage_mpi.h
//
// Classe:  TSendStorageMpi,
//          TReceiveStorageMpi.
//
// Descr.: 
//          TSendStorageMpi:
//            Classe derivada de TSendStorage que implementa o buffer
//            usando MPI para a comunicacao entre processos.
//
//          TReceiveStorageMpi:
//            Classe derivada de TReceiveStorage que implementa o buffer
//            usando MPI para a comunicacao entre processos.
//
// Versao:  01 / 03
//
#include "oopstorage.h"


class OOPMPISendStorage: public OOPSendStorage
{
  char *buffr;     //********pensar em sua dimensao
  int position;
public:

  OOPMPISendStorage()  {position=0;}
  ~OOPMPISendStorage() {}

  // Metodos para EMPACOTAR os dados a serem enviados.
  //  p : Ponteiro para o buffer que contem os dados a serem empacotados.
  //  n : Numero de elementos no buffer (default: um unico dado).
  //
  int PkByte  ( char    *p, int n);
  int PkInt   ( int     *p, int n);
  int PkShort ( short   *p, int n);
  int PkLong  ( long    *p, int n);
  int PkUint  ( u_int   *p, int n);
  int PkUshort( u_short *p, int n);
  int PkUlong ( u_long  *p, int n);
  int PkFloat ( float   *p, int n);
  int PkDouble( double  *p, int n);
  int PkStr   ( char *str );

  // Envia os dados contidos no buffer e reinicializa-o.
  int Send( int msg_id, int tid ) ;

  // Usada em TError.
  char *ClassName()  { return( "OOPMPISendStorage" ); }
  
};


class OOPMPIReceiveStorage:public OOPReceiveStorage
{
  char *buffr;
  int size,position;
public:

  OOPMPIReceiveStorage()  {size=0,position=0;}
  virtual ~OOPMPIReceiveStorage() {};

  // Metodos para DESEMPACOTAR dados do buffer.
  //  p : Ponteiro para o buffer onde os dados serao lidos.
  //  n : Numero de elementos a serem lidos (default: um unico dado).
  //
  int UpkByte  ( char    *p, int n);
  int UpkInt   ( int     *p, int n);
  int UpkShort ( short   *p, int n);
  int UpkLong  ( long    *p, int n);
  int UpkUint  ( u_int   *p, int n);
  int UpkUshort( u_short *p, int n);
  int UpkUlong ( u_long  *p, int n);
  int UpkFloat ( float   *p, int n);
  int UpkDouble( double  *p, int n);
  int UpkStr   ( char *str );

  static void AddClassRestore(long classid,TRestFunction f);

  // Recebe os dados para o buffer.
  int Receive(int msg_id, int tid);

  // Restaura o proximo objeto do buffer.
  //
  OOPSaveable *Restore();

  char *ClassName() { return( "OOPMPIReceiveStorage" ); }


private:

  static map<long,void*> gFuncTree;

};

