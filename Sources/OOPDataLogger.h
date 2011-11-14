/* @file
 * @brief Defines the LogStructure struct and OOPDataLogger class.
 */

#ifndef OOPDATALOGGER_H
#define OOPDATALOGGER_H

#include "oopdatamanager.h"

class OOPDMOwnerTask;
class OOPDMRequestTask;

/**
 * @brief Implements the structure of the log data (socket).
 * @ingroup managerdata
 */
struct LogStructure{
	int ProcId;
	OOPObjectId ObjId;
	string Action;
	string State;
	OOPDataVersion Version;
	OOPObjectId TaskId;
	int ProcoOrigin;
	string comment;
};

/**
 * @brief Implements the data logger.
 * @ingroup managerdata
 */
class OOPDataLogger {
public:
	void LogReleaseAccess(int proc, OOPObjectId & Id,
						  OOPMDataState state, int targetproc,
						  OOPObjectId & taskId,
						  OOPMDataState currentstate, OOPDataVersion & version);
	void LogSetVersion(int proc, OOPObjectId & Id, OOPDataVersion & oldver,
					   const OOPDataVersion & newver,
					   OOPMDataState state,
					   const OOPObjectId & TaskId);
	void LogGeneric(int proc, OOPObjectId & Id, char * msg);	
	void GrantAccessLog(int proc, 
						const OOPObjectId & objId,
						OOPMDataState mstate,
						const OOPDataVersion & version,
						int procorig);
	void GrantAccessLog(int proc, 
						const OOPObjectId & objId,
						OOPMDataState mstate,
						const OOPDataVersion & version,
						int procorig,
						OOPObjectId & taskId,
						OOPMDataState currentstate);
	void SubmitAccessRequestLog(int proc, 
								const OOPObjectId & objId,
								OOPMDMOwnerMessageType mtype,
								OOPMDataState mstate,
								OOPMDataState currentstate,
								const OOPDataVersion & version,
								int procorig, const OOPObjectId & taskId);
    /**
     * @brief Simple constructor 
     */
    OOPDataLogger();
	
	
    /**
     * @brief Constructor which takes the filename parameter
     * @param logfilename Logger will be generated on that file 
     */
    OOPDataLogger(char * logfilename);
	~OOPDataLogger();
	
	/**
 	 * @brief Generates logs on the GrantAccess method of the OOPMetaData class
	 * @param town OOPDMOwnerTask object upon which the logging information will
	 * be obtained
	 * @since 16/09/2003
	 * @author Gustavo Longhin
 	 */
	void SendGrantAccessLog(OOPDMOwnerTask *town, int processor);
	
    /**
     * @param town OwnerTask to be 
     */
    void SendOwnTask(OOPDMOwnerTask *town);
    void ReceiveOwnTask(OOPDMOwnerTask *town);
	
	
	void SendReqTask(OOPDMRequestTask *req);
	void ReceiveReqTask(OOPDMRequestTask *req);
	
    void CancelTask(OOPMetaData &data);
	
private:
	/**
 	 * @brief Generates the header on the logger file
	 */
	void GenerateHeader();
	void PrintLog();
	char * GetStateName(OOPMDataState state);
	
    /**
     * @brief Output stream for the logging generation
     */
    ofstream fLogger;
	ofstream fMetaDataLogger;
	LogStructure fLog;
	
};

extern OOPDataLogger * LogDM;

#endif //OOPDATALOGGER_H
