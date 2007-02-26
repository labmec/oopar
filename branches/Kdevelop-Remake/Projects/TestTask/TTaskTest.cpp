#include "TTaskTest.h"



OOPMReturnType TTaskTest::Execute(){
  int wait = 0;
  wait = 3;  
  cout << "Executing Task " << Id() << " Goint out in " << wait << " secs\n";
  cout.flush();
  sleep(wait);
  IncrementWriteDependentData();
  cout << "Leaving Executing Task " << endl;
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
