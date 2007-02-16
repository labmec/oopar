//
// C++ Implementation: oopaccesstaglist
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopaccesstaglist.h"
#include "oopdatamanager.h"

OOPAccessTagList::OOPAccessTagList()
{
}


OOPAccessTagList::~OOPAccessTagList()
{
}

void OOPAccessTagList::AppendTag(const OOPAccessTag & tag)
{
  fTagList.push_back(tag);
}


void OOPAccessTagList::Clear() 
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  std::set<OOPObjectId> lset;
  for(;it!=fTagList.end();it++)
  {
    lset.insert(it->Id());
    if(it->AccessMode() == EWriteAccess)
    {
      DM->PostData(*it);
    }
    it->ClearPointer(); 
  }
//  fTagList.clear();
  DM->ObjectChanged(lset);
}

void OOPAccessTagList::GrantAccess(const OOPAccessTag & granted)
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  for(;it!=fTagList.end();it++)
  {
    if(it->IsMyAccessTag(granted)) 
    {
      *it = granted;
    }
  }
}
bool OOPAccessTagList::CanExecute()
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  for(;it!=fTagList.end();it++)
  {
    if(!it->CanExecute()) return false;
  }
  return true;
}

void OOPAccessTagList::IncrementWriteDependent()
{
  std::vector<OOPAccessTag>::iterator it = fTagList.begin();
  for(;it!=fTagList.end();it++)
  {
    if(it->AccessMode() == EWriteAccess)
    {
      it->IncrementVersion();
    }
  }
  
}

void OOPAccessTagList::SubmitIncrementedVersions()
{
#warning "Not sure here see questions "
/**
  Para cada Tag, na lista, verificar quem tinha acesso escrita ?
  Para cada um submeter ao MetaData a nova versão e o novo ponteiro ?
  Ou ... Apenas postar na fila de mensagens que existe dado alterado ?
  Ou ainda, fazer as duas coisas ?
  Não há chamadas para esse método, Must Delete ?
 */
}
void OOPAccessTagList::Print(std::ostream & out)
{
  std::vector<OOPAccessTag>::iterator it;
  for(it = fTagList.begin(); it != fTagList.end(); it++)
  {
    it->Print(out);
  }
}
void OOPAccessTagList::Write (TPZStream  & buf, int withclassid)
{
  int size = fTagList.size();
  buf.Write(&size);
  int i = 0;
  for(i=0;i<size;i++)
  {
    fTagList[i].Write(buf, withclassid);
  }
}
void OOPAccessTagList::Read(TPZStream & buf, void *context)
{
  int size = 0;
  buf.Read(&size);
  fTagList.resize(size);
  int i = 0;
  for(i=0;i<size;i++)
  {
    fTagList[i].Read(buf, context);
  }
}

void OOPAccessTagList::PostRequests(OOPObjectId & Id)
{
  int i = 0;
  for(i=0;i<fTagList.size();i++)
  {
    fTagList[i].SetTaskId(Id);
    DM->PostAccessRequest( fTagList[i]);
  }
}
