//
// C++ Implementation: ooppardefs
//
// Description: 
//
//
// Author: Philippe R. B. Devloo <phil@fec.unicamp.br>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ooppardefs.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"

void RegisterOOParRestore()
{
  OOPReceiveStorage::AddClassRestore(TDMOWNERTASK_ID, OOPDMOwnerTask::Restore);
  OOPReceiveStorage::AddClassRestore(TDMREQUESTTASK_ID, OOPDMRequestTask::Restore);
  OOPReceiveStorage::AddClassRestore(TTERMINATIONTASK_ID, OOPTerminationTask::Restore);

}
