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
//extern OOPDataManager * DM;

/**
 * Implements a information tag concerning access requirements from Tasks to Data.
 * Assumes responsibility from three discontinued classe OOPAccessInfo,
 * OOPDataDepend and OOPTaskDepend.
 * OOPAccessTag is the new OOPar access currency, OOPAccessTags are axchanged among processors, tasks, etc.
 */
class OOPAccessTag{
public:
  OOPAccessTag();
  int Count() const
  {
    return fObjectAutoPtr.Count();
  }

/*
ENoAccess,
  EReadAccess,
  EWriteAccess,    */
  std::string AccessModeString();

  ~OOPAccessTag();
  operator bool ();
  bool operator < (const OOPAccessTag & compare) const
  {
    if(this->fVersion < compare.fVersion) return true;
    if(!(this->fVersion == compare.fVersion)) return false;
    if(this->fAccessMode == EReadAccess && compare.fAccessMode != this->fAccessMode) return true;
    if(this->fAccessMode != compare.fAccessMode) return false;
    if(this->fProcessor < compare.fProcessor) return true;
    if(this->fProcessor != compare.fProcessor) return false;
    if(this->fObjectAutoPtr.operator->() < compare.fObjectAutoPtr.operator->()) return true;
    if(this->fObjectAutoPtr.operator->() != compare.fObjectAutoPtr.operator->()) return false;
    if(this->fObjectId < compare.fObjectId) return true;
    if(this->fObjectId != compare.fObjectId) return false;
    return false;
  }
  bool CanExecute();
  int Proc() const
  {
   return fProcessor;
  }
  void SetTaskId(const OOPObjectId & Id)
  {
    fTaskId = Id;
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
//  TPZSaveable * GetPointer();

  TPZAutoPointer<TPZSaveable> AutoPointer() const
  {
    return this->fObjectAutoPtr;
  }

  void SetAutoPointer(TPZAutoPointer<TPZSaveable> pointer)
  {
    fObjectAutoPtr = pointer;
  }
  bool IsMyAccessTag(const OOPAccessTag & granted);
  void ClearPointer()
  {
    fObjectAutoPtr = TPZAutoPointer<TPZSaveable>(0);
  }

  void IncrementVersion()
  {
    fVersion++;
  }

  void SetVersion(const OOPDataVersion &version)
  {
    fVersion = version;
  }

  OOPObjectId TaskId() const
  {
    return fTaskId;
  }
  OOPObjectId Id() const
  {
    return fObjectId;
  }
  OOPAccessTag (const OOPObjectId & Id,
                 const OOPMDataState & st,
                 const OOPDataVersion & version, int processor)
  {
    fObjectId = Id;
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
    fProcessor = 0;
    fAccessMode = EReadAccess;
    fObjectId = id;
    fObjectAutoPtr = obj;
  }

  bool operator == (const OOPAccessTag & other)
  {
    return (fTaskId == other.fTaskId && fAccessMode == other.fAccessMode
                        && fVersion == other.fVersion
                        && fProcessor == other.fProcessor);
  }
  void Print (std::ostream & out) const
  {
    out << "Object Id " << fObjectId;
    out << "  Version " << fVersion << endl;
    switch(fAccessMode){
      case EReadAccess:
      {
        out << "AccessMode ReadAccess" << endl;
      }
      break;
      case EWriteAccess:
      {
        out << "AccessMode WriteAccess" << endl;
      }
      break;
      default:
      {
        out << "AccessMode NoAccess defined" << endl;
      }
    }
    out << " Processor " << fProcessor << endl;
    out << " TaskId " << fTaskId << endl;
    out << " Has Pointer " << fObjectAutoPtr << endl;
  }
  void ShortPrint (std::ostream & out) const
  {
    out << "Id:" << fObjectId << "|";
    //string access = (fAccessMode == 2 ? string("W") : string("R"));
    out << "S:" << fAccessMode << "|";
    out << "P:" << fProcessor << "|";
    out << "T:" << fTaskId << "|";
    out << "V:" << fVersion << "|";
    out << "D:" << fObjectAutoPtr << "|";
    out << "Cnt:" << Count() << "\n";
  }

  OOPMDataState AccessMode() const
  {
    return fAccessMode;
  }
  void SetAccessMode(OOPMDataState state)
  {
    fAccessMode = state;
  }
  OOPDataVersion Version() const
  {
    return fVersion;
  }

  void SetProcessor(int proc)
  {
    fProcessor = proc;
  }
};

inline std::ostream &operator<<(std::ostream &out, const OOPAccessTag & info)
{
  info.ShortPrint(out);
  return out;
}


#endif
