//
// C++ Interface: oopsnapshottask
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPSNAPSHOTTASK_H
#define OOPSNAPSHOTTASK_H

#include "ooptask.h"

class OOPTaskManager;
class OOPDataManager;


/**
Implements a task which on its execute method, takes a snapshot of the TM and DM data strutucture.
 * Still experimental

@author Gustavo C Longhin
*/
class OOPSnapShotTask : public OOPTask{
public:
  OOPSnapShotTask(){};
  OOPSnapShotTask(int ProcId) : OOPTask(ProcId) {};

  virtual ~OOPSnapShotTask();
  /**
   * Implements the functionality of the class
   * In that case creates the snapshot of both DM and TM
   * @since 11/2003
   */
  virtual OOPMReturnType Execute ();

  virtual int ClassId () const
  {
    return TSNAPSHOTTASK_ID;
  }
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
  void Write (TPZStream & buf,int withclassid);
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
  void Read (TPZStream & buf, void * context);

};
template class TPZRestoreClass<OOPSnapShotTask, TSNAPSHOTTASK_ID>;

extern OOPTaskManager *TM;
extern OOPDataManager *DM;

#endif
