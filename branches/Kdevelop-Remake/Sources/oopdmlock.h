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
Implements a lock for the DataManager
 * Has its pthread_mutex_t component defined as static, and initialized outside the class scope.
 * All accesses to the DM data structure (lists mostly) are protected by a calling the lock on this mutex.
*/
class OOPDMLock{
public:
  OOPDMLock();

  ~OOPDMLock();
private:
  static pthread_mutex_t fLock;
};

#endif
