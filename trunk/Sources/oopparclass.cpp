
#include "ooptask.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"


void ParAddClass ()
{
	OOPReceiveStorage::AddClassRestore (TTASK_ID, OOPTask::Restore);
	OOPReceiveStorage::AddClassRestore (TDAEMONTASK_ID,
					    OOPDaemonTask::Restore);
	// OOPReceiveStorage::AddClassRestore(TTMMESSAGETASK_ID,TTMMessageTask::Restore);
	OOPReceiveStorage::AddClassRestore (TDMREQUESTTASK_ID,
					    OOPDMRequestTask::Restore);
	OOPReceiveStorage::AddClassRestore (TDMOWNERTASK_ID,
					    OOPDMOwnerTask::Restore);
}
