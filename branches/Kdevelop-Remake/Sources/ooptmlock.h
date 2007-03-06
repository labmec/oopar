//
// C++ Interface: ooptmlock
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPTMLOCK_H
#define OOPTMLOCK_H

#include <pthread.h>

/**
Implements a lock for the TaskManager
 * Has its pthread_mutex_t component defined as static, and initialized outside the class scope.
 * All accesses to the TM data structure (lists mostly) are protected by a calling the lock on this mutex.

@author Gustavo C Longhin
*/
class OOPTMLock{
public:
  OOPTMLock();

  ~OOPTMLock();
  void Unlock();
  void Lock();
private:
  static pthread_mutex_t fLock;
};

#endif
