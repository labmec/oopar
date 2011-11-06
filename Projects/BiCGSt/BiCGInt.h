/***************************************************************************
                          BiCGInt.h  -  description
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
#include "bicgdefs.h"

class TBiCGInt : public TPZSaveable{
public:
  TBiCGInt();
  int value;
  long GetClassID(){
    return BICGINT_ID;
  }

  int Write(OOPStorageBuffer * buf,int classid);
	int Read(OOPStorageBuffer * buf,void *context);
  
};

