#ifndef PARDEFSH
#define PARDEFSH
/*enum OOPMDMOwnerMessageType
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
};*/
//BlockingReadAccess concept no longer exists
//longhin: 30/01/2003
/**
 * Describes the possible access states for a data
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
//#definE NUMOBJECTS  0x100000
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

#define OOPDOUBLE_ID 3009
#define OOPINT_ID 3010



#endif
