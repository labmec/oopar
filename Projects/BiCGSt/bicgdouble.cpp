/***************************************************************************
                          bicgdouble.cpp  -  description
                             -------------------
    begin                : Wed Feb 11 2004
    copyright            : (C) 2004 by longhin
    email                : longhin@carlsberg.fec.unicamp.br
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bicgdouble.h"

TPZSaveable * BiCGDouble::Restore (OOPStorageBuffer * buf) {
	BiCGDouble *bd = new BiCGDouble;
	void *context;
	bd->Read(buf,context);
	return bd;
}

BiCGDouble::BiCGDouble() : TPZSaveable() {
}
BiCGDouble::~BiCGDouble(){
}
int BiCGDouble::Write(OOPStorageBuffer * buf, int classid)
{
  TPZSaveable::Write(*buf,classid);
  buf->PkDouble(&value);
	return 1;
}
/**************/
/*** Unpack ***/
int BiCGDouble::Read(OOPStorageBuffer * buf,void * context)
{
  TPZSaveable::Read(*buf,context);
  double aux;
  buf->UpkDouble(&aux);
  value=aux;  
	return 1;
}
long BiCGDouble::GetClassID ()
{
	return BICGDOUBLE_ID;
}
