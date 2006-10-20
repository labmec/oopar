/***************************************************************************
                          tcomputebeta.cpp  -  description
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

#include "tcomputebeta.h"

TComputeBeta::TComputeBeta(){
}
TComputeBeta::~TComputeBeta(){
}
void TComputeBeta::SetId_alpha(OOPObjectId & Id){
  fId_alpha = Id;
}

void TComputeBeta::SetId_beta(OOPObjectId & Id){
  fId_beta = Id;
}
void TComputeBeta::SetId_rho_1(OOPObjectId & Id){
  fId_rho_1 = Id;
}  
void TComputeBeta::SetId_rho_2(OOPObjectId & Id){
  fId_rho_2 = Id;
}
