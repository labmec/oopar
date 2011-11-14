/**
 * @file
 */
#ifndef OOPWAITTASK_H
#define OOPWAITTASK_H


#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include <ooptask.h>
class OOPStorageBuffer;

/**
 * @brief This task creates an entry point into the OOPAR system. The Task contains two central methods. Wait() will cause the calling 
 * thread to sleep till the data dependency of the task is satisfied. Finish() will release the datadependency from the OOPAR 
 * system.
 * @note Uses semaphore for synchronization.
 * @author Philippe R. B. Devloo
 * @ingroup managertask
 */
class OOPWaitTask : public OOPTask
{
	/**
	 * @brief Semaphore for the Main Thread Execution
	 */
	boost::interprocess::interprocess_semaphore * fMainSemaphore;
	/**
	 * @brief Semaphore for the WaitTask execution
	 */
	boost::interprocess::interprocess_semaphore * fExecSemaphore;
public:
	OOPWaitTask() {  }
	OOPWaitTask(int Procid);
	
	~OOPWaitTask();
	
	virtual void Write(TPZStream & buf, int withclassid);
	virtual void Read(TPZStream & buf, void * context);
	virtual int ClassId() const;
	virtual OOPMReturnType Execute();
	void Finish();
	void Wait();
	using OOPTask::fProc;
	
};

#endif
