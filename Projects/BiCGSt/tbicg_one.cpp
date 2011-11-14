/***************************************************************************
 tbicg_one.cpp  -  description
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

#include "tbicg_one.h"
#include <oopmetadata.h>

OOPSaveable * TBiCG_One::Restore(OOPReceiveStorage * buf){
	TBiCG_One * bicgone = new TBiCG_One(0);
	bicgone->Unpack(buf);
	return bicgone;
}

TBiCG_One::TBiCG_One(int proc) : OOPTask(proc) {
}
TBiCG_One::~TBiCG_One(){
}

int TBiCG_One::Write(OOPSendStorage * buf,int classid) {
	OOPTask::Write(buf,classid);
}
int TBiCG_One::Read(OOPReceiveStorage * buf, void *context) {
	OOPTask::Read(buf,context);
}
OOPMReturnType TBiCG_One::Execute (){
#warning "Not yet implemented"
#warning "Version increment not implemented"
	//Get the actual values from Saveable objects
	BiCGDouble* bresid = (BiCGDouble*) fDataDepend.Dep(0).ObjPtr()->Ptr();
	double * resid = &bresid->value;
	BiCGDouble* bnormb = (BiCGDouble*) fDataDepend.Dep(1).ObjPtr()->Ptr();
	double * normb = &bnormb->value;
	BiCGDouble* bnormr = (BiCGDouble*) fDataDepend.Dep(2).ObjPtr()->Ptr();
	double * normr = &bnormr->value;
	BiCGDouble* btol = (BiCGDouble*) fDataDepend.Dep(3).ObjPtr()->Ptr();
	double * tol = &btol->value;
	TBiCGInt * bmax_iter = (TBiCGInt*) fDataDepend.Dep(4).ObjPtr()->Ptr();
	int * max_iter = &bmax_iter->value;
	
	if(((*resid) = (*normr)/(*normb)) <= (*tol)){
		*tol = *resid;
		*max_iter = 0;
		//How to implement here the increment on the versions from all data ?
	}
	
}