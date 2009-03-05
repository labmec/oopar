//
// C++ Interface: oopsocketstorage
//
// Description:
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPSOCKETSTORAGE_H
#define OOPSOCKETSTORAGE_H

#include "pzmanvector.h"
#include "pzfilebuffer.h"
#include "pzsave.h"

#include "oopsocket.h"
//128
const int MAXSIZE = 256;

/**
	@author Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>
*/
class OOPSocketStorageBuffer : public TPZStream
{
private:

    TPZManVector<char,MAXSIZE> m_Buffer;

    int m_Length;

    int Pack(void *ptr, int n, int dtype);

public:
    OOPSocketStorageBuffer();

    ~OOPSocketStorageBuffer();

    // Heranca

    void Write(int *p, int size=1);

    void Write(double *p, int size=1);

    void Write(const char *p, int size=1);

    void Write(std::string *p, int size=1);

    void Read(int *p, int size=1);

    void Read(double *p, int size=1);

    void Read(char *p, int size=1);

    void Read(std::string *p, int size=1);

    // Outros

    virtual TPZSaveable *Restore ();

    int PkInt (int *p, int n = 1);

    int PkDouble (double *p, int n = 1);

    int PkByte (const char *p, int n = 1);

    int PkString (char *p);

    int UpkInt (int *p, int n = 1);

    int UpkDouble (double *p, int n = 1);

    int UpkByte (char *p, int n = 1);

    int UpkString (char *p);

    char& operator[]( const int index ) const
    {
        return m_Buffer[ index ];
    }

    void Resize(int size)
    {
        m_Buffer.Resize(size);
    }

    int Size()
    {
        return m_Length;
    }

};

#endif
