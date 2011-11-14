/**
 * @file 
 */
#ifndef OOPDELOBJECTTASK_H
#define OOPDELOBJECTTASK_H

#include <ooptask.h>


/**
 * @brief Implements a Task specific for the deletion of objects.
 * @ingroup task
 * @author Gustavo Camargo Longhin
 */
/**
 * It actually does not delete the object, in its Execute method it posts the deletion request for the object.
 * A user should never use a OOPDelObjectTask, its use is internal to OOPar
 */
class OOPDelObjectTask : public OOPTask
{
private:
	/**
	 * @brief Identifies the To Be Killed object
	 */
	OOPObjectId fTargetObject;
public:
	OOPDelObjectTask();
	OOPDelObjectTask(OOPObjectId & Id, int procid);
	
	~OOPDelObjectTask();
	virtual void Write(TPZStream & buf, int withclassid);
	virtual void Read(TPZStream & buf, void * context);
	virtual int ClassId() const;
	virtual OOPMReturnType Execute();
	using OOPTask::fProc;
	static TPZSaveable *Restore (TPZStream & buf, void *context = 0);
};

#endif
