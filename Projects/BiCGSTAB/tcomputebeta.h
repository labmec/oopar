/***************************************************************************
                          tcomputebeta.h  -  description
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

#ifndef TCOMPUTEBETA_H
#define TCOMPUTEBETA_H

#include <ooptask.h>

/**Implements the computation of the beta inside the else on the begining of the loop
  *@author longhin
  */

class TComputeBeta : public OOPTask  {
public: 
	/**
	* Returns the estimated execution time.
	* returns 0 if the task is instantaneous
	* returns > 0 if estimate is known
	* return < 0 if no estimate is known
	*/
	virtual long ExecTime ();

	/**
	* Execute the task, verifying that all needed data acesses are satisfied.
	*/
	virtual OOPMReturnType Execute ();

	/**
	* Returns last created Id.
	*/
	virtual long GetClassID ()
	{
		return TCOMPUTEBETA_ID;
	}
  void SetId_alpha(OOPObjectId & Id);
  void SetId_beta(OOPObjectId & Id);
  void SetId_rho_1(OOPObjectId & Id);
  void SetId_rho_2(OOPObjectId & Id);
private:
  OOPObjectId fId_alpha;
  OOPObjectId fId_beta;
  OOPObjectId fId_rho_1;
  OOPObjectId fId_rho_2;
};

#endif
