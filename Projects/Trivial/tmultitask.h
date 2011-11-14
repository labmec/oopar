/**
 * @file
 */

#ifndef TMULTITASK_H
#define TMULTITASK_H
#include "ooptask.h"
//#include "gnudefs.h"
#include "tmultidata.h"
#include "tresultdata.h"
#include "ooptask.h"
//class OOPMReturnType;
class TPZSaveable;

class OOPStorageBuffer;


class TMultiTask : public OOPTask {
public:
	
static	TPZSaveable * Restore(OOPStorageBuffer *buf);
	
public:
	
	OOPObjectId RhsId;
	
	TMultiTask(int i);
	void SetData(TMultiData & data, TResultData & resdata);
	
    /**
    * @brief Returns the estimated execution time. \n
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
		return 3000;
	}

    /**
    * @brief Virtual Unpack function.
    * Defines the necessary interface for task communication along the network.
    * @param * buf Buffer for data manipulation.
    */
	virtual int Read(OOPStorageBuffer * buf,void *context);

	virtual int Write(OOPStorageBuffer * buf, int classid);

    /**
    * @brief Returns class name
    */
	virtual char *ClassName ()
	{
		return ("TTask");
	}

    /**
    * @brief Returns true if the object belongs to a class derived from the specified classid
    * @param Classid Id of inquired super class
    */
	int DerivedFrom (long Classid);

    /**
    * @brief Returns true if the object belongs to a class derived from the specified class name
    * @param classname Name of inquired super class
    */
	int DerivedFrom (char *classname);
	
};
#endif //TMULTITASK_H
