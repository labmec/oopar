/***************************************************************************
                          bicgdouble.h  -  description
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

#ifndef BICGDOUBLE_H
#define BICGDOUBLE_H

#include "bicgdefs.h"
#include "oopstorage.h"
#include "pzsave.h"


/**Implements the transmitable double variable
  *@author longhin
  */

class BiCGDouble : public TPZSaveable {
public:
  double value;
	BiCGDouble();
	~BiCGDouble();
  long GetClassID();

  int Write(OOPStorageBuffer * buf,int classid);
	int Read(OOPStorageBuffer * buf,void * context);
	static TPZSaveable *Restore (OOPStorageBuffer * buf);

};

#endif
