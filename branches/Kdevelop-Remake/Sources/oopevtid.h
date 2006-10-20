//
// C++ Interface: oopevtid
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPEVTID_H
#define OOPEVTID_H

#ifdef OOP_MPE
#define MAX_MSG_LENGTH 30

#include <map>
#include <set>
#include <iostream>

template<class TEvt>
class OOPEvtManager;

class OOPEvent
{
protected:
  /**
   * Indentifies the thread which is performing the execution of the current Task
   */
  int f_ThreadId;
  /**
   * Defines a structure used for appending the message to the state log.
   */
  struct OOPLogMessage
  {
    private:
    /**
     * Size of the message
     * Has to have is bits inverted so it can be properly interpreted by the MPE library
     * Thanks to Philippe (The Bit Brusher) Devloo
     */
    unsigned short fL;
    /**
     * The message text itself
     */
    char fText[MAX_MSG_LENGTH+1];
    /**
     * Not used yet - on the todo list
     */
    public:
    OOPLogMessage(){
      fText[0]='\0';
      fL=0;
    }
    /**
     * Not used yet - on the todo list
     */
    void SetText(const char *text);
  } fMess;

  /**
   * Sets the text to be logged for the State of execution which is defined by a combination of
   * f_EvtStart and f_EvtEnd on that order.
   * The text is presented on the log.
   */
  void SetText(const char * title);

};
/**
 * Implements an Event Id from the MPE logging infrastructure.
 * Used only when MPE is used as mpi interface.
 * Event Ids are not initialized automatically.
 * Requires a call for Initialize so it can obtain its unique ids.
 */
class OOPStateEvent : public OOPEvent {
public:
  OOPStateEvent & operator = (const OOPStateEvent & copy);
  /**
   * An empty constructor is needed for creating a map
   */
  OOPStateEvent();
  /**
   * Simple constructor.
   */
  OOPStateEvent(const std::string &eventname, const std::string &description);

  /**
   * The destructor will return the fThreadId to the event manager
  */
  ~OOPStateEvent();
  /**
   * Identifies the start event for an execution
   * Triggered at the begining of the Execute Method for any OOPTask
   */
  int f_EvtStart;
  /**
   * Identifies the end event for an execution
   * Triggered at the end of the Execute Method for any OOPTask
   */
  int f_EvtEnd;


  /**
   * Pointer to the event manager
   */
  OOPEvtManager<OOPStateEvent> *f_manager;
  /**
   * Triggers the siganling of the f_EvtStart event
   * A subsequent call to LogFinish defines a Executing state in that period
   */
  void LogStart();
  /**
   * Triggers the signalling of the f_EvtEnd event.
   * A prior call to LogStart and that current call defines a Executing state.
   */
  void LogFinish();
  /**
   * Initializes the Events Ids according to the MPE infrastructure
   * Called once for each EvtId defined.
   */
  void Initialize(int myindex, std::string &description, bool withdescription, const std::string &color);
  /**
   * Initializes the Events Ids according to the MPE infrastructure
   * Called once for each EvtId defined.
   */
  void SetManager(OOPEvtManager<OOPStateEvent> *manager);
  /**
   * Prints the current object
   */
  void Print(std::ostream & out = std::cout);
};


/**
 * Implements an Event Id from the MPE logging infrastructure.
 * Used only when MPE is used as mpi interface.
 * Event Ids are not initialized automatically.
 * Requires a call for Initialize so it can obtain its unique ids.
 */
class OOPSoloEvent : public OOPEvent {
public:
  /**
   * An empty constructor is needed for creating a map
   */
  OOPSoloEvent();
  /**
   * Simple constructor.
   */
  OOPSoloEvent(const std::string &eventname, const std::string &description);

  /**
   * The destructor will return the fThreadId to the event manager
  */
  ~OOPSoloEvent();
  /**
   * Identifies the start event for an execution
   * Triggered at the begining of the Execute Method for any OOPTask
   */
  int f_EvtStart;


  /**
   * Pointer to the event manager
   */
  OOPEvtManager<OOPSoloEvent> *f_manager;
  /**
   * Sets the text to be logged for the State of execution which is defined by a combination of
   * f_EvtStart and f_EvtEnd on that order.
   * The text is presented on the log.
   */
  void Log();
  /**
   * Initializes the Events Ids according to the MPE infrastructure
   * Called once for each EvtId defined.
   */
  void Initialize(int myindex, std::string &description, bool withdescription, const std::string &color);

  /**
   * Prints the current object
   */
  void Print(std::ostream & out = std::cout);

  void SetManager(OOPEvtManager<OOPSoloEvent> *manager);
};


#endif

#endif
