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
#include "bicgdouble.h"
#include <oopmetadata.h>
#include <BiCGInt.h>
#include <TParVector.h>
#include <tparmatrix.h>


OOPSaveable * TBiCGFor_One::Restore(OOPReceiveStorage * buf){
      #warning "Shouldn't the new object be created on the CM->ProcID() processor ?"
      TBiCGFor_One * tbfor = new TBiCGFor_One(0);
      tbfor->Unpack(buf);
      return tbfor;
}
TBiCGFor_One::TBiCGFor_One(int proc) : OOPTask(proc) {
}
TBiCGFor_One::~TBiCGFor_One(){
}
long TBiCGFor_One::ExecTime (){
      return -1;
}

OOPMReturnType TBiCGFor_One::Execute (){
      /**Implements the first set of instructions from the for loop found in the BiCGStab method. Implements the following tasks:
    if (rho_1 == 0) {
      tol = Norm(r) / normb;
      return 2;
    }
    if (i == 1)
      p = r;
    else {
      //beta(0) = (rho_1(0)/rho_2(0)) * (alpha(0)/omega(0));
      beta = (rho_1/rho_2) * (alpha/omega); //TComputeBeta
      //p = r + beta(0) * (p - omega(0) * v);
      p *= beta; //p.Add(beta,1)
      p.Add(1., r);
      p.Add(- beta * omega, v);
    }
    M.Solve(p, phat);

       bicgforone->AddDependentData(OOPMDataDepend(fId_rho_1,EReadAccess,rho_version));
       bicgforone->AddDependentData(OOPMDataDepend(fId_tol,EReadAccess,tolVersion));
       bicgforone->AddDependentData(OOPMDataDepend(fId_normr,EReadAccess,normr_ver));
       bicgforone->AddDependentData(OOPMDataDepend(f_lId_r[i],EReadAccess,r_Version));
       bicgforone->AddDependentData(OOPMDataDepend(fId_normb,EReadAccess,normb_ver));
       bicgforone->AddDependentData(OOPMDataDepend(fId_max_iter, EReadAccess, max_iter_Version));
       bicgforone->AddDependentData(OOPMDataDepend(f_lId_p[i], EWriteAccess, p_Version));
       bicgforone->AddDependentData(OOPMDataDepend(fId_beta, EWriteAccess, beta_ver));
       bicgforone->AddDependentData(OOPMDataDepend(fId_rho_2, EReadAccess, rho_2_ver));
       bicgforone->AddDependentData(OOPMDataDepend(fId_alpha, EReadAccess, alpha_Version));
       bicgforone->AddDependentData(OOPMDataDepend(fId_omega, EReadAccess, omega_Version));
       bicgforone->AddDependentData(OOPMDataDepend(f_lId_v[i], EReadAccess, v_Version));
       bicgforone->AddDependentData(OOPMDataDepend(f_lId_M[i], EReadAccess, M_Version));
       bicgforone->AddDependentData(OOPMDataDepend(f_lId_phat[i], EReadAccess, phat_Version));
    

  *@author longhin
  */

      BiCGDouble* brho_1 = (BiCGDouble*) fDataDepend.Dep(0).ObjPtr()->Ptr();
      double * rho_1 = &brho_1->value;
      BiCGDouble* btol = (BiCGDouble*) fDataDepend.Dep(1).ObjPtr()->Ptr();
      double * tol = &btol->value;
      BiCGDouble* bnormr = (BiCGDouble*) fDataDepend.Dep(2).ObjPtr()->Ptr();
      double * normr = &bnormr->value;
      TParVector* br = (TParVector *) fDataDepend.Dep(3).ObjPtr()->Ptr();

      BiCGDouble* bnormb = (BiCGDouble*) fDataDepend.Dep(4).ObjPtr()->Ptr();
      double * normb = &bnormb->value;
      TBiCGInt * bmax_iter = (TBiCGInt*) fDataDepend.Dep(5).ObjPtr()->Ptr();
      int * max_iter = &bmax_iter->value;
      TParVector * bp = (TParVector*) fDataDepend.Dep(6).ObjPtr()->Ptr();

      BiCGDouble* bbeta = (BiCGDouble*) fDataDepend.Dep(7).ObjPtr()->Ptr();
      double * beta = &bbeta->value;
      BiCGDouble* brho_2 = (BiCGDouble*) fDataDepend.Dep(8).ObjPtr()->Ptr();
      double * rho_2 = &brho_2->value;
      BiCGDouble* balpha = (BiCGDouble*) fDataDepend.Dep(9).ObjPtr()->Ptr();
      double * alpha = &balpha->value;
      BiCGDouble* bomega = (BiCGDouble*) fDataDepend.Dep(10).ObjPtr()->Ptr();
      double * omega = &bomega->value;
      TParVector* bv = (TParVector*) fDataDepend.Dep(11).ObjPtr()->Ptr();

      TParMatrix* bM = (TParMatrix*) fDataDepend.Dep(12).ObjPtr()->Ptr();

      BiCGDouble* bphat = (BiCGDouble*) fDataDepend.Dep(0).ObjPtr()->Ptr();
      double * phat = &bphat->value;

/*    if (rho_1 == 0) {
      tol = Norm(r) / normb;
      return 2;
    }
    if (i == 1)
      p = r;
    else {
      //beta(0) = (rho_1(0)/rho_2(0)) * (alpha(0)/omega(0));
      beta = (rho_1/rho_2) * (alpha/omega); //TComputeBeta
      //p = r + beta(0) * (p - omega(0) * v);
      p *= beta; //p.Add(beta,1)
      p.Add(1., r);
      p.Add(- beta * omega, v);
    }
    M.Solve(p, phat);*/
      int i, nel;
      if((*rho_1)==0){
            *tol = *normr/(*normb);
            //Submit necessary Norm(r) tasks computation.
            //Update the versions correctly so that the application finishes
            return ESuccess;
      }
      if(*max_iter==1){
            nel = br->NElements();
            for(i=0;i<nel;i++)
                  (*bp)[i]=(*br)[i];
      }else{
            *beta=(*rho_1/(*rho_2)) * (*alpha/(*omega));
            for(i=0;i<nel;i++){
                  (*bp)[i] = (*beta) * (*bp)[i];
                  (*bp)[i] = (*bp)[i] + (*br)[i];// * 1;
                  (*bp)[i] = (*bp)[i] + (- *beta * *omega) * (*bv)[i];
            }
      }

      //Call M.Solve(p.phat)
      //Increment Versions
      //bp
      OOPDataVersion bpver = fDataDepend.Dep(6).ObjPtr()->Version();
      ++bpver;
      fDataDepend.Dep(6).ObjPtr()->SetVersion(bpver, Id());
      //bbeta
      OOPDataVersion bbetaver = fDataDepend.Dep(7).ObjPtr()->Version();
      ++bbetaver;
      fDataDepend.Dep(7).ObjPtr()->SetVersion(bbetaver, Id());
      
      
      #warning "Execute Method still incomplete"
      #warning "If rho_1==1 still need to be implemented"
      #warning "M.Solve(p,phat) still need to be implemented"

      return ESuccess;
}
