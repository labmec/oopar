//
// C++ Implementation: ooptmlock
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ooptmlock.h"

pthread_mutex_t OOPTMLock::fLock = PTHREAD_MUTEX_INITIALIZER;

OOPTMLock::OOPTMLock()
{
	pthread_mutex_lock(&fLock);
}


OOPTMLock::~OOPTMLock()
{
	pthread_mutex_unlock(&fLock);
}

void OOPTMLock::Lock()
{
	pthread_mutex_lock(&fLock);
}
void OOPTMLock::Unlock()
{
	pthread_mutex_unlock(&fLock);
}
