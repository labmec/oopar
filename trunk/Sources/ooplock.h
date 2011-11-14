/**
 * @file
 */

#ifndef OOPLOCK_H_
#define OOPLOCK_H_

#include <pthread.h>
#include "tpzautopointer.h"
#include <pzlog.h>
#ifdef LOG4CXX
static LoggerPtr loggerlock (Logger::getLogger ("OOPar.OOPLock"));
#endif


/**
 * @brief Implements a lock for the TaskManager. \n
 * @author Philippe Devloo
 * @since 23/07/2009
 * @ingroup managercomm
 */
/**
 * Has its pthread_mutex_t component defined as static, and initialized outside the class scope.
 * All accesses to the TM data structure (lists mostly) are protected by a calling the lock on this mutex.
 */
template<class LockService>
class OOPLock {
public:
	OOPLock(LockService *obj);
	OOPLock(TPZAutoPointer<LockService> obj);
	
	~OOPLock();
	void Unlock();
	void Lock();
private:
	LockService *fReference;
	bool fIamLocked;
};

template<class LockService>
inline OOPLock<LockService>::OOPLock(LockService *obj) : fReference(obj)
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__ << "Trying to acquire";
		LOGPZ_DEBUG(loggerlock,sout.str())
	}
#endif
	pthread_mutex_lock(fReference->Mutex());
	fIamLocked=true;
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__ << "Acquired";
		LOGPZ_DEBUG(loggerlock,sout.str())
	}
#endif
}

template<class LockService>
inline OOPLock<LockService>::OOPLock(TPZAutoPointer<LockService> obj) : fReference(obj.operator->())
{
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__ << "Trying to acquire";
		LOGPZ_DEBUG(loggerlock,sout.str())
	}
#endif
	pthread_mutex_lock(fReference->Mutex());
	fIamLocked = true;
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << __PRETTY_FUNCTION__ << "Acquired";
		LOGPZ_DEBUG(loggerlock,sout.str())
	}
#endif
}

template<class LockService>
inline OOPLock<LockService>::~OOPLock()
{
	if(fIamLocked)
	{
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << __PRETTY_FUNCTION__ << "Giving up the lock";
			LOGPZ_DEBUG(loggerlock,sout.str())
		}
#endif
		pthread_mutex_unlock(fReference->Mutex());
		fIamLocked = false;
	}
}

template<class LockService>
inline void OOPLock<LockService>::Lock()
{
	if(!fIamLocked)
	{
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << __PRETTY_FUNCTION__ << "Trying to acquire";
			LOGPZ_DEBUG(loggerlock,sout.str())
		}
#endif
		pthread_mutex_lock(fReference->Mutex());
		fIamLocked = true;
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << __PRETTY_FUNCTION__ << "Acquired";
			LOGPZ_DEBUG(loggerlock,sout.str())
		}
#endif
	}
}

template<class LockService>
inline void OOPLock<LockService>::Unlock()
{
	if(fIamLocked)
	{
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << __PRETTY_FUNCTION__ << "Giving up the lock";
			LOGPZ_DEBUG(loggerlock,sout.str())
		}
#endif
		pthread_mutex_unlock(fReference->Mutex());
		fIamLocked = false;
	}
}


#endif /* OOPLOCK_H_ */
