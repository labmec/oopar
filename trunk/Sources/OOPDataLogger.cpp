/* Generated by Together */

#include "OOPDataLogger.h"

using namespace std;
class OOPDMOwnerTask;

void OOPDataLogger::GenerateHeader(){
	fLogger << "ProcId\t"
	<< "ObjectId\t"
	<< "Action\t"
	<< "Data State\t"
	<< "Version\t"
	<< "Task Id\t"
	<< "ProcOrigin\t"
	<< "comment\n";
}
OOPDataLogger::OOPDataLogger(){
	//Initialize ostream pointer
	fLogger.open("datalogger.log");
	GenerateHeader();
}
OOPDataLogger::OOPDataLogger(char * logfilename){
	fLogger.open(logfilename);
	GenerateHeader();
} 
OOPDataLogger::~OOPDataLogger(){
	fLogger.flush();
	fLogger.close();
}

void OOPDataLogger::LogReleaseAccess(int proc, OOPObjectId & Id, OOPMDataState state, int targetproc, OOPObjectId & taskId, OOPMDataState currentstate, OOPDataVersion & version){
	fLogger << proc << "\tId " << Id << "\tReleasing access ";
	switch (state )
	{
		case  ENoAccess:
			fLogger << "ENoAccess\t";
			break;
		case  EReadAccess:
			fLogger << "EReadAccess ";
			fLogger << "From processor " << targetproc << endl;
			break;
		case  EWriteAccess:
			fLogger << "EWriteAccess ";
			fLogger << "From task " << taskId << endl;
			break;
		case  EVersionAccess:
			fLogger << "EVersionAccess ";
			fLogger << "From task " << taskId << endl;
			break;
	}/*
	fLogger << "\t" << currentstate;
	fLogger << "\tVersion " << version;
	fLogger << "\tTaskId " << taskId;
	fLogger << "\t\tCheck if Action and State are consistent\n";*/
	
}
//OK
void OOPDataLogger::LogSetVersion(int proc, OOPObjectId & Id, OOPDataVersion & oldver,
					const OOPDataVersion & newver,
					OOPMDataState state,
					const OOPObjectId & TaskId){
	fLogger << proc << "\tId " << Id << "\tSetting old version "
						<< oldver << " To new version " << newver
						<< "\t" << GetStateName(state)
						<< "\t\ttask " << TaskId << endl;
	fLogger.flush();						
}
//OK

void OOPDataLogger::LogGeneric(int proc, OOPObjectId & Id, char * msg){
#warning "Still can be enriched"
	fLogger << proc;
	fLogger << "\tId " << Id << "\t";
	fLogger << msg << endl;
}
void OOPDataLogger::SendGrantAccessLog(OOPDMOwnerTask *town, int processor){
	town->LogMe(fLogger);
	fLogger << endl;
	fLogger.flush();
}
void OOPDataLogger::GrantAccessLog(int proc, 
									const OOPObjectId & objId,
									OOPMDataState mstate,
									const OOPDataVersion & version,
									int procorig){
	//town->LogMe(fLogger);
	fLogger << proc << "\t";
	fLogger << "Id " << objId << "\t";
	fLogger << "Granting ";
	fLogger << GetStateName(mstate) << "\t";
	fLogger << "Version " << version << "\t";
	fLogger << "Unknown\t";
	fLogger << "ProcOrigin " << procorig;
	fLogger << "\n";
	fLogger.flush();
}

//Falta comment
void OOPDataLogger::GrantAccessLog(int proc, 
									const OOPObjectId & objId,
									OOPMDataState mstate,
									const OOPDataVersion & version,
									int procorig,
									OOPObjectId & taskId,
									OOPMDataState currentstate){
	//town->LogMe(fLogger);
	fLogger << proc << "\t";
	fLogger << "Id " << objId << "\t";
	fLogger << "Granting ";
	fLogger << GetStateName(mstate) << "\t";										
	fLogger << GetStateName(currentstate);										
	fLogger << "\tVersion " << version << "\t";
	fLogger << "To Task " << taskId << "\t";
	fLogger << "ProcOrigin " << procorig;
	fLogger << "\n";
	fLogger.flush();
}

void OOPDataLogger::SendOwnTask(OOPDMOwnerTask *town){
	town->LogMe(fLogger);
	fLogger << endl;
	fLogger.flush();
}
void OOPDataLogger::ReceiveOwnTask(OOPDMOwnerTask *town){
	town->LogMeReceived(fLogger);
	fLogger << endl;
	fLogger.flush();
}
void OOPDataLogger::SendReqTask(OOPDMRequestTask *req){
	req->LogMe(fLogger);
	fLogger.flush();
}
void OOPDataLogger::SubmitAccessRequestLog(int proc, 
									const OOPObjectId & objId,
									OOPMDMOwnerMessageType mtype,
									OOPMDataState mstate,
									OOPMDataState currentstate,
									const OOPDataVersion & version,
									int procorig, const OOPObjectId & taskId){
	fLogger << proc << "\t";
	fLogger << "Id " << objId;
	fLogger << "\tSubmitting ";
	fLogger << GetStateName(mstate );
	fLogger << "\t" << GetStateName(currentstate);
	fLogger << "\tVersion " << version << "\t";
	fLogger << "To task " << taskId;
	fLogger << "\n";
	fLogger.flush();
}

void OOPDataLogger::ReceiveReqTask(OOPDMRequestTask *req){
	req->LogMe(fLogger);
	fLogger << "\t\treceived\n";
	fLogger.flush();
}

char * OOPDataLogger::GetStateName(OOPMDataState state){
	switch (state )
	{
		case  ENoAccess:
			return "ENoAccesst";
			break;
		case  EReadAccess:
			return "EReadAccess";
			break;
		case  EWriteAccess:
			return "EWriteAccess";
			break;
		case  EVersionAccess:
			return "EVersionAccess";
			break;
	}
	return "Unidentified";
}
