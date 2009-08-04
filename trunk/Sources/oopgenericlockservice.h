/*
 * oopgenericlockservice.h
 *
 *  Created on: Jul 29, 2009
 *      Author: phil
 */

#ifndef OOPGENERICLOCKSERVICE_H_
#define OOPGENERICLOCKSERVICE_H_

#include <pthread.h>

class OOPGenericLockService
{

public:

	OOPGenericLockService()
	{
		pthread_mutex_init (&fMutex, 0);
	}

	~OOPGenericLockService()
	{
		pthread_mutex_destroy (&fMutex);
	}

	pthread_mutex_t *Mutex()
	{
		return &fMutex;
	}

private:

	/**
	 * the mutex object around which we will be locking
	 */
	pthread_mutex_t fMutex;

};

#endif /* OOPGENERICLOCKSERVICE_H_ */
