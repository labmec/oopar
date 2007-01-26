//
// C++ Interface: oopaccesstaglist
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPACCESSTAGLIST_H
#define OOPACCESSTAGLIST_H

#include "oopaccesstag.h"
#include <set>
#include <vector>

class OOPDataManager;
extern OOPDataManager *DM;



/**
Implements a list of OOPAccessTags
*/
class OOPAccessTagList{
public:
  OOPAccessTagList();

  ~OOPAccessTagList();

  void GrantAccess(const OOPAccessTag & granted);
  void Clear();
  void AppendTag(const OOPAccessTag & tag);
  bool CanExecute();
  TPZSaveable * ObjectPtr(int idepend)
  {
    return fTagList[idepend].GetPointer();
  }
  void IncrementWriteDependent();
  int Count()
  {
    return fTagList.size();
  }
  void Print(std::ostream & out);
  void Read (TPZStream & buf, void *context);
  void Write (TPZStream & buf);
private:
  vector<OOPAccessTag> fTagList;

};

#endif

