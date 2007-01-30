//
// C++ Implementation: oopdmlock
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopdmlock.h"

pthread_mutex_t fLock = PTHREAD_MUTEX_INITIALIZER;

OOPDMLock::OOPDMLock()
{
  pthread_mutex_lock(&fLock);
}


OOPDMLock::~OOPDMLock()
{
  pthread_mutex_unlock(&fLock);
}


