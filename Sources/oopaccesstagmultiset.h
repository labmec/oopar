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

#include "oopaccesstag.h"
class OOPDataManager;
/**
 * Implements multi set of data dependency objects.
 */
class OOPAccessTagMultiSet{
public:
  /**
   * Simple constructor
   * Internal initialization performed
   */
  OOPAccessTagMultiSet();
  /**
   * Destructor
   */
  ~OOPAccessTagMultiSet();
  /**
   * Inserts a new OOPAccessTag on the multiset container
   * @param tag : OOPAccessTag to be inserted on the multiset
   */
  void InsertTag(const OOPAccessTag & tag)
  {
    fTagMultiSet.insert(tag);
  }
  /**
   * Returns the amount of elements currently stored on the multiset container
   */
  int Count()
  {
    return fTagMultiSet.size();
  }
  /**
   * Returns an OOPAccessTag compatible with the given parameters.
   * @param version : OOPDataVersion against which the possible OOPAccessTag is checked.
   * @param need : The AccessMode required against which the possible OOPAccessTag is checked.
   */
  OOPAccessTag GetCompatibleRequest(const OOPDataVersion & version, OOPMDataState need);
  /**
   * Returns an Incompatible OOPAccessTag.
   * Incompatibility is checked against the OOPDataVersion parameter.
   */
  OOPAccessTag IncompatibleRequest(OOPDataVersion & version);
  /**
   * Todos diferente de:
   * ToProc - enviar
   * Iguais ao DM->GetProcID() - enviar filtrado
   */
  void TransferRequests(int ToProc);
  /**
   * Prints the content of the multiset
   */
  void Print(std::ostream & out) const;

  /// Verifies whether a similar access request exists within the list of requests
  bool HasSimilarRequest(OOPAccessTag tag, int processor);

  /// generates the set of accesstags that need to be sent when changing the owning processor
  void GetProcessorAccessRequests(int processor, std::set<OOPAccessTag> &requests, int localprocessor);

private:
  /**
   * Container for the OOPAccessTag objects.
   * MultiSet container, see STL documentation for MultiSet details
   */
  multiset<OOPAccessTag> fTagMultiSet;

};

#endif
