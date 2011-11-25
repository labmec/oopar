/**
 * @file
 * @brief Contains the definition of the TTaskTest class.
 */
#ifndef TTASKTEST_H
#define TTASKTEST_H

#include "ooptask.h"
#include "tmultidata.h"
#include "tresultdata.h"
#include "ooptask.h"

class OOPSaveable;

class OOPSendStorage;
class OOPReceiveStorage;

class TTaskTest : public OOPTask {
public:
	
	static	OOPSaveable * Restore(OOPReceiveStorage *buf);
	
public:
	/** @brief Simple constructor */
	TTaskTest(int i);
	
	void SetData(TMultiData & data, TResultData & resdata);
	
    /** @brief Returns the estimated execution time. */
	/**
	 * returns 0 if the task is instantaneous
	 * returns > 0 if estimate is known
	 * return < 0 if no estimate is known
	 */
	virtual long ExecTime ();
	
    /**
	 * @brief Executes the task, verifying that all needed data acesses are satisfied.
	 */
	virtual OOPMReturnType Execute ();
	
    /**
	 * @brief Returns last created Id.
	 */
	virtual long GetClassID ()
	{
		return 3001;
	}

    /**
	 * @brief Defines the necessary interface for task communication along the network.
	 * @param buf Buffer for data manipulation.
	 */
	virtual void Read(TPZStream &buf,void *context);
	virtual void Write(TPZStream &buf,int withclassid);
	
    /**
	 * @brief Returns class name
	 */
	virtual char *ClassName ()
	{
		return ("TTaskTest");
	}
	
    /**
	 * @brief Returns true if the object belongs to a class derived from the specified classid
	 * @param Classid Id of inquired super class
	 */
	virtual int DerivedFrom (long Classid);
	
    /**
	 * @brief Returns true if the object belongs to a class derived from the specified class name
	 * @param classname Name of inquired super class
	 */
	virtual int DerivedFrom (char *classname);
};

#endif //TTASKTEST_H
