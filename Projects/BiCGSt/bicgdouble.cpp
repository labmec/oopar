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

BiCGDouble::BiCGDouble() : OOPSaveable() {
}
BiCGDouble::~BiCGDouble(){
}
int BiCGDouble::Pack (OOPSendStorage * buf)
{
  OOPSaveable::Pack(buf);
  buf->PkDouble(&value);
	return 1;
}
/**************/
/*** Unpack ***/
int BiCGDouble::Unpack (OOPReceiveStorage * buf)
{
  OOPSaveable::Unpack(buf);
  double aux;
  buf->UpkDouble(&aux);
  value=aux;  
	return 1;
}
long BiCGDouble::GetClassID ()
{
	return BICGDOUBLE_ID;
}
