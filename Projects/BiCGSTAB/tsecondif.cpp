/***************************************************************************
                          tsecondif.cpp  -  description
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

#include "tsecondif.h"

TSecondIf::TSecondIf(){
}
TSecondIf::~TSecondIf(){
}
OOPMReturnType TSecondIf::Execute (){
  /**
  0 - Id_max_iter
  1 - Id_beta
  2 - Id_rho_1
  3 - Id_rho_2
  4 - Id_alpha
  5 - Id_omega
  6 < nproc lId_p ..lId_r ..lId_v
  */
  int i, nproc;
  nproc = CM->NumProcessors();
  OOPDataVersion ver = fDataDepend->Dep(0)->ObjPtr().Version();
  int iver = ver.GetLevelVersion(ver.GetNLevels()-1);
  if (iver = 1){
    //p = r;
    OOPDataVersion version = fDataDepend->Dep(6)->ObjPtr().Version();
    TSetVecToVec * upvec;
    for(i=0;i<nproc;i++){
      upvec = new TSetVecToVec(i);
      upvec->AddDependentData(OOPMetaData(this->fDataDepend->Dep(6+3*(i+1)-3), EWrite, version));\\p
      upvec->AddDependentData(OOPMetaData(this->fDataDepend->Dep(6+3*(i+1)-2), EWrite, version));\\r
      upvec->Submit();
    }
    //Incrementar a versao do beta e dos ps ?
  }else{
    //beta = (rho_1/rho_2) * (alpha/omega); //TComputeBeta
    TComputeBeta * cbeta = new TComputeBeta(0);
    cbeta->AddDependentData(OOPMDataDepend(fDataDepend->Dep(1),EWrite,fDataDepend->Dep(1).Version());
    cbeta->AddDependentData(OOPMDataDepend(fDataDepend->Dep(2),ERead,fDataDepend->Dep(2).Version());
    cbeta->AddDependentData(OOPMDataDepend(fDataDepend->Dep(3),ERead,fDataDepend->Dep(3).Version());
    cbeta->AddDependentData(OOPMDataDepend(fDataDepend->Dep(4),ERead,fDataDepend->Dep(4).Version());
    cbeta->AddDependentData(OOPMDataDepend(fDataDepend->Dep(5),ERead,fDataDepend->Dep(5).Version());
    cbeta->Submit();

    
    //p *= beta; //p.Add(beta,1)
    TUpdateP * updatep;
    OOPDataVersion betaver = fDataDepend->Dep(1).Version()
    betaver.Increment();
    for(i=0;i<nproc;i++){
      updatep = new TUpdateP(i);
      updatep->AddDependentData(OOPMDataDepend(this->fDataDepend->Dep(6+3*(i+1)-3), EWrite, version));
      updatep->AddDependentData(OOPMDataDepend(fDataDepend->Dep(1),ERead,betaver);
      updatep->Submit();
    }
    
    //p.Add(1., r);
    OOPDataVersion pversion = this->fDataDepend->Dep(6).Version();
    pversion.Increment();
    TVecAdd * vecadd;
    for(i=0;i<nproc;i++){
      vecadd = new TVecAdd(i);
      vecadd->AddDependentData(OOPMDataDepend(this->fDataDepend->Dep(6+3*(i+1)-3), EWrite, version));
      vecadd->AddDependentData(OOPMDataDepend(this->fDataDepend->Dep(6+3*(i+1)-2), ERead, version));
      vecadd->SetValue(1);
      vecadd->Submit();
    }
    
    //p.Add(- beta * omega, v);
    
  }
  
}