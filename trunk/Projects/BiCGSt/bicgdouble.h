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

#include <bicgdefs.h>
#include <oopsaveable.h>


/**Implements the transmitable double variable
  *@author longhin
  */

class BiCGDouble : public OOPSaveable {
public:
  double value;
	BiCGDouble();
	~BiCGDouble();
  long GetClassID();

  int Pack (OOPSendStorage * buf);
	int Unpack (OOPReceiveStorage * buf);
	static OOPSaveable *Restore (OOPReceiveStorage * buf);

};

#endif
