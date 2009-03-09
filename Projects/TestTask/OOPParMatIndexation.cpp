/*
 *  OOPParMatIndexation.cpp
 *  OOParApp
 *
 *  Created by Gustavo Longhin on 2/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OOPParMatIndexation.h"

template class TPZRestoreClass<OOPParMatIndexation, OOPMATINDEXATION_ID>;


#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPParMatIndexation"));
#endif

OOPParMatIndexation::OOPParMatIndexation() : TPZSaveable()
{
}

int OOPParMatIndexation::ClassId() const 
{
	return OOPMATINDEXATION_ID;
}
void OOPParMatIndexation::Read(TPZStream & buf, void * context)
{
  TPZSaveable::Read(buf, context);
  int clsid=0;
  buf.Read(&clsid);
  if(clsid!=ClassId())
	{
#ifdef LOGPZ
    std::stringstream sout;
    sout << "ClassId missmatch on OOPDouble::Read";
    LOGPZ_ERROR(logger,sout.str().c_str());
#endif
  }
	int size = 0;
	buf.Read(&size, 1);
	m_Indices.resize(size);
	int i = 0;
	for(i = 0; i < size; i++)
	{
		int j;
		int rowSize = 0;
		std::vector< int > rowsvec;
		buf.Read(&rowSize, 1);
		rowsvec.resize(rowSize);
		for(j = 0; j < rowSize; j ++)
		{
			buf.Read(&rowsvec[i], 1);
		}
		int colSize = 0;
		buf.Read(&colSize, 1);
		std::vector < int > colsvec;
		colsvec.resize(colSize);
		for(j = 0; j < colSize; j ++)
		{
			buf.Read(&colsvec[i], 1);
		}
		m_Indices[i] = std::pair< std::vector<int>, std::vector<int> > (rowsvec, colsvec);
	}
}
void OOPParMatIndexation::Write(TPZStream & buf, int withclassid)
{
	std::cout << "Write do MatIndexation" << std::endl;
	std::cout.flush();
  TPZSaveable::Write(buf, withclassid);
  int clsid = ClassId();
  buf.Write(&clsid);
	
	int size = m_Indices.size();
	buf.Write(&size, 1);
	int i = 0;
	for(i = 0; i < size; i++)
	{
		int j;
		int rowSize = m_Indices[i].first.size();
		buf.Write(&rowSize, 1);
		for(j = 0; j < m_Indices[i].first.size(); j ++)
		{
			buf.Write(&m_Indices[i].first[i], 1);
		}
		int colSize = m_Indices[i].second.size();
		buf.Write(&colSize, 1);
		for(j = 0; j < m_Indices[i].second.size(); j ++)
		{
			buf.Write(&m_Indices[i].second[i], 1);
		}
	}
	std::cout << "Saiu do Write do MatIndexation" << std::endl;
	std::cout.flush();
}

std::vector < int > OOPParMatIndexation::GetColVector(int i)
{
	return m_Indices[i].second;
}
std::vector < int > OOPParMatIndexation::GetRowVector(int i)
{
	return m_Indices[i].first;
}

void OOPParMatIndexation::SetIndexationVector(std::vector < std::pair < std::vector <int > , std::vector < int > > > & theVector)
{
	m_Indices = theVector;
}


