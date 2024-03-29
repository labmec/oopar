/* Generated by Together */
#include "OOPDouble.h"

#include <sstream>
#include <pzlog.h>
#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPAR.OOPDouble"));
#endif

OOPDouble::OOPDouble() : TPZSaveable(), fValue(0.)
{
}

int OOPDouble::ClassId() const
{
	return OOPDOUBLE_ID;
}
void OOPDouble::Read(TPZStream & buf, void * context){
	TPZSaveable::Read(buf, context);
	buf.Read(&fValue);
	int clsid=0;
	buf.Read(&clsid);
	if(clsid!=ClassId()){
#ifdef LOG4CXX
		std::stringstream sout;
		sout << "ClassId missmatch on OOPDouble::Read";
		LOGPZ_ERROR(logger,sout.str().c_str());
#endif
	}
}
void OOPDouble::Write(TPZStream & buf, int withclassid){
	TPZSaveable::Write(buf, withclassid);
	buf.Write(&fValue);
	int clsid = ClassId();
	buf.Write(&clsid);
}
template class TPZRestoreClass<OOPDouble, OOPDOUBLE_ID>;
