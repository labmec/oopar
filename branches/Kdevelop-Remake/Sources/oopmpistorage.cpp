

#include "oopmpistorage.h"
#include "oopcommmanager.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>


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

#ifndef OOP_MPE
#define PMPI_Pack_size MPI_Pack_size
#define PMPI_Pack MPI_Pack
#define PMPI_Send MPI_Send
#define PMPI_Test MPI_Test
#define PMPI_Probe MPI_Probe
#define PMPI_Unpack MPI_Unpack
#define PMPI_Get_count MPI_Get_count
#endif



void OOPMPIStorageBuffer::ExpandBuffer(int more_dimension)
{
  m_Buffer.Resize(m_Buffer.NElements()+more_dimension);
  if (more_dimension < 0)
  {
    stringstream sout;
    sout << "Expanding buffer to invalid size " << more_dimension;
    cout << sout.str().c_str() << endl;
#ifdef LOGPZ
    LOGPZ_DEBUG(logger,sout.str().c_str());
#endif

  }
}
int OOPMPIStorageBuffer::ResetBuffer (int size)
{
  m_Length = 0;
  m_Buffer.Resize(size);
  return 1;
}
int OOPMPIStorageBuffer::PackGeneric (void *ptr, int n, MPI_Datatype mpitype)
{
  int nbytes;
  PMPI_Pack_size(n,mpitype,MPI_COMM_WORLD,&nbytes);
  m_Buffer.Resize(m_Length+nbytes);
  int mpiret;
  mpiret = PMPI_Pack (ptr, n, mpitype, &m_Buffer[0], m_Buffer.NElements(), &m_Length,
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
    LOGPZ_DEBUG(logger,sout.str().c_str()):
#endif
      }
#endif
  if(m_Length >= MAXSIZE)
{
#ifdef LOGPZ
    std::stringstream st;
    st << __PRETTY_FUNCTION__ << " Sending a message of size " <<
      m_Length << " maxsize = " << MAXSIZE << " FATAL THINGS WILL HAPPEN ";
    LOGPZ_ERROR(logger,st.str());
    std::cout << st.str() << endl;
#endif

  }
  int ret;
  int tag = 0;
  ret = MPI_Send (&m_Buffer[0], m_Length, MPI_PACKED,
                  target, tag, MPI_COMM_WORLD);

#ifdef DEBUGALL
  switch(ret)
  {
      case MPI_SUCCESS:

#ifdef LOGPZ
      stringstream sout;
      sout <<" - No error; MPI routine completed successfully";
      LOGPZ_ERROR(logger,sout.str().c_str()):
#endif
          break;
      case MPI_ERR_COMM:
#ifdef LOGPZ
      stringstream sout;
      sout << "-  Invalid communicator.  A common error is to use a null communicator in a call (not even allowed in MPI_Comm_rank ).";
      LOGPZ_ERROR(logger,sout.str().c_str()):
#endif
          break;
      case MPI_ERR_COUNT:
#ifdef LOGPZ
      stringstream sout;
      sout << "- Invalid count argument.  Count arguments must be non-negative a count of zero is often valid";
      LOGPZ_ERROR(logger,sout.str().c_str()):
#endif
          break;
      case MPI_ERR_TYPE:
#ifdef LOGPZ
      stringstream sout;
      sout << "- Invalid datatype argument.  May be an uncommitted MPI_Datatype (see MPI_Type_commit ).";
      LOGPZ_ERROR(logger,sout.str().c_str()):
#endif
          break;
      case MPI_ERR_TAG:
#ifdef LOGPZ
      stringstream sout;
      sout << "- Invalid tag argument.  Tags must be non-negative;  tags  in  a\n"
      << "receive  (  MPI_Recv , MPI_Irecv , MPI_Sendrecv , etc.) may also\n"
      << "be MPI_ANY_TAG .  The largest tag value is available through the\n"
      << "the attribute MPI_TAG_UB .";
      LOGPZ_ERROR(logger,sout.str().c_str()):
#endif
          break;
      case MPI_ERR_RANK:
#ifdef LOGPZ
      sout << "-  Invalid  source  or  destination rank.";
      LOGPZ_ERROR(logger,sout.str().c_str()):
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
  //if(m_Request) MPI_Request_free(&m_Request);
}
void OOPMPIStorageBuffer::CancelRequest()
{
#ifndef BLOCKING
  if(m_IsReceiving) MPI_Cancel(&m_Request);
#endif
}
int OOPMPIStorageBuffer::Receive ()
{

  // nonblocking!!!!

  if(m_IsReceiving) return 1;

  MPI_Irecv (&m_Buffer[0], m_Buffer.NElements(), MPI_PACKED, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &m_Request);


  m_IsReceiving = 1;
  return 1;

  //Blocking
  // 	 MPI_Status status;
  // 	 MPI_Recv(&m_Buffer[0], m_Buffer.NElements(), MPI_PACKED, MPI_ANY_SOURCE,
  // 			   MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  //
  //          return 1;

}

bool OOPMPIStorageBuffer::TestReceive()
{
  if(!m_IsReceiving) return false;
  MPI_Status status;
  int test_flag, ret_test;
  //#ifdef OOP_MPE
  ret_test = PMPI_Test (&m_Request, &test_flag, &status);
  //Checks if test_flag is true and if source is something valid
  if(test_flag && status.MPI_SOURCE >=0 )
  {
    m_Status = status;
  }
  //ret_test=MPI_Test (&m_Request, &test_flag, &status);
  /*#else
  	ret_test=MPI_Test (&m_Request, &test_flag, &status);
  #endif*/
  return test_flag;
}
/**
* Restores next object in the buffer
 */
TPZSaveable *OOPMPIStorageBuffer::Restore ()
{
#ifndef BLOCKING
  if(!TestReceive())
  {
    LOGPZ_WARN(logger,"Restore called at the wrong moment\n");
    return NULL;
  }
#endif
  m_IsReceiving = 0;
  m_Length = 0;
  TPZSaveable *obj = TPZSaveable::Restore(*this, 0);
#ifdef LOGPZ
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << "Proc " << CM->GetProcID() << " Restored object with classid ";
    if (obj) sout << obj->ClassId();
    LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  return obj;
}

int OOPMPIStorageBuffer::ReceiveBlocking ()
{
  MPI_Status status;
  int probres=-1;
  probres = PMPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD, &status);
  int count = -1;
  PMPI_Get_count(&status, MPI_PACKED , &count);
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Receiving " << count << " bytes !!" << " probres = " << probres ;
    LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  int res = -1;
  if (count)
  {
    m_Buffer.Resize(count);
  }
  else
  {
    m_Buffer.Resize(1);
  }
  res = MPI_Recv (&m_Buffer[0], count , MPI_PACKED, MPI_ANY_SOURCE,
                   MPI_ANY_TAG, MPI_COMM_WORLD, &status);// << endl;
  if(res == MPI_SUCCESS)
  {
    return 1;
  }
  else
  {
    cout << "Falhou Recv\n";
    return -1;
  }
}
// Metodos para DESEMPACOTAR dados do buffer.
// p : Ponteiro para o buffer onde os dados serao lidos.
// n : Numero de elementos a serem lidos (default: um unico dado).
int OOPMPIStorageBuffer::UpkByte (char *p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_CHAR,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkInt (int *p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_INT,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkShort (short *p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_SHORT,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkLong (long *p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_LONG,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkUint (u_int * p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_UNSIGNED,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkUshort (u_short * p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_UNSIGNED_SHORT,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkUlong (u_long * p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_UNSIGNED_LONG,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkFloat (float *p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_FLOAT,
               MPI_COMM_WORLD);
  return 1;
}
int OOPMPIStorageBuffer::UpkDouble (double *p, int n)
{
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, MPI_DOUBLE,
               MPI_COMM_WORLD);
  return 1;
}

int OOPMPIStorageBuffer::UpkStr (char *p)
{
  int n;
  PMPI_Unpack (&m_Buffer[0], m_Buffer.NElements(), &m_Length, &n, 1,
               MPI_INT, MPI_COMM_WORLD);
  p[0] = '\0';
  if(n)
  {
    UpkByte(p,n);
  }
  return 1;
}
void OOPMPIStorageBuffer::Write(int *p, int size)
{
  PkInt(p, size);
}
void OOPMPIStorageBuffer::Write(double *p, int size)
{
  PkDouble(p, size);
}
void OOPMPIStorageBuffer::Write(const char *p, int size)
{
  PkByte(p, size);
}
void OOPMPIStorageBuffer::Write(string *p, int size)
{
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
void OOPMPIStorageBuffer::Read(int *p, int size)
{
  UpkInt(p, size);
}
void OOPMPIStorageBuffer::Read(double *p, int size)
{
  UpkDouble(p, size);
}
void OOPMPIStorageBuffer::Read(char *p, int size)
{
  UpkByte(p, size);
}
void OOPMPIStorageBuffer::Read(string *p, int size)
{
  if(!size) return;
  int i=0;
  for (i=0;i<size;i++)
  {
    char buf[2000];
    UpkStr(buf);
    p[i] = buf;
  }
}
