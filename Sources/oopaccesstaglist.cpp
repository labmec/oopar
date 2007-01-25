//
// C++ Implementation: oopaccesstaglist
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopaccesstaglist.h"
#include "oopdatamanager.h"

OOPAccessTagList::OOPAccessTagList()
{
}


OOPAccessTagList::~OOPAccessTagList()
{
}




void OOPAccessTagList::Clear()
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  std::set<OOPObjectId> lset;
  for(;it!=fTagList.end();it++)
  {
    lset.insert(it->Id());
    if(it->AccessMode() == EWriteAccess)
    {
      DM->SubmitData(*it);
    }
    it->ClearPointer();
  }
//  fTagList.clear();
  DM->ObjectChanged(lset);
}

void OOPAccessTagList::GrantAccess(const OOPAccessTag & granted)
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  for(;it!=fTagList.end();it++)
  {
    if(it->IsMyAccessTag(granted)) 
    {
      *it = granted;
    }
  }
}
bool OOPAccessTagList::CanExecute()
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  for(;it!=fTagList.end();it++)
  {
    if(!it->CanExecute()) return false;
  }
  return true;
}

void OOPAccessTagList::IncrementWriteDependent()
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  for(;it!=fTagList.end();it++)
  {
    if(it->AccessMode() == EWriteAccess)
    {
      it->IncrementVersion();
    }
  }
  
}

