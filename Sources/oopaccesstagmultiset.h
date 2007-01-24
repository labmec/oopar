//
// C++ Interface: oopaccesstagmultiset
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPACCESSTAGMULTISET_H
#define OOPACCESSTAGMULTISET_H

/**
Implements multi set of data dependency objects.
*/
class OOPAccessTagMultiSet{
public:
  OOPAccessTagMultiSet();
  ~OOPAccessTagMultiSet();
  void InsertTag(OOPAccessTag & tag);
  int Count();
  OOPAccessTag GetCompatibleRequest(OOPDataVersion & version, MDataState need);
  /**
   * Todos diferente de:
   * ToProc - enviar
   * Iguais ao DM->GetProcID() - enviar filtrado
   */
  void TransferRequests(int ToProc);
private:
  multiset<OOPAccessTag> fTagMultiSet;
};

#endif
