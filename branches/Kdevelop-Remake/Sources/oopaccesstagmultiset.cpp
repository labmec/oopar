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
OOPAccessTag OOPAccessTagMultiSet::GetCompatibleRequest(const OOPDataVersion & version, OOPMDataState need)
{
  std::multiset<OOPAccessTag>::iterator it;
  for(it = fTagMultiSet.begin(); it != fTagMultiSet.end(); it++)
  {
    if((it->Version() == version) && (it->AccessMode() == need))
    {
      return *it;
    }
  }
}
OOPAccessTag OOPAccessTagMultiSet::IncompatibleRequest(OOPDataVersion & version)
{
  std::multiset<OOPAccessTag>::iterator it;
  for(it = fTagMultiSet.begin(); it != fTagMultiSet.end(); it++)
  {
    if(it->Version() < version)
    {
      return *it;
    }
  }
}

