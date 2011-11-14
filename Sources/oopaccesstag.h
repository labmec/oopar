/**
 * @file
 * @brief Defines the OOPAccessTag class.
 */
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

using namespace std;

/**
 * @brief Implements a information tag concerning access requirements from Tasks to Data.
 * @author Edimar Cesar Rylo
 * @since 2007
 * @ingroup managerdata
 */
/**
 * Assumes responsibility from three discontinued classe OOPAccessInfo,
 * OOPDataDepend and OOPTaskDepend.
 * OOPAccessTag is the new OOPar access currency, OOPAccessTags are axchanged among processors, tasks, etc.
 */
class OOPAccessTag {
public:
	OOPAccessTag();
	int Count() const
	{
		return fObjectAutoPtr.Count();
	}
	
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
	OOPDataVersion GetVersion()
	{
		return fVersion;
	}
private:
	/**
	 * @brief Describes the type of access state
	 */
	OOPMDataState fAccessMode;
	/**
	 * @brief Identifies the Data to which the current accesstag exists
	 */
	OOPObjectId fObjectId;
	/**
	 * @brief Identifies the TaskId willing to access the data object
	 */
	OOPObjectId fTaskId;
	/**
	 * @brief Identifies in which version the task needs the data to be
	 */
	OOPDataVersion fVersion;
	/**
	 * @brief Processor which filed the request
	 */
	int fProcessor;
	/**
	 * @brief AutoPointer to the object referred to by OOPObjectId
	 */
	TPZAutoPointer<TPZSaveable> fObjectAutoPtr;
public:
	void Write (TPZStream  & buf, int withclassid);
	void Read (TPZStream & buf, void * context);
	
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
	
	/**
	 * @brief Constructor with initial parameters
	 * @param Id Id of task requiring access on the data.
	 * @param st Type of access on the data
	 * @param version Version required on the data
	 * @param processor Processor where the access should occur
	 */	
	OOPAccessTag (const OOPObjectId & Id,
				  const OOPMDataState & st,
				  const OOPDataVersion & version, int processor)
	{
		fObjectId = Id;
		fAccessMode = st;
		fVersion = version;
		fProcessor = processor;
	}
	/**
	 * @brief Constructor with initial parameters
	 * @param taskid Id of task requiring access on the data.
	 * @param tag Type of access
	 * @param processor Processor where the access should occur
	 */
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
	 * @brief Operator equal overloaded
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
	 * @brief Copy constructor
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
