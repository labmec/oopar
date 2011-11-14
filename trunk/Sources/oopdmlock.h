/**
 * @file
 */
#ifndef OOPDMLOCK_H
#define OOPDMLOCK_H
#include "pthread.h"

/**
 * @brief Implements a lock for the DataManager
 * @ingroup managercomm
 * @author Edimar Cesar Rylo
 */
/**
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
