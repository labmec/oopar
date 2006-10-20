//
// C++ Implementation: oopevtid
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sstream>
#include <pzlog.h>
#ifdef LOGPZ
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskControl"));
#endif



#include "oopevtid.h"
#include "oopevtmanager.h"

#ifdef OOP_MPI
#include "mpi.h"
#endif
#ifdef OOP_MPE
#include "mpe.h"
#endif

#ifdef OOP_MPE


using namespace std;
void OOPEvent::OOPLogMessage::SetText(const char * title)
{
/*  MPE_Describe_comm_state( MPI_COMM_WORLD, f_ThreadId, f_EvtStart, f_EvtEnd,
                             "Task Execution", "blue",
                             "%s" );*/
  strncpy(fText, title,MAX_MSG_LENGTH);
  fText[MAX_MSG_LENGTH]='\0';

  fL = strlen(title) > MAX_MSG_LENGTH ? MAX_MSG_LENGTH : strlen(title);
  int i;
  for(i=fL; i<MAX_MSG_LENGTH+1; i++) fText[i] = '\0';
/*  cout << __PRETTY_FUNCTION__ << " fL = " << fMess.fL << " Message = " << " MSG_LEN " << MAX_MSG_LENGTH << endl;
  for(i=0; i<fMess.fL; i++) cout << fMess.fText[i];
  cout << endl;*/
  unsigned int first,second;
  first = fL/256;
  second = fL%256;
  fL = (second << 8) + first;

}

OOPStateEvent::OOPStateEvent(const std::string &eventname, const std::string &description){
  f_ThreadId = -1;
  f_EvtStart = -1;
  f_EvtEnd = -1;
  f_manager = 0;
  gEvtDB.Initialize(eventname,*this);
  fMess.SetText(description.c_str());
  if(f_manager) {
    MPE_Log_comm_event(MPI_COMM_WORLD,f_ThreadId,f_EvtStart, NULL);
  }
}
OOPStateEvent & OOPStateEvent::operator = (const OOPStateEvent & copy){
  f_ThreadId = copy.f_ThreadId;
  f_EvtStart = copy.f_EvtStart;
  f_EvtEnd = copy.f_EvtEnd;
  f_manager = copy.f_manager;
  memcpy(&fMess,&copy.fMess, sizeof(OOPLogMessage));
  return * this;
}

OOPStateEvent::OOPStateEvent(){
  f_ThreadId = -1;
  f_EvtStart = -1;
  f_EvtEnd = -1;
  f_manager = 0;
}
void OOPStateEvent::Initialize(int myindex, std::string &description, bool withdescription, const std::string &color)
{
  f_ThreadId = myindex;
  // initializar o f_EvtStart
  //MPE_Log_get_solo_eventID(&f_EvtStart);
  MPE_Log_get_state_eventIDs(&f_EvtStart, &f_EvtEnd);
  if(withdescription)
  {
    MPE_Describe_comm_state( MPI_COMM_WORLD, f_ThreadId, f_EvtStart, f_EvtEnd,
                             description.c_str(), color.c_str(),
                             "%s" );
  }

}
void OOPStateEvent::SetManager(OOPEvtManager<OOPStateEvent> *manager)
{
  this->f_manager= manager;
}
OOPStateEvent::~OOPStateEvent()
{
  
  if(f_manager) {
    MPE_Log_comm_event(MPI_COMM_WORLD,f_ThreadId,f_EvtEnd, (char*)&fMess);
    f_manager->ReleaseEvent(f_ThreadId);
  }
  f_manager = 0;
}
void OOPStateEvent::Print(std::ostream & out)
{
  out << "-------------Index " << f_ThreadId << " Start ID " << f_EvtStart 
      << " End ID " << f_EvtEnd << endl;
  out.flush();
}

void OOPSoloEvent::Print(std::ostream & out)
{
  out << "-------------Index " << f_ThreadId << " Start ID " << f_EvtStart 
      << endl;
  out.flush();
}


OOPSoloEvent::OOPSoloEvent(const std::string &eventname, const std::string &description){
  f_ThreadId = -1;
  f_EvtStart = -1;
  f_manager = 0;
  gEvtDB.Initialize(eventname,*this);
  fMess.SetText((char *)description.c_str());
  Log();
}

OOPSoloEvent::OOPSoloEvent(){
  f_ThreadId = -1;
  f_EvtStart = -1;
  f_manager = 0;
  
}

/*void OOPEvent::SetText(char * title){
  if(f_ThreadId < 0) exit( -1);
  fMess.SetText(title);
  MPE_Describe_comm_state( MPI_COMM_WORLD, f_ThreadId, f_EvtStart, f_EvtEnd,
                             "Task Execution", "blue",
                             "%s" );
  strncpy(fMess.fText, title,MAX_MSG_LENGTH);
 fMess.fText[MAX_MSG_LENGTH]='\0';

  fMess.fL = strlen(title) > MAX_MSG_LENGTH ? MAX_MSG_LENGTH : strlen(title);
  int i;
  for(i=fMess.fL; i<MAX_MSG_LENGTH+1; i++) fMess.fText[i] = '\0';
  cout << __PRETTY_FUNCTION__ << " fL = " << fMess.fL << " Message = " << " MSG_LEN " << MAX_MSG_LENGTH << endl;
  for(i=0; i<fMess.fL; i++) cout << fMess.fText[i];
  cout << endl;
  unsigned int first,second;
  first = fMess.fL/256;
  second = fMess.fL%256;
  fMess.fL = (second << 8) + first;
}*/

void OOPSoloEvent::Log(){
  if(f_manager)
  {
    MPE_Log_comm_event(MPI_COMM_WORLD,f_ThreadId,f_EvtStart, (char*)&fMess);
  }
}
void OOPSoloEvent::Initialize(int myindex, std::string &description, bool withdescription, const std::string &color)
{
  f_ThreadId = myindex;
  // initializar o f_EvtStart
  MPE_Log_get_solo_eventID(&f_EvtStart);
  
  if(withdescription)
  {
    MPE_Describe_comm_event( MPI_COMM_WORLD, f_ThreadId,
                                    f_EvtStart, description.c_str(), color.c_str(), "%s");
/*    MPE_Describe_comm_state( MPI_COMM_WORLD, f_ThreadId, f_EvtStart, f_EvtEnd,
                             description.c_str(), "blue",
                             "%s" );*/
  }

}

void OOPSoloEvent::SetManager(OOPEvtManager<OOPSoloEvent> *manager)
{
  this->f_manager= manager;
}

OOPSoloEvent::~OOPSoloEvent()
{
  if(f_manager) f_manager->ReleaseEvent(f_ThreadId);
  f_manager = 0;
}


#endif

