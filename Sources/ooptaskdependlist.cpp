//
// C++ Implementation: ooptaskdependlist
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ooptaskdependlist.h"
#include "oopmetadata.h"

OOPTaskDependList::OOPTaskDependList()
{
}
OOPTaskDependList::OOPTaskDependList(const OOPTaskDependList &copy){
  fDependList = copy.fDependList;
}
OOPTaskDependList::~OOPTaskDependList()
{
}


void OOPTaskDependList::SetDependency(const OOPMDataDependList &datalist) {
  int i = 0;
#warning "Wilderness of consts ! This is absolutely not necessary, consts are forcing me to do this !"
  OOPMDataDependList dplst = datalist;
  int size = dplst.NElements();
  for(i = 0; i < size ;i++){
    OOPTaskData td;
    td.fAccess = dplst.Dep(i).State();
    td.fData = dplst.Dep(i).ObjPtr()->Ptr();
    td.fVersion = dplst.Dep(i).Version();
  }
}

void OOPTaskDependList::Print(std::ostream &out) const{
  int i = 0;
  for(i = 0 ; i < (signed)fDependList.size() ; i++){
    out << "Implement me\n";
  }
}

int OOPTaskDependList::NDepend() const{
  return fDependList.size();
}
void OOPTaskDependList::IncrementVersion(int i){
  fDependList[i].fVersion.Increment();
}
TPZAutoPointer<TPZSaveable> OOPTaskDependList::ObjPtr(int i){
  return fDependList[i].fData;
}
OOPMDataState OOPTaskDependList::AccessType(int i){
  return fDependList[i].fAccess;
}

