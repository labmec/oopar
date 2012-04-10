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
//#include "oopsocket.h"

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <typeinfo>
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
		Pack_size(n, dtype, &nbytes);
	}
	catch(const std::exception& e)
	{
#ifdef LOG4CXX
		std::stringstream sout;
		sout << "OOPSocketStorage: Exception catched on SOCKET.Pack_size! " << e.what();
		LOGPZ_ERROR(logger,sout.str())
#endif
		std::cout << "OOPSocketStorage: Exception catched! " << e.what();
		exit(-1);
	}
	m_Buffer.Resize(m_Length+nbytes);
	int ret;
	try
	{
		ret = Pack(ptr, n, dtype, &m_Buffer[0], m_Buffer.NElements(), &m_Length);
	}
	catch(const std::exception& e)
	{
#ifdef LOG4CXX
		std::stringstream sout;
		sout << "OOPSocketStorage: Exception catched on SOCKET.Pack! " << e.what();
		LOGPZ_ERROR(logger,sout.str())
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

void OOPSocketStorageBuffer::Write(long double *p, int size)
{
    DebugStop();
//	PkDouble(p, size);
}

void OOPSocketStorageBuffer::Write(float *p, int size)
{
	PkFloat(p, size);
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

void OOPSocketStorageBuffer::Read(long double *p, int size)
{
    DebugStop();
//	UpkDouble(p, size);
}

void OOPSocketStorageBuffer::Read(float *p, int size)
{
	UpkFloat(p, size);
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
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "OOPSocketStorage: "<< __PRETTY_FUNCTION__  << " Restored object with classid: ";
		if (obj) sout << obj->ClassId();
		LOGPZ_DEBUG(logger,sout.str())
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

int OOPSocketStorageBuffer::PkFloat(float *p, int n)
{
	return Pack(p, n, SOCKET_FLOAT);
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
	OOPSocketStorageBuffer::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_INT);
	return 1;
}


int OOPSocketStorageBuffer::UpkDouble (double *p, int n)
{
	OOPSocketStorageBuffer::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_DOUBLE);
	return 1;
}

int OOPSocketStorageBuffer::UpkFloat (float *p, int n)
{
	OOPSocketStorageBuffer::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_FLOAT);
	return 1;
}


int OOPSocketStorageBuffer::UpkByte (char *p, int n)
{
	OOPSocketStorageBuffer::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, p, n, SOCKET_CHAR);
	return 1;
}


int OOPSocketStorageBuffer::UpkString (char *p)
{
	int n;
	OOPSocketStorageBuffer::Unpack(&m_Buffer[0], m_Buffer.NElements(), &m_Length, &n, 1, SOCKET_INT);
	p[0] = '\0';
	if(n)
	{
		UpkByte(p,n);
	}
	return 1;
}

int OOPSocketStorageBuffer::Pack_size(int incount, int dtype, int *size)
{
    int datasize;
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*incount;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*incount;;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*incount;
            break;
        case SOCKET_PACKED:
            datasize=incount;
            break;
        default:
            printf("SOCKETSTORAGE.Pack_size: Undefined SOCKET type: %d\n",dtype); fflush(stdout);
            return SOCKET_ERR_TYPE;
    }
    (*size)=datasize;
    return SOCKET_SUCCESS;
}


int OOPSocketStorageBuffer::Pack(void *inb, int insize, int dtype, void *outb, int outsize, int *pos)
{
    int datasize;
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*insize;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*insize;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*insize;
            break;
        case SOCKET_PACKED:
            datasize=insize;
            break;
        default:
            printf("SOCKET.Pack: Undefined SOCKET type: %d\n",dtype); fflush(stdout);
            return SOCKET_ERR_TYPE;
    }
    if((*pos)+datasize > outsize)
    {
        printf("SOCKET.Pack: Outbuffer size out of bounds: %d\n",outsize); fflush(stdout);
        return SOCKET_ERR_COUNT;
    }
    memcpy(((char*)outb)+(*pos),inb,datasize);
    (*pos) = (*pos) + datasize;
    return SOCKET_SUCCESS;
}


int OOPSocketStorageBuffer::Unpack(void *inb, int insize, int *pos, void *outb, int outsize, int dtype)
{
    int datasize;
    switch(dtype)
    {
        case SOCKET_CHAR:
            datasize=sizeof(char)*outsize;
            break;
        case SOCKET_INT:
            datasize=sizeof(int)*outsize;
            break;
        case SOCKET_DOUBLE:
            datasize=sizeof(double)*outsize;
            break;
        case SOCKET_PACKED:
            datasize=outsize;
            break;
        default:
            printf("SOCKET.Unpack: Undefined SOCKET type: %d\n",dtype); fflush(stdout);
            return SOCKET_ERR_TYPE;
    }
    if((*pos)+datasize > insize)
    {
        printf("SOCKET.Unpack: Inbuffer size out of bounds: %d+%d=%d?\n",*pos,datasize,insize); fflush(stdout);
        return SOCKET_ERR_COUNT;
    }
    memcpy(outb,((char*)inb)+(*pos),datasize);
    (*pos) = (*pos) + datasize;
    return SOCKET_SUCCESS;
}
