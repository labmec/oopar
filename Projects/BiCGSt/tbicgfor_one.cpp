/***************************************************************************
                          tbicgfor_one.cpp  -  description
                             -------------------
    begin                : Mon Feb 16 2004
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

#include "tbicgfor_one.h"

TBiCGFor_One::TBiCGFor_One(int proc) : OOPTask(proc) {
}
TBiCGFor_One::~TBiCGFor_One(){
}
long TBiCGFor_One::ExecTime (){
      return -1;
}

OOPMReturnType TBiCGFor_One::Execute (){
      #warning "Execute Method not implemented"
      return ESuccess;
}
