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
#include <oopsaveable.h>
#include <bicgdefs.h>
class TBiCGInt : public OOPSaveable{
public:
  TBiCGInt();
  int value;
  long GetClassID(){
    return BICGINT_ID;
  }

  int Pack (OOPSendStorage * buf);
	int Unpack (OOPReceiveStorage * buf);
  
};

