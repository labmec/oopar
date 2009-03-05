//
// C++ Interface: oopdelobjecttask
//
// Description: 
//
//
// Author: Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPDELOBJECTTASK_H
#define OOPDELOBJECTTASK_H

#include <ooptask.h>


/**
  Implements a Task specific for the deletion of objects.
  It actually does not delete the object, in its Execute method it posts the deletion request for the object.
  A user should never use a OOPDelObjectTask, its use is internal to OOPar

  @author Gustavo Camargo Longhin <longhin@labmec.fec.unicamp.br>
*/
class OOPDelObjectTask : public OOPTask
{
private:
  /**
   * Identifies the To Be Killed object
   */
  OOPObjectId fTargetObject;
public:
  OOPDelObjectTask();
  OOPDelObjectTask(OOPObjectId & Id);

  ~OOPDelObjectTask();
  virtual void Write(TPZStream & buf, int withclassid);
  virtual void Read(TPZStream & buf, void * context);
  virtual int ClassId() const;
  virtual OOPMReturnType Execute();
  using OOPTask::fProc;
  static TPZSaveable *Restore (TPZStream & buf, void *context = 0);
};
#endif
