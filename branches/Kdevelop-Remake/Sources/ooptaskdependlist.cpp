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

#include "pzlog.h"

#ifdef LOGPZ
using namespace log4cxx;
using namespace
  log4cxx::helpers;
static LoggerPtr
logger (Logger::getLogger ("OOPAR.OOPTaskDependList"));
#endif

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
  int size = datalist.NElements();
  fDependList.resize(size);
  for(i = 0; i < size ;i++){
    OOPTaskData td;
    td.fAccess = datalist.Dep(i).State();
    if(datalist.Dep(i).ObjPtr())
    {
      td.fData = datalist.Dep(i).ObjPtr()->Ptr();
    }
    else
    {
#ifdef LOGPZ  
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Lacking a pointer to the metadata object ";
    LOGPZ_DEBUG(logger,sout.str());
  }
#endif  

    }
    td.fVersion = datalist.Dep(i).Version();
    fDependList[i]=td;
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

