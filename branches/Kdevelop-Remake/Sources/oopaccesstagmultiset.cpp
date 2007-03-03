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
    if(it!=fTagMultiSet.begin()) out << std::endl;
    it->ShortPrint(out);
  }
}
OOPAccessTag OOPAccessTagMultiSet::GetCompatibleRequest(const OOPDataVersion & version, OOPMDataState need)
{
  OOPAccessTag result;
  std::multiset<OOPAccessTag>::iterator it;
  for(it = fTagMultiSet.begin(); it != fTagMultiSet.end(); it++)
  {
    if((it->Version() == version) && (it->AccessMode() == need))
    {
      result = *it;
      fTagMultiSet.erase(it);
    }
  }
  return result;
}
OOPAccessTag OOPAccessTagMultiSet::IncompatibleRequest(OOPDataVersion & version)
{
  OOPAccessTag result;
  std::multiset<OOPAccessTag>::iterator it;
  for(it = fTagMultiSet.begin(); it != fTagMultiSet.end(); it++)
  {
    if(it->Version() < version)
    {
      fTagMultiSet.erase(it);
      return *it;
    }
  }
  return result;
}

