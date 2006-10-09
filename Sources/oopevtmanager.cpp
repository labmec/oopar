//
// C++ Implementation: oopevtmanager
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
//#include "ooptaskcontrol.h"


//#include "ooptask.h"
//#include "ooperror.h"
//#include "ooptaskmanager.h"

#include <sstream>
#include <pzlog.h>
#ifdef LOGPZ
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));
#endif

//#include "oopcommmanager.h"

#ifdef OOP_MPI
#include "mpi.h"
#endif
#ifdef OOP_MPE
#include "mpe.h"
#endif

#include "oopevtmanager.h"

#ifdef OOP_MPE
//OOPEvtManager OOPTaskControl::s_EvtMan;

OOPEventDatabase gEvtDB(20);

using namespace std;

template<class TEvt>
OOPEvtManager<TEvt>::OOPEvtManager(const std::string &description, int nevts, bool withdescription, const std::string color){
  m_Avail.clear();
  m_Used.clear();
  m_Evts.clear();
  pthread_mutex_init(&m_EvtMutex, NULL);
  f_description = description;
  FillMeUp(nevts,withdescription, color);
}

template<class TEvt>
OOPEvtManager<TEvt>::OOPEvtManager(){
  pthread_mutex_init(&m_EvtMutex, NULL);
}
template<class TEvt>
OOPEvtManager<TEvt> & OOPEvtManager<TEvt>::operator = (const OOPEvtManager<TEvt> & copy){
  m_Avail = copy.m_Avail;
  m_Used = copy.m_Used;
  m_Evts = copy.m_Evts;
  f_description = copy.f_description;
  return * this;
}

template<class TEvt>
void OOPEvtManager<TEvt>::FillMeUp(int nevts, bool withdescription, std::string color){
  pthread_mutex_lock(&m_EvtMutex);
  if(m_Evts.size()) {
    pthread_mutex_unlock(&m_EvtMutex);
    return;
  }
  int i=0;
  for(i=0;i<nevts;i++){
    TEvt levt;
    levt.Initialize(i,f_description,withdescription, color);
    pair< int, TEvt > item(i, levt);
    m_Evts.insert(item);
    m_Avail.insert(i);
  }
  pthread_mutex_unlock(&m_EvtMutex);
}

template<class TEvt>
void OOPEvtManager<TEvt>::GetEvent(TEvt & Evt){
  pthread_mutex_lock(&m_EvtMutex);
  if(m_Avail.size() == 0){
    int curr = m_Used.size();
    TEvt levt;
    levt.Initialize(curr,f_description,true,"black");
    pair< int, TEvt > item(curr, levt);
    m_Evts.insert(item);
    m_Avail.insert(curr);
  }
  set<int>::iterator it = m_Avail.begin();
  if(it == m_Avail.end()){
#ifdef LOGPZ    
    stringstream sout;
    sout << __FUNCTION__ << __LINE__ << " MPE Event ID management failed ";
    LOGPZ_DEBUG(logger,sout.str());
#endif
  }else{
#ifdef LOGPZ    
    stringstream sout;
    sout << __FUNCTION__ << __LINE__ << " Inserted on Used and removed from Avail " << *it;
    LOGPZ_DEBUG(logger,sout.str());
#endif
//    m_Evts.begin();
//    map< int, TEvt>::iterator mit = m_Evts.begin();
//    std::map< int, TEvt>::iterator mit;
//    mit = m_Evts.find(*it);
    if(m_Evts.find(*it) == m_Evts.end()) {
      cout << "Ferrou"<< endl;
      return;
    }
    Evt = m_Evts[*it];
    m_Used.insert(*it);
    m_Avail.erase(*it);
    Evt.SetManager(this);
#ifdef LOGPZ    
    stringstream sout;
    Evt.Print(sout);
    LOGPZ_DEBUG(logger,sout.str());
#endif

  }
  pthread_mutex_unlock(&m_EvtMutex);

}

template< class TEvt>
void OOPEvtManager<TEvt>::ReleaseEvent(int index){
#ifdef LOGPZ
  map<int, TEvt>::iterator it;
  it = m_Evts.find(index);
  if(it!=m_Evts.end()){
    stringstream sout;
    sout << __FUNCTION__ << __LINE__ << "Releasing EvtID " << *it;
    it->second.Print(sout);
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif
  pthread_mutex_lock(&m_EvtMutex);
  m_Avail.insert(index);
  m_Used.erase(index);
  pthread_mutex_unlock(&m_EvtMutex);
}

OOPEventDatabase::OOPEventDatabase(int numobjects)
{
  f_numobjects = numobjects;
}

void OOPEventDatabase::AddStateEvent(const std::string &eventname, 
    const std::string &description, const std::string &color, bool withdescription)
{
  OOPEvtManager<OOPStateEvent> evtman(description, f_numobjects, withdescription, color);
  fStateEvents[eventname]=evtman;
}
  
void OOPEventDatabase::AddSoloEvent(const std::string &eventname,
    const std::string &description, const std::string &color, bool withdescription)
{
  OOPEvtManager<OOPSoloEvent> evtman(description, f_numobjects, withdescription, color);
  fSoloEvents[eventname]=evtman;
}
  
void OOPEventDatabase::Initialize(const std::string &eventname,
    OOPStateEvent &evt)
{
  if(fStateEvents.find(eventname)!= fStateEvents.end()){
    fStateEvents[eventname].GetEvent(evt);
  }
}
  
void OOPEventDatabase::Initialize(const std::string &eventname,
    OOPSoloEvent &evt)
{
  if(fSoloEvents.find(eventname)!= fSoloEvents.end()){
    fSoloEvents[eventname].GetEvent(evt);
  }
}


template class OOPEvtManager<OOPStateEvent>;
template class OOPEvtManager<OOPSoloEvent>;

#endif


