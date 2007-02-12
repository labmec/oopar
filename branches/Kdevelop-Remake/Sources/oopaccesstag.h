//
// C++ Interface: oopaccesstag
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OOPACCESSTAG_H
#define OOPACCESSTAG_H

#include <deque>
#include <set>
#include <list>
#include <iostream>
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "ooppardefs.h"

#include "tpzautopointer.h"
#include "pzsave.h"

//#include "oopdatamanager.h"
using namespace std;
class   OOPMetaData;
//extern OOPDataManager * DM;

/**
 * Implements a information tag concerning access requirements from Tasks to Data.
 * Assumes responsibility from three discontinued classe OOPAccessInfo,
 * OOPDataDepend and OOPTaskDepend.
 */
class OOPAccessTag{
public:
  OOPAccessTag();

  ~OOPAccessTag();
  operator bool ();
  bool operator < (const OOPAccessTag & compare) const{};
  bool CanExecute();
  int Proc() const
  {
   return fProcessor;
  }
private:
  /**
   * Describes the type of access state 
   */
  OOPMDataState fAccessMode;
  /**
   * Identifies the Data to which the current accesstag exists
   */
  OOPObjectId fObjectId;
  /**
   * Identifies the TaskId willing to access the data object 
   */
  OOPObjectId fTaskId;
  /**
   * Identifies in which version the task needs the data to be 
   */
  OOPDataVersion fVersion;
  /**
   * Processor which filed the request
   */
  int fProcessor;
  /**
   * AutoPointer to the object refered to by OOPObjectId
   */
  TPZAutoPointer<TPZSaveable> fObjectAutoPtr;
  /**
   * Constructor with initial parameters
       * @param TaskId Id of task requiring access on the data.
       * @param st Type of access on the data
       * @param version Version required on the data
       * @param proc Processor where the access should occur
   */
public:
  void Write (TPZStream  & buf, int withclassid);
  void Read (TPZStream & buf, void * context);
  TPZSaveable * GetPointer();
  bool IsMyAccessTag(const OOPAccessTag & granted);
  void ClearPointer()
  {
    fObjectAutoPtr = TPZAutoPointer<TPZSaveable>(0); 
  }
  
  void IncrementVersion()
  {
    fVersion++;
  }
  
  OOPObjectId Id() const
  {
    return fObjectId;
  }  
  OOPAccessTag (const OOPObjectId & TaskId,
                 const OOPMDataState & st,
                 const OOPDataVersion & version, int processor)
  {
    fTaskId = TaskId;
    fAccessMode = st;
    fVersion = version;
    fProcessor = processor;
  }
  OOPAccessTag (const OOPObjectId & taskid,
                 const OOPAccessTag & tag, int processor)
  {
    fTaskId = taskid;
    fAccessMode = tag.AccessMode();
    fVersion = tag.Version ();
    fObjectId = tag.fObjectId;
    fObjectAutoPtr = tag.fObjectAutoPtr;
    fProcessor = processor;
  }
  /**
   * Operator equal overloaded
   */
  OOPAccessTag & operator = (const OOPAccessTag & aci)
  {
    fTaskId = aci.fTaskId;
    fAccessMode = aci.fAccessMode;
    fVersion = aci.fVersion;
    fProcessor = aci.fProcessor;
    fObjectId = aci.fObjectId;
    fObjectAutoPtr = aci.fObjectAutoPtr;
    return *this;
  }
  /**
   * Copy constructor
   * @param aci AccessInfo object to be copied
   */
  OOPAccessTag (const OOPAccessTag & aci)
  {
    fTaskId = aci.fTaskId;
    fAccessMode = aci.fAccessMode;
    fVersion = aci.fVersion;
    fProcessor = aci.fProcessor;
    fObjectId = aci.fObjectId;
    fObjectAutoPtr = aci.fObjectAutoPtr;
  }
  OOPAccessTag (OOPObjectId & id, TPZAutoPointer<TPZSaveable> obj)
  {
    //fTaskId = aci.fTaskId;
    //fAccessMode = aci.fAccessMode;
    //fVersion = aci.fVersion;
    //fProcessor = DM->GetProcId();
    fObjectId = id;
    fObjectAutoPtr = obj;
  }

  bool operator == (const OOPAccessTag & other)
  {
    return (fTaskId == other.fTaskId && fAccessMode == other.fAccessMode
                        && fVersion == other.fVersion
                        && fProcessor == other.fProcessor);
  }
  void Print (std::ostream & out = std::cout) const
  {
    out << "Data State " << fAccessMode << endl;
    out << "Processor " << fProcessor << endl;
    out << "TaskId " << fTaskId << endl;
    out << "Version" << fVersion << endl;
  }
  void ShortPrint (std::ostream & out = std::cout) const
  {
    out << "S:" << fAccessMode << ";";
    out << "P:" << fProcessor << ";";
    out << "T:" << fTaskId << ";";
    out << "V:" << fVersion;
  }

  bool CanExecute (const OOPMetaData & object) const;
  OOPMDataState AccessMode() const
  {
    return fAccessMode;
  }
  OOPDataVersion Version() const
  {
    return fVersion;
  }
};

inline std::ostream &operator<<(std::ostream &out, const OOPAccessTag & info)
{
  info.ShortPrint(out);
  return out;
}


#endif
