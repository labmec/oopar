//
// C++ Interface: oopdmlock
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPDMLOCK_H
#define OOPDMLOCK_H
#include "pthread.h"
/**
Implements a Mutex for thread safe access on the DM data structure
*/
class OOPDMLock{
public:
  OOPDMLock();

  ~OOPDMLock();
private:
  static pthread_mutex_t fLock;
};

#endif
