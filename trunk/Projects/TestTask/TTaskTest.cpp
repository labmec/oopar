#include "TTaskTest.h"



OOPMReturnType TTaskTest::Execute(){
	cout << "Executing Task " << Id() << " Goint out in 5 secs\n";
	cout.flush();
	sleep(1);
	IncrementWriteDependentData();
	return ESuccess;
}

void TTaskTest::Write(TPZStream & buf, int withclassid){
	OOPTask::Write(buf, withclassid);
	int clsid = ClassId();
	buf.Write(&clsid);
}
void TTaskTest::Read(TPZStream & buf, void * context){
	OOPTask::Read(buf, context);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
}
