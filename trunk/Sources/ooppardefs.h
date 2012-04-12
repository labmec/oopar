#ifndef PARDEFSH
#define PARDEFSH

/**
 * @addtogroup managertask
 * @{
 */
enum OOPMDMOwnerMessageType
{
	ENoMessage,
	ETransferOwnership,
	EGrantReadAccess,
};

enum OOPDMRequestMessageType
{
	ENoRequest,
	ERequestReadAccess,
	ERequestWriteAccess,
};

/**
 * @brief Describes the possible access states for a data
 */
enum OOPMDataState
{
	ENoAccess,
	EReadAccess,
	EWriteAccess,
	EDelete
};

enum OOPMReturnType
{
	ESuccess,
	EFail,
	EContinue
};


#define OOP_MAX_BUFFER_SIZE 5120
#define NUMOBJECTS  1000
#define OOPSAVEABLE_ID -1
#define TTASK_ID 3000
#define TDAEMONTASK_ID 3001
#define TTMMESSAGETASK_ID 3002
#define TDMOWNERTASK_ID 3003
#define TDMREQUESTTASK_ID 3004
#define OOPMETADATA_ID 3005
#define TTERMINATIONTASK_ID 3006
#define TSNAPSHOTTASK_ID 3007
#define TDELOBJECTTASK_ID 3008

#define OOPDOUBLE_ID 3012
#define OOPINT_ID 3010

#define OOPCOLLECTOR_TASK_ID 3011

/**
 * @}
 */

#endif
