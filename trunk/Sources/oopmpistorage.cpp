
//
// Autor:   Fabio Amaral Castro,  RA: 991722
//
// Arquivo: storage_mpi.c
//
// Classe:  TSendStorageMpi,
//          TReceiveStorageMpi.
//
// Descr.: 
//          TSendStorageMpi:
//            Classe derivada de TSendStorage que implementa o buffer
//            usando PVM3.3.7 para a comunicacao entre processos.
//
//          TReceiveStorageMpi:
//            Classe derivada de TReceiveStorage que implementa o buffer
//            usando PVM3.3.7 para a comunicacao entre processos.
//
// Versao:  01 / 03.
//
#include "oopstorage.h"
#include "oopmpistorage.h"

//TSend

int OOPMPISendStorage::PkByte(char *p, int n=1){
  maxbuff=position+n;
  MPI_Pack(p,n,MPI_CHAR,buffr,maxbuff,&position,MPI_COMM_WORLD);
  position=maxbuff;
  return 0;
}

int OOPMPISendStorage::Send(int msg_id, int tid){
  MPI_Send(&position,1,MPI_INT,tid,msg_id,MPI_COMM_WORLD);
  MPI_Send(buffr,position,MPI_PACKED,tid,msg_id,MPI_COMM_WORLD);
  return 0;
}

int OOPMPIReceiveStorage::Receive(int msg_id, int tid){
  MPI_Status status;	       
  //como saber tamanho do dado a receber???
  MPI_Recv(&size,1 MPI_INT,tid,msg_id,MPI_COMM_WORLD,&status);
  buffr = new(char[size]);
  MPI_Recv(buffr,size,MPI_PACKED,tid,msg_id,MPI_COMM_WORLD,&status);
  return 0;
}

int OOPMPIReceiveStorage::UpkByte(char *p, int n=1){
  MPI_Unpack(buffr,size,&position,p,n,MPI_CHAR,MPI_COMM_WORLD);
  position=position+n;
  return 0;
}

//TReceive

OOPSaveable *OOPMPIReceiveStorage::Restore()
{
  long class_id;
	//Assumindo 1 como número de elementos a serem lidos.
  UpkLong(&class_id, 1);
  if ( !class_id )
    return( 0 );

  map<long,void*>::iterator i;
  i = gFuncTree.find(class_id);
  //i = gFuncTree.find(class_id);
  if (i==gFuncTree.end())
    {
      char str[20];
      sprintf( str, "%d", (int)class_id );
      Error( 1, "Restore <class_id %s not recognized>\n", str );
    }

  TRestFunction f;
  //void *c = i;//gFuncTree.contents(i);
  f = (TRestFunction) gFuncTree[class_id];//i;//c;

  return f(this);

}


void OOPMPIReceiveStorage::AddClassRestore(long classid,TRestFunction f)
{
  gFuncTree[classid]= (void *) f;
}



