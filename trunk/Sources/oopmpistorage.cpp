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

// $Author: longhin $
// $Id: oopmpistorage.cpp,v 1.24 2004-08-09 19:12:13 longhin Exp $
// $Revision: 1.24 $



#include "oopmpistorage.h"
#include "mpi.h"
#include <iostream>
#include <sys/types.h>
      #include <unistd.h>

void OOPMPIStorageBuffer::ExpandBuffer (int more_dimension)
{
	f_send_buffr.Resize(f_send_buffr.NElements()+more_dimension);
	if (more_dimension < 0) {
#warning "Finish("ExpandBuffer <Cannot accept negative number as an argument>");"
	}
}
int OOPMPIStorageBuffer::ResetBuffer (int size)
{
	f_send_position = 0;
	f_send_buffr.Resize(0);
	return 1;
}
int OOPMPIStorageBuffer::PackGeneric (void *ptr, int n, int mpitype)
{
	int nbytes;
	MPI_Pack_size(n,mpitype,MPI_COMM_WORLD,&nbytes);
	f_send_buffr.Resize(f_send_position+nbytes);
	int mpiret;
	mpiret = MPI_Pack (ptr, n, mpitype, &f_send_buffr[0], f_send_buffr.NElements(), &f_send_position,
		  MPI_COMM_WORLD);
	return mpiret;
}
int OOPMPIStorageBuffer::Send (int target)
{
#ifdef VERBOSE
	cout << "PID" << getpid() << " Called MPI_Send ret = \n";
	cout.flush();
#endif
	int ret;
	int tag = 0;
	ret = MPI_Send (&f_send_buffr[0], f_send_position, MPI_PACKED,
				target, tag, MPI_COMM_WORLD);
#ifdef VERBOSE
	switch(ret){
		case MPI_SUCCESS:
			cout <<" - No error; MPI routine completed successfully\n";
			break;
		case MPI_ERR_COMM:
			cout << "-  Invalid communicator.  A common error is to use a null communicator in a call (not even allowed in MPI_Comm_rank ).\n";
			break;
		case MPI_ERR_COUNT:
			cout << "- Invalid count argument.  Count arguments must be non-negative a count of zero is often valid\n";
			break;
		case MPI_ERR_TYPE:
		  cout << "- Invalid datatype argument.  May be an uncommitted MPI_Datatype (see MPI_Type_commit ).\n";
			break;
		case MPI_ERR_TAG:
		  cout << "- Invalid tag argument.  Tags must be non-negative;  tags  in  a\n"
		   << "receive  (  MPI_Recv , MPI_Irecv , MPI_Sendrecv , etc.) may also\n"
			<< "be MPI_ANY_TAG .  The largest tag value is available through the\n"
		  << "the attribute MPI_TAG_UB .\n";
			break;
		case MPI_ERR_RANK:
			cout << "-  Invalid  source  or  destination rank.\n";
			break;
	}
	cout.flush();
#endif
	ResetBuffer();
	return ret;
}
int OOPMPIStorageBuffer::PkStr (char *p)
{
	int len = strlen(p);
	PkInt(&len,1);
	//return PackGeneric(p,len,MPI_CHAR);
	return PkByte(p,len);
}
int OOPMPIStorageBuffer::PkDouble (double *p, int n)
{
	return PackGeneric(p,n,MPI_DOUBLE);
}
int OOPMPIStorageBuffer::PkFloat (float *p, int n)
{
	return PackGeneric(p,n,MPI_FLOAT);
}
int OOPMPIStorageBuffer::PkUlong (u_long * p, int n)
{
	return PackGeneric(p,n,MPI_UNSIGNED_LONG);
}
int OOPMPIStorageBuffer::PkUshort (u_short * p, int n)
{
	return PackGeneric(p,n,MPI_UNSIGNED_SHORT);
}
int OOPMPIStorageBuffer::PkUint (u_int * p, int n)
{
	return PackGeneric(p,n,MPI_UNSIGNED);
}
int OOPMPIStorageBuffer::PkLong (long *p, int n)
{
	return PackGeneric(p,n,MPI_LONG);
}
int OOPMPIStorageBuffer::PkShort (short *p, int n)
{
	return PackGeneric(p,n,MPI_SHORT);
}
int OOPMPIStorageBuffer::PkInt (int *p, int n)
{
	return PackGeneric(p,n,MPI_INT);
}
int OOPMPIStorageBuffer::PkByte (const char *p, int n)
{
	return PackGeneric((char *)p,n,MPI_CHAR);
}
using namespace std;

  // Metodos para EMPACOTAR os dados a serem enviados.
  // p : Ponteiro para o buffer que contem os dados a serem empacotados.
  // n : Numero de elementos no buffer (default: um unico dado).
  // 


//       TReceiveStorageMpi
OOPMPIStorageBuffer::~OOPMPIStorageBuffer()
{
	//if(f_request) MPI_Request_free(&f_request);
}
void OOPMPIStorageBuffer::FreeRequest(){
	if(f_request) MPI_Request_free(&f_request);
}
int OOPMPIStorageBuffer::Receive ()
{      // nonblocking!!!!
	//f_buffr.Resize(50000);
	if(f_isreceiving) return 1;
	//MPI_Status status;
	//int test_flag;
	// recebe (nonblocking) primeros 10^6 bytes
	MPI_Irecv (&f_recv_buffr[0], f_recv_buffr.NElements(), MPI_PACKED, MPI_ANY_SOURCE,
		   MPI_ANY_TAG, MPI_COMM_WORLD, &f_request);
	f_isreceiving = 1;
	return 1;
}

bool OOPMPIStorageBuffer::TestReceive() {
	if(!f_isreceiving) return false;
	MPI_Status status;
	int test_flag, ret_test;
	ret_test=MPI_Test (&f_request, &test_flag, &status);
#ifdef VERBOSE
	cout << "Test returned " << ret_test << endl;
	cout << "Flag " << test_flag << endl;
	cout.flush();
#endif	
	return test_flag;
}
  /**
   * Restores next object in the buffer
   */
TPZSaveable *OOPMPIStorageBuffer::Restore () {
	if(!TestReceive()) {
		cout << "Restore called at the wrong moment\n";
		cout.flush();
		return NULL;
	}
	f_isreceiving = 0;
	f_recv_position = 0;
	TPZSaveable *obj = TPZSaveable::Restore(*this, 0);
	//MPI_Request_free(&f_request);
	return obj;
}

int OOPMPIStorageBuffer::ReceiveBlocking ()
{
	//f_buffr.resize(50000);
	Receive();
	if(TestReceive()) {
		return 1;
		
	}
	//MPI_Status status;
#ifdef VERBOSE
	cout << "Going to MPI_Wait\n";
	cout << "PID" << getpid() << endl;
	cout.flush();
#endif
	//MPI_Wait(&f_request,&status);
	sleep(1);
	return 1;
	/*
	MPI_Status status;
	// recebe primeiros 10^6 bytes
	cout << "MPI_Recv returned " << 
	MPI_Recv (&f_buffr[0], f_buffr.NElements(), MPI_PACKED, MPI_ANY_SOURCE,
		   MPI_ANY_TAG, MPI_COMM_WORLD, &status) << endl;
	// desempacota dimensao do pacote completo
	cout << "Returning 1\n";
	cout.flush();
	*/
}
  // Metodos para DESEMPACOTAR dados do buffer.
  // p : Ponteiro para o buffer onde os dados serao lidos.
  // n : Numero de elementos a serem lidos (default: um unico dado).
int OOPMPIStorageBuffer::UpkByte (char *p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_CHAR,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(char);
	return 1;
}
int OOPMPIStorageBuffer::UpkInt (int *p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_INT,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(int);
	return 1;
}
int OOPMPIStorageBuffer::UpkShort (short *p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_SHORT,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(short);
	return 1;
}
int OOPMPIStorageBuffer::UpkLong (long *p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_LONG,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(long);
	return 1;
}
int OOPMPIStorageBuffer::UpkUint (u_int * p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_UNSIGNED,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(u_int);
	return 1;
}
int OOPMPIStorageBuffer::UpkUshort (u_short * p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_UNSIGNED_SHORT,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(u_short);
	return 1;
}
int OOPMPIStorageBuffer::UpkUlong (u_long * p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_UNSIGNED_LONG,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(u_long);
	return 1;
}
int OOPMPIStorageBuffer::UpkFloat (float *p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_FLOAT,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(float);
	return 1;
}
int OOPMPIStorageBuffer::UpkDouble (double *p, int n)
{
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_DOUBLE,
		    MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(double);
	return 1;
}
//essa eh meio complicada...
int OOPMPIStorageBuffer::UpkStr (char *p)
{
	int n;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, &n, 1,
			    MPI_INT, MPI_COMM_WORLD);
	UpkByte(p,n);
	return 1;
}
 void OOPMPIStorageBuffer::Write(int *p, int size){
	 PkInt(p, size);
 }
 void OOPMPIStorageBuffer::Write(double *p, int size){
	 PkDouble(p, size);
 }
 void OOPMPIStorageBuffer::Write(const char *p, int size){
	 PkByte(p, size);
 }
 void OOPMPIStorageBuffer::Write(string *p, int size){
	 PkInt(&size);
	 char* buf = new char[p->length()];
  	 p->copy(buf, p->length());	 
	 PkStr(buf);
 }
 void OOPMPIStorageBuffer::Read(int *p, int size){
	 UpkInt(p, size);	 
 }
 void OOPMPIStorageBuffer::Read(double *p, int size){
	 UpkDouble(p, size);
 }
 void OOPMPIStorageBuffer::Read(char *p, int size){
	 UpkByte(p, size);
 }
 void OOPMPIStorageBuffer::Read(string *p, int size){
	 UpkInt(&size);
	 char * buf = new char[size];
	 int i=0;
	 for (i=0;i<size;i++)
	 	UpkStr(&buf[i]);
	 p->insert(size, buf);
 }
