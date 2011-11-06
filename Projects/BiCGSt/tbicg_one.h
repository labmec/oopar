/***************************************************************************
                          tbicg_one.h  -  description
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

#ifndef TBICG_ONE_H
#define TBICG_ONE_H

#include "ooptask.h"
#include "bicgdefs.h"
#include "bicgdouble.h"
#include "BiCGInt.h"

/**Implements the last operations on the BiCGStab method before the loop. The operations are:
  if(resid = normr/normb){
      tol=resid;
      max_iter=0;
      return 0;
  }
  *@author longhin
  */

class TBiCG_One : public OOPTask  {
public:
      static TPZSaveable * Restore(OOPStorageBuffer * buf);
	TBiCG_One(int proc);
	~TBiCG_One();
	/**
	* Returns the estimated execution time.
	* returns 0 if the task is instantaneous
	* returns > 0 if estimate is known
	* return < 0 if no estimate is known
	*/
	long ExecTime (){
    return -1;
  }

	/**
	* Execute the task, verifying that all needed data acesses are satisfied.
	*/
	OOPMReturnType Execute ();
  
  long GetClassID(){
    return TBICG_ONE_ID;
  }

  int Write(OOPStorageBuffer * buf,int classid);
  int Read(OOPStorageBuffer * buf,void *context);

};

#endif
