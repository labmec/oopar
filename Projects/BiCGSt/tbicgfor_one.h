/***************************************************************************
                          tbicgfor_one.h  -  description
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

#ifndef TBICGFOR_ONE_H
#define TBICGFOR_ONE_H

#include <ooptask.h>
#include <bicgdefs.h>

/**
 * Implements the first set of instructions from the for loop found in the BiCGStab method. Implements the following tasks:
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
    
 * The M.Solve instruction must be implemented by the class instantiated by the M object.
 * A fast solution for this is multiple inheritance. If TParMatrix were also descendent from TPZFMatrix the Solve
 * method will be implemented in the base class.
 * The inplications of this should checked.
 *    
 * @author longhin
 */

class TBiCGFor_One : public OOPTask  {
public:
      static OOPSaveable * Restore(OOPReceiveStorage * buf);
	TBiCGFor_One(int proc);
	~TBiCGFor_One();
	long ExecTime ();

	/**
	* Execute the task, verifying that all needed data acesses are satisfied.
	*/
	OOPMReturnType Execute ();

	/**
	* Returns last created Id.
	*/
	long GetClassID ()
	{
		return TBICGFOR_ONE_ID;
	}
      
};

#endif
