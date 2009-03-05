//
// C++ Implementation: oopsnapshottask
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopsnapshottask.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "ooptmlock.h"

OOPSnapShotTask::~OOPSnapShotTask()
{
}


OOPMReturnType OOPSnapShotTask::Execute(){
  cout << "Executing Task SnapShotTask on Processor " << fProc << endl;
  cout.flush();
  OOPTMLock lock;
  DM->SnapShotMe(cout);
  TM->SnapShotMe(cout);
  IncrementWriteDependentData();
  return ESuccess;
}

void OOPSnapShotTask::Write(TPZStream & buf, int withclassid){
  OOPTask::Write(buf, withclassid);
  int clsid = ClassId();
  buf.Write(&clsid);
}
void OOPSnapShotTask::Read(TPZStream & buf, void * context){
  OOPTask::Read(buf, context);
  int clsid = 0;
  buf.Read(&clsid);
  if(ClassId()!=clsid){
    cout << "ClassId Missmatch\n";
  }
}
extern OOPTaskManager *TM;
extern OOPDataManager *DM;
