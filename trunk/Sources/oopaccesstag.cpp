//
// C++ Implementation: oopaccesstag
//
// Description:
//
//
// Author: Gustavo C Longhin <longhin@labmec.fec.unicamp.br>, (C) 2007
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
  if(!(fVersion.CanExecute(granted.fVersion))) return false;
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
    fObjectAutoPtr->Write(buf, 1);
  }else
  {
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
#ifdef LOGPZ
  {
    stringstream sout;
    sout << "Before reading an object";
    this->ShortPrint( sout);
    LOGPZ_DEBUG(logger, sout.str().c_str());
  }
#endif

  TPZSaveable * r = TPZSaveable::Restore(buf, context);
  if(r)
  {
    this->fObjectAutoPtr = TPZAutoPointer<TPZSaveable>(r);
#ifdef LOGPZ
    {
      stringstream sout;
      sout << "Read object of class id C:" << this->fObjectAutoPtr->ClassId();
      LOGPZ_DEBUG(logger, sout.str().c_str());
    }
#endif
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
