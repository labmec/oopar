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

#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPTaskManager"));
static LoggerPtr AccessLogger(Logger::getLogger("OOPar.OOPDataManager.OOPAccessTag"));
static LoggerPtr MetaLogger(Logger::getLogger("OOPar.OOPDataManager.MetaData"));
static LoggerPtr tasklogger (Logger::getLogger ("OOPar.OOPTaskManager.OOPTask"));
#endif


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


void OOPAccessTagList::Clear(TPZAutoPointer<OOPDataManager> DM)
{
	std::vector<OOPAccessTag>::iterator it = fTagList.begin();
	std::set<OOPObjectId> lset;
	for(;it!=fTagList.end();it++)
	{
		lset.insert(it->Id());
		if(it->AccessMode() == EWriteAccess)
		{
#ifdef LOG4CXX
            {
                std::stringstream sout;
                sout << __PRETTY_FUNCTION__ << " Releasing write access request for tag ";
                it->ShortPrint(sout);
                sout << " ClassId " << it->AutoPointer()->ClassId();
                sout << " On processor " << DM->GetProcID();
                LOGPZ_DEBUG(logger, sout.str())
            }
#endif
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
		if(!it->CanExecute())
		{
#ifdef LOG4CXX
			{
				stringstream sout;
				sout << "CanExecute returned FALSE on Id " << it->Id();
				LOGPZ_DEBUG(logger,sout.str().c_str());
			}
#endif
			return false;
		}
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

/**
 * @brief Set an explicit version for a write dependent object
 */
void OOPAccessTagList::SetVersion(int tagindex, OOPDataVersion newversion)
{
    if(tagindex <0 || tagindex >= fTagList.size())
    {
        DebugStop();
    }
    if (fTagList[tagindex].AccessMode() == EWriteAccess) 
    {
        fTagList[tagindex].SetVersion(newversion);
    }
    else {
        LOGPZ_ERROR(logger, "SetVersion called on a ReadAccess object")
#ifdef LOG4CXX
        {
            std::stringstream sout;
            sout << "Trying to modify the version of object index " << tagindex << " Taglist : ";
            Print(sout);
            sout << "SetVersion WRONG CALL";
            LOGPZ_ERROR(logger, sout.str())
            
        }
#endif
    }
}


void OOPAccessTagList::SubmitIncrementedVersions(TPZAutoPointer<OOPDataManager> DM)
{
	std::vector<OOPAccessTag>::iterator it = fTagList.begin();
	for(;it!=fTagList.end();it++)
	{
		if(it->AccessMode() == EWriteAccess)
		{
			DM->PostData(*it);
		}
#ifdef LOG4CXX
		{
			stringstream sout;
			sout << "After Posting Data with incremented version, Releasing Access according to Tag:";
			it->ShortPrint(sout);
			LOGPZ_DEBUG(AccessLogger,sout.str().c_str());
		}
		{
			stringstream sout;
			sout << "Object Id:" << it->Id() << " Released from Task T:" << it->TaskId();
			LOGPZ_INFO(MetaLogger, sout.str().c_str());
		}
#endif
		it->ClearPointer();
	}
	for(it=fTagList.begin();it!=fTagList.end();it++)
	{
		DM->ObjectChanged(it->Id());
	}
}
void OOPAccessTagList::Print(std::ostream & out)
{
	std::vector<OOPAccessTag>::iterator it;
	for(it = fTagList.begin(); it != fTagList.end(); it++)
	{
		it->Print(out);
	}
}

void OOPAccessTagList::ShortPrint(std::ostream & out)
{
	std::vector<OOPAccessTag>::iterator it;
	for(it = fTagList.begin(); it != fTagList.end(); it++)
	{
		it->ShortPrint(out);
		out << "||";
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

void OOPAccessTagList::PostRequests(OOPObjectId & Id, TPZAutoPointer<OOPDataManager> DM)
{
#ifdef LOG4CXX
	stringstream sout;
	sout << "Posting Access Requests for Task T:" << Id;
#endif
	int processor = DM->GetProcID();
	int i = 0;
#ifdef LOG4CXX
	sout << " with " << fTagList.size() << " Tag(s):";
#endif
	for(i=0;i<(signed int)fTagList.size();i++)
	{
		fTagList[i].SetProcessor(processor);
		fTagList[i].SetTaskId(Id);
#ifdef LOG4CXX
		fTagList[i].ShortPrint(sout);
#endif
		DM->PostAccessRequest( fTagList[i]);
	}
#ifdef LOG4CXX
	LOGPZ_DEBUG(logger,sout.str().c_str());
	LOGPZ_DEBUG(tasklogger,sout.str().c_str());
#endif
	
}

OOPAccessTag OOPAccessTagList::GetTag(int i)
{
	return fTagList[i];
}


