/** @file */

#include "TTaskTest.h"

int TTaskTest::DerivedFrom(char *classname){
	if(!strcmp(ClassName(),classname)) return 1;
	return TPZSaveable::DerivedFrom(classname);
}

int TTaskTest::DerivedFrom(long Classid){
	if(Classid == GetClassID()) return 1;
	return TPZSaveable::DerivedFrom(Classid);
}

void TTaskTest::Write(TPZStream &buf, int classid)
{
	OOPTask::Write(buf,classid);
}

void TTaskTest::Read(TPZStream &buf, void *context)
{
	OOPTask::Read(buf,context);
	long clid;
	buf->UpkLong(&clid);
	if(clid != LONGVEC_ID) {
		cout << __PRETTY_FUNCTION__ << "wrong class id" << endl;
		exit(-1);
	}
}

OOPMReturnType TTaskTest::Execute(){
	cout << "Calling Execute method on TMultiTask" << endl;
	deque<OOPMDataDepend>::iterator i;
	for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
		if (i->fNeed == EWriteAccess){
			//dat->IncrementVersion();
			//DM->GetObjPtr(i->fDataId)->IncrementVersion();
			DM->IncrementVersion(i->fDataId);
		}
		
	}
	TaskFinished();
	return ESuccess; // execute the task, verifying that
}

long TTaskTest::ExecTime() {
	return -1;
	
}

void TTaskTest::SetData(TMultiData & data, TResultData & resdata){
	//Identificar qual a versão do dado
}

TTaskTest::TTaskTest(int proc) : OOPTask(proc){
	fProc= proc;
}

OOPSaveable * TTaskTest::Restore(OOPReceiveStorage *buf){
	TTaskTest *t = new TTaskTest(0);
	t->Unpack(buf);
	return t;
}
