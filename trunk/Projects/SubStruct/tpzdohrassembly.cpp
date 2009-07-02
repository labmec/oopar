/*
 *  tpzdohrassembly.cpp
 *  SubStruct
 *
 *  Created by Philippe Devloo on 04/03/09.
 *  Copyright 2009 UNICAMP. All rights reserved.
 *
 */

#include "tpzdohrassembly.h"
#include "pzfmatrix.h"
#include "pzvec.h"
#include "pzlog.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("substruct.dohrassembly"));
#endif

/// sum the values in the local matrix into the global matrix
void TPZDohrAssembly::Assemble(int isub, const TPZFMatrix &local, TPZFMatrix &global)
{
	TPZVec<int> &avec = fFineEqs[isub];
	int neq = avec.NElements();
	int ieq;
	for(ieq=0; ieq<neq; ieq++)
	{
		global(avec[ieq],0) += local.GetVal(ieq,0);
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Assembling destination indices " << avec << std::endl;
		local.Print("Input vector",sout);
		global.Print("Resulting vector",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
}

/// extract the values from the global matrix into the local matrix
void TPZDohrAssembly::Extract(int isub, const TPZFMatrix &global, TPZFMatrix &local)
{
	TPZVec<int> &avec = fFineEqs[isub];
	int neq = avec.NElements();
	int ieq;
	local.Resize(neq,1);
	for(ieq=0; ieq<neq; ieq++)
	{
		local(ieq,0) = global.GetVal(avec[ieq],0);
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "sub structure " << isub << " Extracting destination indices " << avec << std::endl;
		local.Print("extracted vector",sout);
		global.Print("Global vector",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
}

/// sum the values in the local matrix into the global matrix
void TPZDohrAssembly::AssembleCoarse(int isub, const TPZFMatrix &local, TPZFMatrix &global)
{
	TPZVec<int> &avec = fCoarseEqs[isub];
	int neq = avec.NElements();
	int ieq;
	for(ieq=0; ieq<neq; ieq++)
	{
		global(avec[ieq],0) += local.GetVal(ieq,0);
	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Assembling destination indices " << avec << std::endl;
		local.Print("Input vector",sout);
		global.Print("Resulting vector",sout);
		LOGPZ_DEBUG(logger,sout.str())
	}
#endif
}

/// extract the values from the global matrix into the local matrix
void TPZDohrAssembly::ExtractCoarse(int isub, const TPZFMatrix &global, TPZFMatrix &local)
{
	TPZVec<int> &avec = fCoarseEqs[isub];
	int neq = avec.NElements();
	int ieq;
	local.Resize(neq,1);
	for(ieq=0; ieq<neq; ieq++)
	{
		local(ieq,0) = global.GetVal(avec[ieq],0);
	}
}
void TPZDohrAssembly::Write(TPZStream & buf, int withclassid)
{
	std::cout << "Write do MatIndexation" << std::endl;
	std::cout.flush();
  TPZSaveable::Write(buf, withclassid);
  int clsid = ClassId();
  buf.Write(&clsid);
	
	WriteVector(buf, fFineEqs);
	WriteVector(buf, fCoarseEqs);
	
}
	
void TPZDohrAssembly::Read(TPZStream &buf, void *context)
{
	TPZSaveable::Read(buf, context);
	int clsid=0;
	buf.Read(&clsid);
	if(clsid!=ClassId())
	{
	#ifdef LOGPZ
		std::stringstream sout;
		sout << "ClassId missmatch on TPZDohrAssembly::Read";
		LOGPZ_ERROR(logger,sout.str().c_str());
	#endif
  }
	
	ReadVector(buf, fFineEqs);
	ReadVector(buf, fCoarseEqs);
	
}
	
void TPZDohrAssembly::WriteVector(TPZStream & buf, TPZVec< TPZVec < int > >  & vec)
{
	int size = vec.NElements();
	buf.Write(&size, 1);
	int i = 0;
	for(i = 0; i< size; i++)
	{
		TPZSaveable::WriteObjects(buf, vec[i]);
	}
}
void TPZDohrAssembly::ReadVector(TPZStream & buf, TPZVec< TPZVec < int > >  & vec)
{
	int size = 0;
	buf.Read(&size, 1);
	vec.Resize(size);
	int i = 0;
	for(i = 0; i< size; i++)
	{
		TPZSaveable::ReadObjects(buf, vec[i]);
	}
}
