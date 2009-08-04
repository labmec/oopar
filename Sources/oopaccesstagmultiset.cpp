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

#ifdef LOG4CXX
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
#ifdef LOG4CXX
      stringstream sout;
      sout << "it->Version().CanExecute(version) " << it->Version().CanExecute(version) <<
        " need " << need;
      LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
    if((it->Version().CanExecute(version)) && (it->AccessMode() == need))
    {
      result = *it;
      result.SetVersion(version);
#ifdef LOG4CXX
      stringstream sout;
      sout << "returning and deleting tag: ";
      result.ShortPrint( sout);
      LOGPZ_DEBUG(logger,sout.str().c_str());
#endif
      fTagMultiSet.erase(it);
      break;
    }
  }
  return result;
}
OOPAccessTag OOPAccessTagMultiSet::IncompatibleRequest(OOPDataVersion & dataversion)
{
  OOPAccessTag result;
  std::multiset<OOPAccessTag>::iterator it;
  for(it = fTagMultiSet.begin(); it != fTagMultiSet.end(); it++)
  {
    if(! it->Version().AmICompatible(dataversion))
    {
      result = *it;
      fTagMultiSet.erase(it);
      break;
    }
  }
  return result;
}

  /// Verifies whether a similar access request exists within the list of requests
bool OOPAccessTagMultiSet::HasSimilarRequest(OOPAccessTag tag, int processor)
{
  OOPObjectId zeroid;
//  int processor = DM->GetProcID();
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
#ifdef LOG4CXX
      LOGPZ_DEBUG(logger,"HasSimilarRequest returning true")
#endif
      return true;
    }
  }
#ifdef LOG4CXX
  LOGPZ_DEBUG(logger,"HasSimilarRequest returning false")
#endif
  return false;

}

  /// generates the set of accesstags that need to be sent when changing the owning processor
void OOPAccessTagMultiSet::GetProcessorAccessRequests(int processor, std::set<OOPAccessTag> &requests, int localprocessor)
{
  OOPObjectId zeroid;
//  int locproc = DM->GetProcID();
  multiset<OOPAccessTag>::iterator it;
  for(it= fTagMultiSet.begin(); it!= fTagMultiSet.end(); it++)
  {
    if(it->Proc() != processor)
    {
      OOPAccessTag tag(*it);
      if(tag.TaskId())
      {
        tag.SetProcessor(localprocessor);
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
    if(it->Proc() == localprocessor)
    {
      fTagMultiSet.insert(*it);
    }
  }
}
