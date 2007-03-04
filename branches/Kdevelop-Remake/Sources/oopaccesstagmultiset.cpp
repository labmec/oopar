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
#include "oopdatamanager.h"

#include "pzlog.h"

#ifdef LOGPZ
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPAccessTagMultiset"));
#endif

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
    if((it->Version().CanExecute(version)) && (it->AccessMode() == need))
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

  /// Verifies whether a similar access request exists within the list of requests
bool OOPAccessTagMultiSet::HasSimilarRequest(OOPAccessTag tag)
{
  OOPObjectId zeroid;
  int processor = DM->GetProcID();
  tag.SetTaskId(zeroid);
  tag.SetProcessor(processor);
  multiset<OOPAccessTag>::iterator it;
  for(it= fTagMultiSet.begin(); it!= fTagMultiSet.end(); it++)
  {
    OOPAccessTag loctag(*it);
    loctag.SetTaskId(zeroid);
    loctag.SetProcessor(processor);
    if(tag == loctag) 
    {
#ifdef LOGPZ
      LOGPZ_DEBUG(logger,"HasSimilarRequest returning true")
#endif
      return true;
    }
  }
#ifdef LOGPZ
  LOGPZ_DEBUG(logger,"HasSimilarRequest returning false")
#endif
  return false;
  
}

  /// generates the set of accesstags that need to be sent when changing the owning processor
void OOPAccessTagMultiSet::GetProcessorAccessRequests(int processor, std::set<OOPAccessTag> &requests)
{
  OOPObjectId zeroid;
  int locproc = DM->GetProcID();
  multiset<OOPAccessTag>::iterator it;
  for(it= fTagMultiSet.begin(); it!= fTagMultiSet.end(); it++)
  {
    if(it->Proc() != processor)
    {
      OOPAccessTag tag(*it);
      if(tag.TaskId())
      {
        tag.SetProcessor(locproc);
        tag.SetTaskId(zeroid);
      }
      requests.insert(tag);
    }
  }
  // retain only the access requests related to the current processor, 
  
  std::multiset<OOPAccessTag> copy(fTagMultiSet);
  fTagMultiSet.clear();
  for(it=copy.begin(); it!=copy.end(); it++)
  {
    if(it->Proc() == locproc)
    {
      fTagMultiSet.insert(*it);
    }
  }
}
