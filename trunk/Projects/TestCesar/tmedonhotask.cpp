//
// C++ Implementation: tmedonhotask
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <cesar@labmec.fec.unicamp.br>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tmedonhotask.h"


//oopar includes
#include "OOPDouble.h"
#include "OOPVector.h"

// include log4cxx header files.
#include <log4cxx/logger.h>

using namespace log4cxx;
static LoggerPtr logger(Logger::getLogger("tmedonhotask"));

TMedonhoTask::TMedonhoTask(int Procid, int start, int end)
 : OOPTask(Procid)
{
  LOG4CXX_INFO(logger,"Entering Constructor");
  fStart = start;
  fEnd = end;
//  fDotResult = 0.;
  LOG4CXX_INFO(logger,"Exiting Constructor");
}

TMedonhoTask::TMedonhoTask()
 : OOPTask(-1)
{
  LOG4CXX_INFO(logger,"Entering Empty Constructor");
  fStart = -1;
  fEnd = -1;
//  fDotResult = 0.;
  LOG4CXX_INFO(logger,"Exiting Empty Constructor");
}

TMedonhoTask::~TMedonhoTask()
{
  LOG4CXX_INFO(logger,"Throw Destructor");
}

void TMedonhoTask::Write(TPZStream & buf, int withclassid)
{
  LOG4CXX_INFO(logger,"Entering pack");
  OOPTask::Write(buf,withclassid);
  
  //pack start
  buf.Write (&fStart,1);
  //pack end
  buf.Write (&fEnd,1);
/*  //pack fX  
  TPZSaveable::WriteObjects(buf,fX);
  //pack fY
  TPZSaveable::WriteObjects(buf,fY);
  //pack dot
  buf.Write(&fDotResult,1);
  //pack cross
  TPZSaveable::WriteObjects(buf,fCrossResult);*/
  LOG4CXX_INFO(logger,"Exiting pack");
}

void TMedonhoTask::Read(TPZStream & buf, void * context)
{
  LOG4CXX_INFO(logger,"Entering unpack");
  OOPTask::Read(buf,context);
  
  //unpack start
  buf.Read (&fStart,1);
  //pack end
  buf.Read (&fEnd,1);
//   //unpack fX  
//   TPZSaveable::ReadObjects(buf,fX);
//   //unpack fY
//   TPZSaveable::ReadObjects(buf,fY);
//   //unpack dot
//   buf.Read(&fDotResult,1);
//   //unpack cross
//   TPZSaveable::ReadObjects(buf,fCrossResult);

  LOG4CXX_INFO(logger,"Exiting unpack");
}

template class TPZRestoreClass<TMedonhoTask,37882396>;

OOPMReturnType TMedonhoTask::Execute()
{
  LOG4CXX_INFO(logger,"Entering Execute");
  
  OOPVector<double> *x,*y,*cross;
  OOPDouble *dot;
  
  LOG4CXX_DEBUG(logger,"acquiring data dep obj ptrs");
  x = dynamic_cast<OOPVector<double > *>(GetDepObjPtr(0));
  y = dynamic_cast<OOPVector<double > *>(GetDepObjPtr(1));
  cross = dynamic_cast<OOPVector<double > *>(GetDepObjPtr(2));
  dot = dynamic_cast<OOPDouble *>(GetDepObjPtr(3));
  
  if (!x || !y || !cross || !dot){
    LOG4CXX_ERROR(logger,"Exiting Execute: fail to acquire data de obj ptrs..");
    return EFail;
  }

  
  if (x->fVecValue.NElements() != y->fVecValue.NElements()){
    LOG4CXX_ERROR(logger,"Exiting Execute: incompatible dimensions");
    return EFail;
  }
  
  LOG4CXX_DEBUG(logger,"Resize cross vector");
  cross->fVecValue.Resize(x->fVecValue.NElements());
  
  int i;
  REAL sum = 0.;
  REAL aux;
  LOG4CXX_DEBUG(logger,"Evaluating de products");
  for (i=fStart;i<fEnd;i++){
    aux = x->fVecValue[i] * y->fVecValue[i];
    sum += aux;
    cross->fVecValue[i] = aux;
  }
  dot->fValue += sum;
  
  IncrementWriteDependentData();
  
  LOG4CXX_INFO(logger,"Exiting Execute");
  return ESuccess;
}
