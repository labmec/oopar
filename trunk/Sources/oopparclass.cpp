#include "ooptask.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"
void ParAddClass ()
{
	OOPStorageBuffer::AddClassRestore (TTASK_ID, OOPTask::Restore);
	OOPStorageBuffer::AddClassRestore (TDAEMONTASK_ID,
					    OOPDaemonTask::Restore);
	// OOPStorageBuffer::AddClassRestore(TTMMESSAGETASK_ID,TTMMessageTask::Restore);
	OOPStorageBuffer::AddClassRestore (TDMREQUESTTASK_ID,
					    OOPDMRequestTask::Restore);
	OOPStorageBuffer::AddClassRestore (TDMOWNERTASK_ID,
					    OOPDMOwnerTask::Restore);
}
