//
// C++ Implementation: oopaccesstag
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <ecrylo@uol.com.br>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "oopaccesstag.h"

#include <pzlog.h>

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPar.OOPAccessTag"));
#endif

OOPAccessTag::OOPAccessTag()
{
  fProcessor = -1;
}


OOPAccessTag::~OOPAccessTag()
{
  fProcessor = -1;
}
OOPAccessTag::operator bool ()
{
#warning "What validates the existence of my tag ? "
  return fObjectId.operator bool();
}

bool OOPAccessTag::CanExecute()
{
  return fObjectAutoPtr;
}
/*TPZSaveable * OOPAccessTag::GetPointer()
{
  return fObjectAutoPtr.operator->();
}*/
bool OOPAccessTag::IsMyAccessTag(const OOPAccessTag & granted)
{
  if(fObjectAutoPtr) return false;
  if(!(fObjectId == granted.fObjectId)) return false; 
  if(!(fAccessMode == granted.fAccessMode)) return false; 
  if(!(fVersion== granted.fVersion)) return false; 
  if(!(fProcessor == granted.fProcessor)) return false; 
  if(!(fTaskId == granted.fTaskId)) return false;
  return true; 
}
void OOPAccessTag::Write (TPZStream  & buf, int withclassid)
{
  fObjectId.Write (buf, 0);
  fTaskId.Write( buf, 0);
  int need = fAccessMode; 
  buf.Write (&need);
  int proc = fProcessor;
  buf.Write(&proc);
  fVersion.Write(buf, 0);
  if(fObjectAutoPtr)
  {
    LOG4CXX_DEBUG(logger,"writing with pointer");
    fObjectAutoPtr->Write(buf, 1);
  }else
  {
    LOG4CXX_DEBUG(logger,"writing -1");
    int aux = -1;
    buf.Write(&aux, 1);
  }
}
  /**
   * method to reconstruct the object
   */
void OOPAccessTag::Read (TPZStream & buf, void * context)
{
  fObjectId.Read (buf,context);
  fTaskId.Read(buf,context);
  int need = 0;
  buf.Read (&need);
  fAccessMode = (OOPMDataState) need; 
  int proc = 0;
  buf.Read(&proc);
  fProcessor = proc;
  fVersion.Read(buf,context);
  TPZSaveable * r = TPZSaveable::Restore(buf, context);
  {
    std::stringstream sout;
    sout << __PRETTY_FUNCTION__ << " pointer read " << r;
    if(r) sout << r->ClassId();
    LOGPZ_DEBUG(logger,sout.str());
  }
  if(r)
  {
    this->fObjectAutoPtr = TPZAutoPointer<TPZSaveable>(r);
  }
  
}
std::string OOPAccessTag::AccessModeString()
{
  switch(fAccessMode ) {
    case EReadAccess:
    {
      return std::string("ReadAccess");
      break;
    }
    case EWriteAccess:
    {
      return std::string("WriteAccess");
      break;
    }
    default:
    {
      return std::string("NoAccess");
      break;
    }
  }
}
