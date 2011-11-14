/***************************************************************************
                          BiCGInt.cpp  -  description
                             -------------------
    begin                : Thu Feb 12 2004
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
#include "BiCGInt.h"

TBiCGInt::TBiCGInt() : TPZSaveable(){}

int TBiCGInt::Write(OOPStorageBuffer * buf,int classid){
  TPZSaveable::Write(buf,classid);
  buf->PkInt(&value);
  return 1;
}

int TBiCGInt::Read(OOPStorageBuffer * buf,void *context){
  TPZSaveable::Read(buf,context);
  int aux;
  buf->UpkInt(&aux);
  value = aux;
  return 1;
}
