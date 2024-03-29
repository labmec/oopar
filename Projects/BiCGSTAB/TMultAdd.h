/* Generated by Together */

#ifndef TMULTADD_H
#define TMULTADD_H
#include <ooptask.h>
class OOPMReturnType;
class TMultAdd : public OOPTask {
public:	

    /**
    	* Returns the estimated execution time.
    	* returns 0 if the task is instantaneous
    	* returns > 0 if estimate is known
    	* return < 0 if no estimate is known
    	*/
	virtual long ExecTime ();

	/**
	* Execute the task, verifying that all needed data acesses are satisfied.
	*/
	virtual OOPMReturnType Execute ();

	/**
	* Returns last created Id.
	*/
	virtual long GetClassID ()
	{
		return TDISTNORM_ID;
	}
};
#endif //TMULTADD_H