#ifndef PARDEFSH
#define PARDEFSH

enum OOPMDMOwnerMessageType {
	ENoMessage,
	ENotifyAccessState,
	ECancelReadAccess,
	ECancelReadAccessConfirmation,
	ETransferOwnership,
	ETransferOwnershipConfirmation,
	EGrantReadAccess,
	EGrantReadAccessConfirmation,
	EGrantVersionAccess,
	EGrantVersionAccessConfirmation,
	ENotifyCreateObject,
	ENotifyDeleteObject,
	ENotifyDeleteObjectConfirmation
};

enum OOPDMRequestMessageType {
	ENoRequest,
	ERequestReadAccess,
	ERequestWriteAccess,
	ERequestDelete
};
//BlockingReadAccess concept no longer exists
//longhin: 30/01/2003
/**
 * Describes the possible access states for a data
 */	
enum OOPMDataState {
	ENoAccess,
	EReadAccess,
	EWriteAccess,
	EVersionAccess
};

enum OOPMTransitionState {
	ENoTransition,
	EReadTransition,
	EOwnerTransition,
	EDeleteTransition
};

enum OOPMReturnType {
	ESuccess,
	EFail,
	EContinue
};

#define NUMOBJECTS  0x100000

#define TTASK_ID 3000
#define TDAEMONTASK_ID 3001
#define TTMMESSAGETASK_ID 3002
#define TDMOWNERTASK_ID 3003
#define TDMREQUESTTASK_ID 3004
#endif

