// -*- c++ -*-

#include "oopcommmanager.h"

OOPCommunicationManager::OOPCommunicationManager(){
  f_num_proc = f_myself = 0;
}

OOPCommunicationManager::~OOPCommunicationManager(){}

int OOPCommunicationManager::Initialize (char *process_name, int num_of_process){
  return 0;
}

int OOPCommunicationManager::GetProcID(){
  return f_myself;
}

int OOPCommunicationManager::NumProcessors (){
  return f_num_proc;
}

int OOPCommunicationManager::IAmTheMaster(){
  return 0;
}

char * OOPCommunicationManager::ClassName(){
  return ("");
}
