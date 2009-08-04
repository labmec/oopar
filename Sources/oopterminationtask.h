/*
 * oopterminationtask.h
 *
 *  Created on: Aug 1, 2009
 *      Author: phil
 */

#ifndef OOPTERMINATIONTASK_H_
#define OOPTERMINATIONTASK_H_

#include "ooptask.h"

/**
 * Implements a class which is responsible for turning Off both TM and DM.
 * In its execute method it calls SetKeepGoing() for both managers setting them to false.
 */
class OOPTerminationTask:public OOPTask
{
public:

	enum ETerminate { EStopSending, EStopSendingConfirmation, EShutdown };
  ~OOPTerminationTask ();
  /**
   * Simple constructor
   */
  OOPTerminationTask ()
  {
  }
  OOPTerminationTask (int ProcId);
  OOPTerminationTask (const OOPTerminationTask & term);

  /// Indicate which part of the shutdown process this termination task refers to
  void SetStatus(ETerminate status);

  /// The process origin needs to be set explicitly
  void SetProcOrigin(int procorigin)
  {
	  fProcOrigin = procorigin;
  }
  /**
   * Returns execution type
   */
  OOPMReturnType Execute ();
  virtual int ClassId () const
  {
    return TTERMINATIONTASK_ID;
  }
   ;
  void Write (TPZStream & buf, int withclassid);
  void Read (TPZStream & buf, void *context = 0);
  long int ExecTime ();
  static TPZSaveable *Restore (TPZStream & buf, void *context = 0);
private:

	// The message which is transferred by this task
	int fStatus;
	/// The processor which originated the termination sequence
	int fProcOrigin;
};

#endif /* OOPTERMINATIONTASK_H_ */

