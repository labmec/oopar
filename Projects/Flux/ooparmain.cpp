#include "TPartitionRelation.h"
#include "oopstorage.h"
#include "oopfilecomm.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include <iostream>

#include "TParAnalysis.h"
#include "TParCompute.h"
#include "TLocalCompute.h"
#include "TTaskComm.h"

#include "fluxdefs.h"


OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;

using namespace std;


void ParAddClass();

int main(int argc, char *argv[]){


	OOPReceiveStorage::AddClassRestore(TPARCOMPUTE_ID,
										TParCompute::Restore);
	OOPReceiveStorage::AddClassRestore(TPARANAYSIS_ID,
										TParAnalysis::Restore);
	OOPReceiveStorage::AddClassRestore(TLOCALCOMPUTE_ID,
										TLocalCompute::Restore);
	OOPReceiveStorage::AddClassRestore(TTASKCOMM_ID,
										TTaskComm::Restore);

	CM = new OOPFileComManager();
	CM->Initialize( argv[0], 0 );
	TM = new OOPTaskManager(CM->GetProcID());
	DM = new OOPDataManager(CM->GetProcID());

	TParAnalysis * paranalysis = new TParAnalysis(DM->GetProcID(), 5);
	paranalysis->Submit();

	TM->Execute();
	
	delete DM; 
	delete TM;
	delete CM;

	//cout.flush();
	return 0;

}
