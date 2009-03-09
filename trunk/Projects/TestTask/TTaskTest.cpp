#include "TTaskTest.h"
#include "OOPInt.h"
#include "OOPTaskManager.h"
//#include "OOPCommManager.h"





OOPMReturnType TTaskTest::Execute()
{
/*
  int wait = 0;
  wait = 1;  
  cout << "Executing Task " << Id() << " Goint out in " << wait << " secs\n";
  OOPInt * lint = dynamic_cast<OOPInt*>(fDependRequest.ObjectPtr( 0));
  lint->fValue = lint->fValue + 1000;
  cout << "Value of Int " << lint->fValue << endl;;
  cout.flush();
//  sleep(wait);
//  IncrementWriteDependentData();
 */
  cout << "Trigering Termination Tasks\n " << endl;
	cout.flush();
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		OOPTerminationTask * tt = new OOPTerminationTask(i);
		tt->Submit();
	}
  return ESuccess;
}

void TTaskTest::Write(TPZStream & buf, int withclassid){
	OOPTask::Write(buf, withclassid);
	m_Matrix.Write(buf,0);
	m_Mesh.Write(buf,0);
	int clsid = ClassId();
	buf.Write(&clsid);
}
void TTaskTest::Read(TPZStream & buf, void * context){
	OOPTask::Read(buf, context);
	m_Matrix.Read(buf,0);
	m_Mesh.Read(buf,0);
	int clsid = 0;
	buf.Read(&clsid);
	if(ClassId()!=clsid){
		cout << "ClassId Missmatch\n";
	}
}
template class TPZRestoreClass<TTaskTest, TTASKTEST_ID>;