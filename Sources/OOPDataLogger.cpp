/* Generated by Together */

#include "OOPDataLogger.h"

using namespace std;
class OOPDMOwnerTask;
OOPDataLogger::OOPDataLogger(){
	//Initialize ostream pointer
}
OOPDataLogger::OOPDataLogger(ofstream & out){
    //fLogger=out;
}
OOPDataLogger::OOPDataLogger(char * logfilename){
	fLogger.open(logfilename);
	LogDM=this;
} 
OOPDataLogger::~OOPDataLogger(){
	fLogger.flush();
	fLogger.close();
}

void OOPDataLogger::GrantAccessLog(OOPDMOwnerTask *town){
	town->LogMe(fLogger);
	fLogger << "\tsent\n";
	fLogger.flush();
}
void OOPDataLogger::SendOwnTask(OOPDMOwnerTask *town){
	town->LogMe(fLogger);
    fLogger << "\tsent\n";
	fLogger.flush();
}
void OOPDataLogger::ReceiveOwnTask(OOPDMOwnerTask *town){
	town->LogMe(fLogger);
	fLogger << "\treceived\n";
	fLogger.flush();
}
void OOPDataLogger::SendReqTask(OOPDMRequestTask *req){
	req->LogMe(fLogger);
    fLogger << "\tsent\n";
	fLogger.flush();
}
void OOPDataLogger::ReceiveReqTask(OOPDMRequestTask *req){
	req->LogMe(fLogger);
	fLogger << "\t\treceived\n";
	fLogger.flush();
}

