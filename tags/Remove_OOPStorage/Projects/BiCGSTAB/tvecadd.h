/***************************************************************************
                          tvecadd.h  -  description
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

#ifndef TVECADD_H
#define TVECADD_H

#include <ooptask.h>

/**Implements the v1 = v1 + escalar*v2
  *@author longhin
  */

class TVecAdd : public OOPTask  {
public: 
	TVecAdd();
	~TVecAdd();
  void SetValue(double value){fValue = value;}
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
		return TDISTNORM_ID;
	}
private:
  double fValue;
  
};

#endif
