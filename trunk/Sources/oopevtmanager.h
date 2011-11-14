/**
 * @file
 */
#ifndef OOPEVTMANAGER_H
#define OOPEVTMANAGER_H
#ifdef OOP_MPE
#include <map>
#include <set>
#include <string>

/**
 * @brief Implements a manager for the Event Ids manipulation. 
 * Holds a list with all initialized EvtIds. \n
 * Is used as static member for OOPTaskControl objects
 * @author Edimar Cesar Rylo
 * @ingroup managertask
 */
template<class TEvt>
class OOPEvtManager{
public:
	
	OOPEvtManager();
	/**
	 * @brief Simple constructor
	 */
	OOPEvtManager(const std::string &description, int nevts, bool withdescription, const std::string color);
	/**
	 * @brief Gets both index and EventId 
	 * Performs calls to the MPE library so it can obtain unique ids for events.
	 * All obtained Event ids are stored on a map with an integer attribute as key.
	 * This key is returned on the index parameter
	 */
	void GetEvent(TEvt & Evt);
	/**
	 * @brief Releases the EventId associated with the index parameters
	 * Allows the released EvetId to be used by another executing task.
	 */
	void ReleaseEvent(int index);
	
	OOPEvtManager<TEvt> & operator = (const OOPEvtManager<TEvt> & copy);
private:
	/**
	 * @brief Instantiates a set of EvntIds
	 */
	void FillMeUp(int nevts, bool withdescription, std::string color);
	/**
	 * @brief string with the description of the event
	 */
	std::string f_description;
	/**
	 * @brief Multiplexes access to the m_Avail, m_Used and m_Evts objects.
	 */
	pthread_mutex_t m_EvtMutex;
	/**
	 * @brief Holds a set of indices which identifies the available EventIds
	 */
	std::set<int> m_Avail;
	/**
	 * @brief Holds a set of indices which identifies the used EventIds
	 */
	std::set<int> m_Used;
	/**
	 * @brief Holds a map from integers to OOPEvtIds objects. \n
	 * Copies of such objects are handled to the OOPTaskControl so the execution of its associated OOPTasks can be logged
	 */
	std::map<int, TEvt > m_Evts;
};


#include "oopevtid.h"
/**
 * @ingroup managertask
 */
class OOPEventDatabase
{
	std::map< std::string, OOPEvtManager<OOPStateEvent> > fStateEvents;
	std::map< std::string, OOPEvtManager<OOPSoloEvent> > fSoloEvents;
	
	/**
	 * @brief Number of events created at startup
	 */
	int f_numobjects;
	
public:
	
	OOPEventDatabase(int numobjects);
	
	void AddStateEvent(const std::string &eventname, const std::string &description, const std::string &color, bool withdescription);
	
	void AddSoloEvent(const std::string &eventname, const std::string &description,const std::string &color, bool withdescription);
	void Initialize(const std::string &eventname, OOPStateEvent &evt);
	
	void Initialize(const std::string &eventname, OOPSoloEvent &evt);
};

extern OOPEventDatabase gEvtDB;


#endif

#endif
