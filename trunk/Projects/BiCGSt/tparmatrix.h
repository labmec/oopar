/***************************************************************************
                          tparmatrix.h  -  description
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

#ifndef TPARMATRIX_H
#define TPARMATRIX_H

//#include <oopsaveable.h>
#include "bicgdefs.h"

/**Implements a matrix which can be trasnmitted through the network.
  *@author longhin
  */

class TParMatrix : public TPZSaveable  {
public: 
	TParMatrix();
	~TParMatrix();
  long GetClassID(){
    return TPARMATRIX_ID;
  }

  int Pack (OOPSendStorage * buf);
	int Unpack (OOPReceiveStorage * buf);

};

#endif
