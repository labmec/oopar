/* Generated by Together */
#ifndef TTASKCOMM_H
#define TTASKCOMM_H
#include "ooptask.h"
#include "fluxdefs.h"
class   OOPStorageBuffer;
class   OOPStorageBuffer;
/**
 * Implements the communication itself among partitions.
 * The class attribution is to perform the contribution on the destinatio partition.
 * TLocalCompute objects which contributes on another partitions perform such contribution with objects of this class.
 * @author Gustavo Camargo Longhin
 * @version 0.8
 * @since 12/06/2003 
 */
class   TTaskComm:public OOPTask
{
public:
    /**
     * Virtual function must be redefined.
     * Used by the TM to access the task�s functinality. 
     */
	OOPMReturnType Execute ();
	void     Write (TPZStream & buf, int classid);
	/**
	* Virtual Unpack function.
	* Defines the necessary interface for task communication along the network.
	* @param * buf Buffer for data manipulation.
	*/
	void     Read (TPZStream & buf, void * context = 0);
  /**
   * Returns a Class ID identifier
   */
	virtual int ClassId () const {
		return TTASKCOMM_ID;
	}
	static TPZSaveable *Restore (TPZStream & buf, void * context = 0);
	TTaskComm (int ProcId);
	TTaskComm ();
};
template class TPZRestoreClass<TTaskComm, TTASKCOMM_ID>;
#endif // TTASKCOMM_H
