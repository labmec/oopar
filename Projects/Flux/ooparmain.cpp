#include "TPartitionRelation.h"
#include "oopstorage.h"
#include "oopfilecomm.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include <iostream>
OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;

using namespace std;


void ParAddClass();

int main(int argc, char *argv[]){
	//int TMULTITASK_ID = 10000;
	//OOPReceiveStorage::AddClassRestore(TMULTITASK_ID, TParCompute::Restore);//,"TBuildGridTask");

	CM = new OOPFileComManager();
	CM->Initialize( argv[0], 0 );
	TM = new OOPTaskManager(CM->GetProcID());
	DM = new OOPDataManager(CM->GetProcID());

	TPartitionRelation * part;
	part = TPartitionRelation::CreateRandom(20);
	
	delete DM;
	delete TM;
	delete CM;

	//cout.flush();
	return 0;

}


