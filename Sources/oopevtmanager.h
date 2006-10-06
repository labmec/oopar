//
// C++ Interface: oopevtmanager
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPEVTMANAGER_H
#define OOPEVTMANAGER_H
#ifdef OOP_MPE
#include <map>
#include <set>

//#include "oopevtid.h"

/**
 * Implements a manager for the Event Ids manipulation
 * Holds a list with all initialized EvtIds.
 * Is used as static member for OOPTaskControl objects
 */
template<class TEvt>
class OOPEvtManager{
public:
  
  OOPEvtManager();
  /**
   * Simple constructor
   */
  OOPEvtManager(const std::string &description, int nevts, bool withdescription);
  /**
   * Gets both index and EventId 
   * Performs calls to the MPE library so it can obtain unique ids for events.
   * All obtained Event ids are stored on a map with an integer attribute as key.
   * This key is returned on the index parameter
   */
  void GetEvent(TEvt & Evt);
  /**
   * Releases the EventId associated with the index parameters
   * Allows the released EvetId to be used by another executing task.
   */
  void ReleaseEvent(int index);

  OOPEvtManager<TEvt> & operator = (const OOPEvtManager<TEvt> & copy);
private:
  /**
   * Instantiates a set of EvntIds
   */
  void FillMeUp(int nevts, bool withdescription);
  /**
   * string with the description of the event
   */
  std::string f_description;
  /**
   * Multiplexes access to the m_Avail, m_Used and m_Evts objects.
   */
  pthread_mutex_t m_EvtMutex;
  /**
   * Holds a set of indices which identifies the available EventIds
   */
  std::set<int> m_Avail;
  /**
   * Holds a set of indices which identifies the used EventIds
   */
  std::set<int> m_Used;
  /**
   * Holds a map from integers to OOPEvtIds objects.
   * Copies of such objects are handled to the OOPTaskControl so the execution of its associated OOPTasks can be logged
   */
  std::map<int, TEvt > m_Evts;
};

#include "oopevtid.h"

class OOPEventDatabase
{
  std::map< std::string, OOPEvtManager<OOPStateEvent> > fStateEvents;
  std::map< std::string, OOPEvtManager<OOPSoloEvent> > fSoloEvents;

  /**
   * number of events created at startup
   */
  int f_numobjects;

public:

  OOPEventDatabase(int numobjects);

  void AddStateEvent(const std::string &eventname, const std::string &description, bool withdescription);
  
  void AddSoloEvent(const std::string &eventname, const std::string &description, bool withdescription);
  
  void Initialize(const std::string &eventname, OOPStateEvent &evt);
  
  void Initialize(const std::string &eventname, OOPSoloEvent &evt);
};

extern OOPEventDatabase gEvtDB;


#endif

#endif
