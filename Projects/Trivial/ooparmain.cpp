
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "oopstorage.h"
#include "oopfilecomm.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "ooptask.h"
#include <errno.h> 

#include "fluxdefs.h"
#include "tmultitask.h"
#include "tmultidata.h"
#include "tresultdata.h"

OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;

#include <iostream>

using namespace std;

//clock_t first_time;
void ParAddClass();

	

//int xpvmforce = 0;
int main(int argc, char *argv[]) {
/*	first_time=clock();
	struct tm *data_hora;
	time_t temporiz;
	time (&temporiz);
	data_hora = localtime( &temporiz );
*/
	ParAddClass();
	//Isso para cada classe !
	OOPStorageBuffer::AddClassRestore(TMULTITASK_ID, TMultiTask::Restore);//,"TBuildGridTask");

	CM = new OOPFileComManager();
	CM->Initialize( argv[0], 0 );
	TM = new OOPTaskManager(CM->GetProcID());
	DM = new OOPDataManager(CM->GetProcID(), TM);

	//Criar tasks e datas
	//submeter aos respectivos managers,
	//disparar o processo.

	/*char arqo[80];
    char hname[8];
	memset(hname,0,8);
    gethostname(hname,6);
	sprintf(arqo, "sp%s%d.out",hname,CM->GetProcID());
	freopen(arqo, "w", stdout); 
	cout<<"Begin:\t"<<asctime(data_hora)<<endl;

	// Abre um arquivo com a extensao igual ao numero deste processo.
	char str[128];
	sprintf( str, "saida%s.%03d", hname, CM->GetProcID() );
//	Err.GSetErrorFile( str );
//	Err.Debug( "\nProcesso %d:\n\n", CM->GetProcID() );
  */
    cout << "Teste" << endl;
	//TDataVersion::main();
	//TDataManager::main();
	OOPTaskManager::main();
	//TM->Execute();

	delete DM;
	delete TM;
	delete CM;

	//cout.flush();
	return 0;
}


