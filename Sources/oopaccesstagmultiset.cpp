//
// C++ Implementation: oopaccesstagmultiset
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopaccesstagmultiset.h"

OOPAccessTagMultiSet::OOPAccessTagMultiSet()
{
}


OOPAccessTagMultiSet::~OOPAccessTagMultiSet()
{
}

void OOPAccessTagMultiSet::Print(std::ostream & out) const
{
  std::multiset<OOPAccessTag>::iterator it;
  for(it = fTagMultiSet.begin(); it != fTagMultiSet.end(); it++)
  {
    it->Print(out);
  }
}

