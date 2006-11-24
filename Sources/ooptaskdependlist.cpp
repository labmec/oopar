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
    td.fVersion = datalist.Dep(i).Version();
    if(datalist.Dep(i).ObjPtr())
    {
      td.fData = datalist.Dep(i).ObjPtr()->Ptr(td.fVersion);
      if(!td.fData)
      {
#ifdef LOGPZ  
  {
    stringstream sout;
    sout << __PRETTY_FUNCTION__ << " Lacking a pointer to the saveable object ";
    LOGPZ_ERROR(logger,sout.str());
  }
#endif  
      }
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
OOPDataVersion & OOPTaskDependList::Version(int i){
  return fDependList[i].fVersion;
}
TPZAutoPointer<TPZSaveable> OOPTaskDependList::ObjPtr(int i){
  if((unsigned)i< fDependList.size())
  {
#ifdef LOGPZ  

    {
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << " returning a pointer of classid ";
      if(fDependList[i].fData) sout << fDependList[i].fData->ClassId();
      LOGPZ_DEBUG(logger,sout.str());
    }
#endif  
    return fDependList[i].fData;
  }
  else
  {
#ifdef LOGPZ
    {
      stringstream sout;
      sout << __PRETTY_FUNCTION__ << " parameter out of range " << i << " size " <<
        fDependList.size();
      LOGPZ_DEBUG(logger,sout.str());
    }
#endif
  }
  return NULL;
}
OOPMDataState OOPTaskDependList::AccessType(int i){
  return fDependList[i].fAccess;
}

