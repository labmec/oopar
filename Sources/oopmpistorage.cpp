
//
// Autor:   Fabio Amaral Castro,  RA: 991722
//
// Arquivo: str_mpi.c
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
// Versao:  01 / 03.
//
#include "storage.h"
#include "str_mpi.h"
#include "mpi.h"


//TSend

OOPMPISendStorage::OOPMPISendStorage(int f_target){
  f_target_tid = f_target;
  ResetBuffer();
}
  
  // Metodos para EMPACOTAR os dados a serem enviados.
  //  p : Ponteiro para o buffer que contem os dados a serem empacotados.
  //  n : Numero de elementos no buffer (default: um unico dado).
  //
int OOPMPISendStorage::PkByte(char *p, int n){
  if(f_position+n*(sizeof(char))>f_size) ExpandBuffer(n*(sizeof(char))+1000);
  MPI_Pack(p,n,MPI_CHAR,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkInt(int *p, int n){
  if(f_position+n*(sizeof(int))>f_size) ExpandBuffer(n*(sizeof(int))+1000);
  MPI_Pack(p,n,MPI_INT,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkShort(short *p, int n){
  if(f_position+n*(sizeof(short))>f_size) ExpandBuffer(n*(sizeof(short))+1000);
  MPI_Pack(p,n,MPI_SHORT,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkLong(long *p, int n){
  if(f_position+n*(sizeof(long))>f_size) ExpandBuffer(n*(sizeof(long))+1000);
  MPI_Pack(p,n,MPI_LONG,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}
 
int OOPMPISendStorage::PkUint(u_int *p, int n){
  if(f_position+n*(sizeof(u_int))>f_size) ExpandBuffer(n*(sizeof(u_int))+1000);
  MPI_Pack(p,n,MPI_UNSIGNED,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkUshort(u_short *p, int n){
  if(f_position+n*(sizeof(u_short))>f_size) ExpandBuffer(n*(sizeof(u_short))+1000);
  MPI_Pack(p,n,MPI_UNSIGNED_SHORT,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkUlong(u_long *p, int n){
  if(f_position+n*(sizeof(u_long))>f_size) ExpandBuffer(n*(sizeof(u_long))+1000);
  MPI_Pack(p,n,MPI_UNSIGNED_LONG,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkFloat(float *p, int n){
  if(f_position+n*(sizeof(float))>f_size) ExpandBuffer(n*(sizeof(float))+1000);
  MPI_Pack(p,n,MPI_FLOAT,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkDouble(double *p, int n){
  if(f_position+n*(sizeof(double))>f_size) ExpandBuffer(n*(sizeof(double))+1000);
  MPI_Pack(p,n,MPI_DOUBLE,f_buffr,f_size,&f_position,MPI_COMM_WORLD);
  return 1;
}

int OOPMPISendStorage::PkStr(char *p){
  //em PkStr, conta-se o número de caracteres na string para depois empacotá-la
  int n=0;
  for(;p[n] != '\0';n++);
  if(f_position+n*(sizeof(char))>f_size) ExpandBuffer(n*(sizeof(char))+1000);
  int maxbuff=f_position+n*(sizeof(char));
  MPI_Pack(p,n,MPI_CHAR,f_buffr,maxbuff,&f_position,MPI_COMM_WORLD);
  return 0;
}

void OOPMPISendStorage::ExpandBuffer(int more_dimension){ 
  if(more_dimension<0) Finish("ExpandBuffer <Cannot accept negative number as an argument>");
  f_size = f_size + more_dimension;
  const char *prov_buffer = f_buffr;
  f_buffr = new(char[sizeof(int)]);
  for(int i=0;i<f_position;i++) f_buffr[i]=prov_buffer[i];
  return;
}

int OOPMPISendStorage::ResetBuffer(){
  f_position=sizeof(int);
  f_size=sizeof(int);
  f_buffr = new(char[sizeof(int)]);
  return 1;
}


int OOPMPISendStorage::Send(int msg_id){
  //MPI_Send(&f_position,1,MPI_INT,f_target_tid,msg_id,MPI_COMM_WORLD);
  int size_position=0,max_buffr_size = 1000000, ret;
  //empacota extensao no inicio do pacote
  MPI_Pack(&f_position,1,MPI_INT,f_buffr,f_position,&size_position,MPI_COMM_WORLD);
  //envia 1000000 bytes de cada vez
  for(int i=0;i<f_position;i = i+max_buffr_size){
    char *send_buffr = f_buffr + i;
    int block_size = max_buffr_size;
    if(f_position<i+max_buffr_size) {
      block_size = f_position-i;
    }
    ret = MPI_Send(send_buffr,block_size,MPI_PACKED,f_target_tid,msg_id,MPI_COMM_WORLD);
  }
  if (ret>0) ResetBuffer();
  return ret;
}

//       TReceiveStorageMpi


int OOPMPIReceiveStorage::Receive(){    //nonblocking!!!!
  MPI_Status status;
  MPI_Request request;
  int max_buffr_size=1000000,size_position=0,request_flag;
  char *receive_buffer;
  receive_buffer = new(char[max_buffr_size]);
  //recebe (nonblocking) primeros 10^6 bytes
  MPI_Irecv(receive_buffer,max_buffr_size,MPI_PACKED,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&request);
  MPI_Test(&request,&request_flag,&status);
  if(request_flag){
    //desempacota dimensao do pacote completo
    MPI_Unpack(receive_buffer,max_buffr_size,&size_position,&f_size,1,MPI_INT,MPI_COMM_WORLD);
    f_buffr = new(char[f_size]);
    f_sender_tid = status.MPI_SOURCE;
    f_msg_id = status.MPI_TAG;
    f_position=sizeof(int);
    //recebe demais blocos de 10^6 bytes
    int receive_position;
    for(receive_position=0;receive_position<max_buffr_size;receive_position++) 
      if(receive_position<f_size) f_buffr[receive_position] = receive_buffer[receive_position];
    for(int i=max_buffr_size;i<f_size;i = i+max_buffr_size){
      MPI_Recv(receive_buffer,max_buffr_size,MPI_PACKED,f_sender_tid,f_msg_id,MPI_COMM_WORLD,&status);
      for(int j=0;j<max_buffr_size;j++,receive_position++) 
	if(receive_position<f_size) f_buffr[receive_position] = receive_buffer[receive_position];
    }
    
    return 1;
  }
  else{
    MPI_Cancel(&request);
    return 0;
  }
}

int OOPMPIReceiveStorage::ReceiveBlocking(){
  MPI_Status status;	       
  int max_buffr_size=1000000,size_position=0;
  char *receive_buffer;
  receive_buffer = new(char[max_buffr_size]);
  //recebe primeiros 10^6 bytes
  MPI_Recv(receive_buffer,max_buffr_size,MPI_PACKED,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
  //desempacota dimensao do pacote completo
  MPI_Unpack(receive_buffer,max_buffr_size,&size_position,&f_size,1,MPI_INT,MPI_COMM_WORLD);
  f_buffr = new(char[f_size]);
  f_sender_tid = status.MPI_SOURCE;
  f_msg_id = status.MPI_TAG;
  f_position=sizeof(int);
  //recebe demais blocos de 10^6 bytes
  int receive_position;
  for(receive_position=0;receive_position<max_buffr_size;receive_position++) 
    if(receive_position<f_size) f_buffr[receive_position] = receive_buffer[receive_position];
  for(int i=max_buffr_size;i<f_size;i = i+max_buffr_size){
    MPI_Recv(receive_buffer,max_buffr_size,MPI_PACKED,f_sender_tid,f_msg_id,MPI_COMM_WORLD,&status);
    for(int j=0;j<max_buffr_size;j++,receive_position++) 
      if(receive_position<f_size) f_buffr[receive_position] = receive_buffer[receive_position];
  }
  return 1;
}

  // Metodos para DESEMPACOTAR dados do buffer.
  //  p : Ponteiro para o buffer onde os dados serao lidos.
  //  n : Numero de elementos a serem lidos (default: um unico dado).
int OOPMPIReceiveStorage::UpkByte(char *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_CHAR,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(char);
  return 1;
}

int OOPMPIReceiveStorage::UpkInt(int *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_INT,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(int);
  return 1;
}

int OOPMPIReceiveStorage::UpkShort(short *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_SHORT,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(short);
  return 1;
}

int OOPMPIReceiveStorage::UpkLong(long *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_LONG,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(long);
  return 1;
}

int OOPMPIReceiveStorage::UpkUint(u_int *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_UNSIGNED,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(u_int);
  return 1;
}

int OOPMPIReceiveStorage::UpkUshort(u_short *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_UNSIGNED_SHORT,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(u_short);
  return 1;
}

int OOPMPIReceiveStorage::UpkUlong(u_long *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_UNSIGNED_LONG,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(u_long);
  return 1;
}

int OOPMPIReceiveStorage::UpkFloat(float *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_FLOAT,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(float);
  return 1;
}

int OOPMPIReceiveStorage::UpkDouble(double *p, int n){
  MPI_Unpack(f_buffr,f_size,&f_position,p,n,MPI_DOUBLE,MPI_COMM_WORLD);
  //f_position=f_position+n*sizeof(double);
  return 1;
}

//essa eh meio complicada...
int OOPMPIReceiveStorage::UpkStr(char *p){
  int n=-1;
  do{
    n++;
    MPI_Unpack(f_buffr,f_size,&f_position,&p[n],1,MPI_DOUBLE,MPI_COMM_WORLD);
  }while(p[n] !='\0"');
  //f_position=f_position+n;
  return 0;
}



