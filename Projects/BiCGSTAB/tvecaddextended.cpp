/***************************************************************************
                          tvecaddextended.cpp  -  description
                             -------------------
    begin                : Thu Jan 22 2004
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

#include "tvecaddextended.h"

TVecAddExtended::TVecAddExtended(){
}
TVecAddExtended::~TVecAddExtended(){
}
OOPMReturnType TVecAdd::Execute (){
  double value = - this->fDataDepend->Dep(2).ObjPtr() * this->fDataDepend->Dep(3).ObjPtr();
  this->fDataDepend->Dep(0).ObjPtr() = value * this->fDataDepend->Dep(1).ObjPtr();
  this->fDataDepend->Dep(0).ObjPtr().Version().Increment();
}