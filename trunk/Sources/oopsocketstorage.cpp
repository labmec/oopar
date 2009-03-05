//
// C++ Implementation: oopsocketstorage
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopsocketstorage.h"
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
static LoggerPtr logger(Logger::getLogger("OOPar.OOPSocketStorageBuffer"));
#endif


int OOPSocketStorageBuffer::Pack(void *ptr, int n, int dtype)
{
  int nbytes;
  try
  {
      OOPSocket::Pack_size(n, dtype, &nbytes);
  }
  catch(const exception& e)
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "OOPSocketStorage: Exception catched on SOCKET.Pack_size! " << e.what();
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    std::cout << "OOPSocketStorage: Exception catched! " << e.what();
    exit(-1);
  }
  m_Buffer.Resize(m_Length+nbytes);
  int ret;
  try
  {
      ret = OOPSocket::Pack(ptr, n, dtype, &m_Buffer[0], m_Buffer.NElements(), &m_Length);
  }
  catch(const exception& e)
  {
#ifdef LOGPZ
    stringstream sout;
    sout << "OOPSocketStorage: Exception catched on SOCKET.Pack! " << e.what();
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
    std::cout << "OOPSocketStorage: Exception catched! " << e.what();
    exit(-1);
  }
  return ret;
}


OOPSocketStorageBuffer::OOPSocketStorageBuffer()
 : m_Buffer(1)
{
  m_Buffer.Resize(0);
  m_Length = 0;
}


OOPSocketStorageBuffer::~OOPSocketStorageBuffer()
{
}


void OOPSocketStorageBuffer::Write(int *p, int size)
{
  PkInt(p, size);
}


void OOPSocketStorageBuffer::Write(double *p, int size)
{
  PkDouble(p, size);
}


void OOPSocketStorageBuffer::Write(const char *p, int size)
{
  PkByte(p, size);
}


void OOPSocketStorageBuffer::Write(std::string *p, int size)
{
  int i;
  for(i=0; i<size; i++)
  {
    int locsize;
    locsize = p[i].length() > 0 ? p[i].length() : 1;

    char* buf = new char[locsize+1];
    buf[0] = '\0';
    p[i].copy(buf, p[i].length());
    PkString(buf);
    delete []buf;
  }
}


void OOPSocketStorageBuffer::Read(int *p, int size)
{
  UpkInt(p, size);
}


void OOPSocketStorageBuffer::Read(double *p, int size)
{
  UpkDouble(p, size);
}


void OOPSocketStorageBuffer::Read(char *p, int size)
{
  UpkByte(p, size);
}


void OOPSocketStorageBuffer::Read(std::string *p, int size)
{
  if(!size) return;
  int i=0;
  for (i=0;i<size;i++)
  {
    char buf[2000];
    UpkString(buf);
    p[i] = buf;
  }
}


TPZSaveable *OOPSocketStorageBuffer::Restore ()
{
  m_Length = 0;
  TPZSaveable *obj = TPZSaveable::Restore(*this, 0);
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "OOPSocketStorage: "<< __PRETTY_FUNCTION__ << " Proc " << CM->GetProcID() << " Restored object with classid: ";
    if (obj) sout << obj->ClassId();
    LOGPZ_DEBUG(logger,sout.str().c_str());
  }
#endif
  return obj;
}


int OOPSocketStorageBuffer::PkInt(int *p, int n)
{
  return Pack(p, n, SOCKET_INT);
}


int OOPSocketStorageBuffer::PkDouble(double *p, int n)
{
  return Pack(p, n, SOCKET_DOUBLE);
}


int OOPSocketStorageBuffer::PkByte(const char *p, int n)
{
  return Pack((char *)p, n, SOCKET_CHAR);
}


int OOPSocketStorageBuffer::PkString(char *p)
{
  int len = strlen(p);
  int result = PkInt(&len,1);
  if(len) return PkByte(p,len);
  return result;

}


int OOPSocketStorageBuffer::UpkInt (int *p, int n)
{
  OOPSocket::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_INT);
  return 1;
}


int OOPSocketStorageBuffer::UpkDouble (double *p, int n)
{
  OOPSocket::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_DOUBLE);
  return 1;
}


int OOPSocketStorageBuffer::UpkByte (char *p, int n)
{
  OOPSocket::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_CHAR);
  return 1;
}


int OOPSocketStorageBuffer::UpkString (char *p)
{
  int n;
  OOPSocket::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, &n, 1, SOCKET_INT);
  p[0] = '\0';
  if(n)
  {
    UpkByte(p,n);
  }
  return 1;
}

