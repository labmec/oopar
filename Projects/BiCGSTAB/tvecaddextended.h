/***************************************************************************
                          tvecaddextended.h  -  description
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

#ifndef TVECADDEXTENDED_H
#define TVECADDEXTENDED_H

#include <ooptask.h>

/**Extends the definition of the TVecAdd
  *@author longhin
  */

class TVecAddExtended : public OOPTask  {
public: 
	TVecAddExtended();
	~TVecAddExtended();
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
	OOPMReturnType Execute ();

	/**
	* Returns last created Id.
	*/
	virtual long GetClassID ()
	{
		return TVECADDEXTENDED_ID;
	}
  
};

#endif
