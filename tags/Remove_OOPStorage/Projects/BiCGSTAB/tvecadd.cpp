/***************************************************************************
                          tvecadd.cpp  -  description
                             -------------------
    begin                : Wed Jan 21 2004
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

#include "tvecadd.h"

TVecAdd::TVecAdd(){
  fValue = 0.;
}
TVecAdd::~TVecAdd(){
}
OOPMReturnType TVecAdd::Execute (){
  if(!fValue){
    fValue = this->fDataDepend->Dep(2).ObjPtr();
  }
  this->fDataDepend->Dep(0).ObjPtr() = fValue * this->fDataDepend->Dep(1).ObjPtr();
  this->fDataDepend->Dep(0).ObjPtr().Version().Increment();
}