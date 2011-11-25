/**
 * @file
 * @brief Contains the declaration of the TTaskComm class.
 */
#ifndef TTASKCOMM_H
#define TTASKCOMM_H

#include "ooptask.h"
#include "bicgdefs.h"

class   OOPReceiveStorage;
class   OOPSendStorage;

/**
 * @brief Implements the communication itself among partitions.
 * @author Gustavo Camargo Longhin
 * @version 0.8
 * @since 12/06/2003 
 */
/**
 * The class attribution is to perform the contribution on the destinatio partition.
 * TLocalCompute objects which contributes on another partitions perform such contribution with objects of this class.
 */
class TTaskComm:public OOPTask
{
public:
	
    /**
     * @note Virtual function must be redefined.
     * @brief Used by the TM to access the task´s functinality. 
     */
	OOPMReturnType Execute ();
	
	/**
	 * @brief Defines the necessary interface for task communication along the network.
	 * @param buf Buffer for data manipulation.
	 */
	void Read(TPZStream &buf, void *context);
	void Write(TPZStream &buf, int classid);
	
	/**
	 * @brief Returns a Class ID identifier
	 */
	virtual long GetClassID () {
		return TTASKCOMM_ID;
	}
	static TPZSaveable *Restore (OOPStorageBuffer * buf);
	TTaskComm (int ProcId);
	/** @brief Sets the value the communication task will transport */
	void SetValue(double val);
	void SetValue(long val);
	
private:
	double fDoubleValue;
	long fLongValue;
	
};

extern ofstream TaskLog;

#endif // TTASKCOMM_H
