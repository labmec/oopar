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

// $Author: tiago $
// $Id: oopmpistorage.cpp,v 1.41 2006-10-02 19:56:33 tiago Exp $
// $Revision: 1.41 $



#include "oopmpistorage.h"
#include "oopcommmanager.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

#ifdef OOP_MPE
//#include "mpe.h"
#endif	

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPMPIStorageBuffer"));
#endif

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
	this->ResetByteCounter();
	return 1;
}
int OOPMPIStorageBuffer::PackGeneric (void *ptr, int n, MPI_Datatype mpitype)
{
	int nbytes;
	MPI_Pack_size(n,mpitype,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	f_send_buffr.Resize(f_send_position+nbytes);
	int mpiret;
	mpiret = MPI_Pack (ptr, n, mpitype, &f_send_buffr[0], f_send_buffr.NElements(), &f_send_position,
					   MPI_COMM_WORLD);
	return mpiret;
}
int OOPMPIStorageBuffer::Send (int target)
{
#ifdef DEBUGALL
{
#ifdef LOGPZ    
    stringstream sout;
    sout << "PID" << getpid() << " Called MPI_Send ret = ";
    LOGPZ_DEBUG(logger,sout.str()):
#endif    
}
#endif
if(f_send_position >= MAXSIZE)
{
#ifdef LOGPZ    
    std::stringstream st;
    st << __PRETTY_FUNCTION__ << " Sending a message of size " << f_send_position << " maxsize = " << MAXSIZE << " FATAL THINGS WILL HAPPEN ";
    LOGPZ_ERROR(logger,st.str());    
    std::cout << st.str() << endl;
#endif
}
int ret;
int tag = 0;
ret = MPI_Send (&f_send_buffr[0], f_send_position, MPI_PACKED,
				target, tag, MPI_COMM_WORLD);
#	ifdef OOP_MPE
//	MPE_Log_send(target, tag, f_send_position);
#	endif	

#ifdef DEBUGALL
switch(ret){
	case MPI_SUCCESS:
#ifdef LOGPZ      
		stringstream sout;
		sout <<" - No error; MPI routine completed successfully";
		LOGPZ_ERROR(logger,sout.str()):
#endif      
			break;
	case MPI_ERR_COMM:
#ifdef LOGPZ      
		stringstream sout;
		sout << "-  Invalid communicator.  A common error is to use a null communicator in a call (not even allowed in MPI_Comm_rank ).";
		LOGPZ_ERROR(logger,sout.str()):
#endif      
			break;
	case MPI_ERR_COUNT:
#ifdef LOGPZ      
		stringstream sout;
		sout << "- Invalid count argument.  Count arguments must be non-negative a count of zero is often valid";
		LOGPZ_ERROR(logger,sout.str()):
#endif      
			break;
	case MPI_ERR_TYPE:
#ifdef LOGPZ      
		stringstream sout;
		sout << "- Invalid datatype argument.  May be an uncommitted MPI_Datatype (see MPI_Type_commit ).";      
		LOGPZ_ERROR(logger,sout.str()):
#endif      
			break;
	case MPI_ERR_TAG:
#ifdef LOGPZ      
		stringstream sout;
		sout << "- Invalid tag argument.  Tags must be non-negative;  tags  in  a\n"
            << "receive  (  MPI_Recv , MPI_Irecv , MPI_Sendrecv , etc.) may also\n"
            << "be MPI_ANY_TAG .  The largest tag value is available through the\n"
            << "the attribute MPI_TAG_UB .";
		LOGPZ_ERROR(logger,sout.str()):
#endif      
			break;
	case MPI_ERR_RANK:
#ifdef LOGPZ      
		sout << "-  Invalid  source  or  destination rank.";
		LOGPZ_ERROR(logger,sout.str()):
#endif      
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
	int result = PkInt(&len,1);
	if(len)	return PkByte(p,len);
	return result;
	
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
void OOPMPIStorageBuffer::CancelRequest(){
    if(f_isreceiving) MPI_Cancel(&f_request);
}
int OOPMPIStorageBuffer::Receive ()
{
    
    // nonblocking!!!!
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
	ret_test=PMPI_Test (&f_request, &test_flag, &status);
#ifdef DEBUG
	/*	cout << "Test returned " << ret_test << endl;
	cout << "Flag " << test_flag << endl;
	cout.flush();*/
#endif	
	return test_flag;
}
/**
* Restores next object in the buffer
 */
TPZSaveable *OOPMPIStorageBuffer::Restore () {
	if(!TestReceive()) {
		LOGPZ_WARN(logger,"Restore called at the wrong moment\n");
		return NULL;
	}
	f_isreceiving = 0;
	f_recv_position = 0;
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TPZSaveable *obj = TPZSaveable::Restore(*this, 0);
	{
		stringstream sout;
		sout << __PRETTY_FUNCTION__ << "Proc " << CM->GetProcID() << " Restored object with classid " << obj->ClassId();
		LOGPZ_DEBUG(logger,sout.str().c_str());
	}
	//MPI_Request_free(&f_request);
	return obj;
}

int OOPMPIStorageBuffer::ReceiveBlocking ()
{
	Receive();
	if(TestReceive()) {
		return 1;
		
	}
	
	MPI_Status status;
	//Isso aqui
	MPI_Wait(&f_request,&status);
	return 1;
	
	//MPI_Status status;
	// recebe primeiros 10^6 bytes
	/*
	 cout << "MPI_Recv returned " << 
	 MPI_Recv (&f_recv_buffr[0], f_recv_buffr.NElements(), MPI_PACKED, MPI_ANY_SOURCE,
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
	int nbytes;
	MPI_Pack_size(n,MPI_CHAR,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_CHAR,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(char);
	return 1;
}
int OOPMPIStorageBuffer::UpkInt (int *p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_INT,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_INT,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(int);
	return 1;
}
int OOPMPIStorageBuffer::UpkShort (short *p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_SHORT,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_SHORT,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(short);
	return 1;
}
int OOPMPIStorageBuffer::UpkLong (long *p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_LONG,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_LONG,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(long);
	return 1;
}
int OOPMPIStorageBuffer::UpkUint (u_int * p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_UNSIGNED,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_UNSIGNED,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(u_int);
	return 1;
}
int OOPMPIStorageBuffer::UpkUshort (u_short * p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_UNSIGNED_SHORT,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_UNSIGNED_SHORT,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(u_short);
	return 1;
}
int OOPMPIStorageBuffer::UpkUlong (u_long * p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_UNSIGNED_LONG,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_UNSIGNED_LONG,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(u_long);
	return 1;
}
int OOPMPIStorageBuffer::UpkFloat (float *p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_FLOAT,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_FLOAT,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(float);
	return 1;
}
int OOPMPIStorageBuffer::UpkDouble (double *p, int n)
{
	int nbytes;
	MPI_Pack_size(n,MPI_DOUBLE,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, p, n, MPI_DOUBLE,
				MPI_COMM_WORLD);
	// f_recv_position=f_recv_position+n*sizeof(double);
	return 1;
}
//essa eh meio complicada...
int OOPMPIStorageBuffer::UpkStr (char *p)
{
	int n;
	int nbytes;
	MPI_Pack_size(n,MPI_INT,MPI_COMM_WORLD,&nbytes);
	f_BytesTransmitted += nbytes;
	MPI_Unpack (&f_recv_buffr[0], f_recv_buffr.NElements(), &f_recv_position, &n, 1,
			    MPI_INT, MPI_COMM_WORLD);
	p[0] = '\0';
	if(n){
	    UpkByte(p,n);
	}
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
	int i;
	for(i=0; i<size; i++)
	{
		int locsize;
		locsize = p[i].length() > 0 ? p[i].length() : 1;
		
		char* buf = new char[locsize+1];
		buf[0] = '\0';
		p[i].copy(buf, p[i].length());
		PkStr(buf);
		delete []buf;
	}
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
	if(!size) return;
	int i=0;
	for (i=0;i<size;i++)
	{
		char buf[2000];
		UpkStr(buf);
		p[i] = buf;
    }
}


/*!
\fn OOPMPIStorageBuffer::ResetByteCounter()
 */
void OOPMPIStorageBuffer::ResetByteCounter()
{
    f_BytesTransmitted = 0;
}



long OOPMPIStorageBuffer::GetBytesTransmitted() const
{
	return f_BytesTransmitted;
}
