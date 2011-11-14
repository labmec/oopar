/**
 * @file
 */
#ifdef OOP_MPE

#ifndef OOPEVTID_H
#define OOPEVTID_H

#define MAX_MSG_LENGTH 30

#include <map>
#include <set>
#include <iostream>

template<class TEvt>
class OOPEvtManager;

/**
 * @brief
 * @ingroup managertask
 */
class OOPEvent
{
protected:
	/**
	 * @brief Indentifies the thread which is performing the execution of the current Task
	 */
	int f_ThreadId;
	
	/**
	 * @brief Defines a structure used for appending the message to the state log.
	 */
	struct OOPLogMessage
	{
    private:
		/**
		 * @brief Size of the message
		 * Has to have is bits inverted so it can be properly interpreted by the MPE library
		 * Thanks to Philippe (The Bit Brusher) Devloo
		 */
		unsigned short fL;
		/**
		 * @brief The message text itself
		 */
		char fText[MAX_MSG_LENGTH+1];
    public:
		OOPLogMessage(){
			fText[0]='\0';
			fL=0;
		}
		
		void SetText(const char *text);
	} fMess;
	
	/**
	 * @brief Sets the text to be logged for the State of execution which is defined by a combination of
	 * f_EvtStart and f_EvtEnd on that order. \n
	 * The text is presented on the log.
	 */
	void SetText(const char * title);
	
};
/**
 * @brief Implements an Event Id from the MPE logging infrastructure.
 * @ingroup managertask
 */
/**
 * Used only when MPE is used as mpi interface.
 * Event Ids are not initialized automatically.
 * Requires a call for Initialize so it can obtain its unique ids.
 */
class OOPStateEvent : public OOPEvent {
public:
	OOPStateEvent & operator = (const OOPStateEvent & copy);
	/**
	 * @brief An empty constructor is needed for creating a map
	 */
	OOPStateEvent();
	/**
	 * @brief Simple constructor.
	 */
	OOPStateEvent(const std::string &eventname, const std::string &description);
	
	/**
	 * @brief The destructor will return the fThreadId to the event manager
	 */
	~OOPStateEvent();
	/**
	 * @brief Identifies the start event for an execution
	 * Triggered at the begining of the Execute Method for any OOPTask
	 */
	int f_EvtStart;
	/**
	 * @brief Identifies the end event for an execution
	 * Triggered at the end of the Execute Method for any OOPTask
	 */
	int f_EvtEnd;
	
	
	/**
	 * @brief Pointer to the event manager
	 */
	OOPEvtManager<OOPStateEvent> *f_manager;
	/**
	 * @brief Triggers the siganling of the f_EvtStart event
	 * A subsequent call to LogFinish defines a Executing state in that period
	 */
	void LogStart();
	/**
	 * @brief Triggers the signalling of the f_EvtEnd event.
	 * A prior call to LogStart and that current call defines a Executing state.
	 */
	void LogFinish();
	/**
	 * @brief Initializes the Events Ids according to the MPE infrastructure
	 * Called once for each EvtId defined.
	 */
	void Initialize(int myindex, std::string &description, bool withdescription, const std::string &color);
	/**
	 * @brief Initializes the Events Ids according to the MPE infrastructure
	 * Called once for each EvtId defined.
	 */
	void SetManager(OOPEvtManager<OOPStateEvent> *manager);
	/**
	 * @brief Prints the current object
	 */
	void Print(std::ostream & out = std::cout);
	
};


/**
 * @brief Implements an Event Id from the MPE logging infrastructure.
 * @ingroup managertask
 */
/**
 * Used only when MPE is used as mpi interface.
 * Event Ids are not initialized automatically.
 * Requires a call for Initialize so it can obtain its unique ids.
 */
class OOPSoloEvent : public OOPEvent {
public:
	/**
	 * @brief An empty constructor is needed for creating a map
	 */
	OOPSoloEvent();
	/**
	 * @brief Simple constructor.
	 */
	OOPSoloEvent(const std::string &eventname, const std::string &description);
	
	/**
	 * @brief The destructor will return the fThreadId to the event manager
	 */
	~OOPSoloEvent();
	/**
	 * @brief Identifies the start event for an execution
	 * Triggered at the begining of the Execute Method for any OOPTask
	 */
	int f_EvtStart;
	
	
	/**
	 * @brief Pointer to the event manager
	 */
	OOPEvtManager<OOPSoloEvent> *f_manager;
	/**
	 * @brief Sets the text to be logged for the State of execution which is defined by a combination of
	 * f_EvtStart and f_EvtEnd on that order.
	 * The text is presented on the log.
	 */
	void Log();
	/**
	 * @brief Initializes the Events Ids according to the MPE infrastructure
	 * Called once for each EvtId defined.
	 */
	void Initialize(int myindex, std::string &description, bool withdescription, const std::string &color);
	
	/**
	 * @brief Prints the current object
	 */
	void Print(std::ostream & out = std::cout);
	
	void SetManager(OOPEvtManager<OOPSoloEvent> *manager);
	
};

#endif

#endif // OOP_MPE


