//
// Autor:   Fabio Amaral Castro,  RA: 991722
//
// Arquivo: str_mpi.h
//
// Classe:  TSendStorageMpi,
//          TReceiveStorageMpi.
//
// Descr.: 
//          TSendStorageMpi:
//            Classe derivada de TSendStorage que implementa o buffer
//            usando MPI para a comunicacao entre processos.
//
//          TReceiveStorageMpi:
//            Classe derivada de TReceiveStorage que implementa o buffer
//            usando MPI para a comunicacao entre processos.
//
// Versao:  01 / 03
//
#ifndef _STR_MPIHH_
#define _STR_MPIHH_

#include "mpi.h"
#include "pzmanvector.h"
#include "pzfilebuffer.h"
#include "pzsave.h"
#include "ooperror.h"
/** 
 * Non abstract class, which implements the receive
 * buffer using MPI (Message Passing Interface) library 
 * for communication.
*/

const int MAXSIZE = 50000000;

class   OOPMPIStorageBuffer:public OOPError
{
	

private:
  /** Buffer which stores received messages */
	TPZManVector<char,MAXSIZE> f_recv_buffr;
	TPZManVector<char,MAXSIZE> f_send_buffr;
  /** Dimension of received message */
	int     f_recv_size;
	int     f_send_size;
  /** Receive buffer position to be unpack */
	int     f_recv_position;
	int     f_send_position;
  /** Id of process that sent received message */
	int     f_sender_tid;
  /** Tag of received message */
	int     f_msg_tag;
  /** request object for non-blocking receive operation */
	MPI_Request  f_request;

    /** Id of process for which message shall be sent */
	int     f_target_tid;

  /** flag indicating whether nonblocking reception is initiated */
    int     f_isreceiving;
    /**
     * Indicates the amount of bytes transmitted through the MPI Interface
     * Requires resetting for each Message being sent
     */
    long f_BytesTransmitted;
    
public:
	~OOPMPIStorageBuffer();
	void FreeRequest();
	/**
     * Contructor which initializes the buffer
	 */
   OOPMPIStorageBuffer() : f_recv_buffr(500000000) {
       //Receiveing related initialization
	   f_recv_size = 0; f_recv_position = 0; f_sender_tid = -1; f_msg_tag = 0; f_isreceiving = 0;
	   f_send_size = 0;
	   f_send_position = 0;
	   ResetBuffer();
       //Sending related initialization
       //Should this be called ? I dont think so
       //ResetBuffer ();
   }
  /**
   * Restores next object in the buffer
   */
	virtual TPZSaveable *Restore ();
   /**
   * Unpacks array of characteres from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkByte (char *p, int n = 1);
  /**
   * Unpacks array of integers from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkInt (int *p, int n = 1);
  /**
   * Unpacks array of short integers from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkShort (short *p, int n = 1);
  /**
   * Unpacks array of long integers from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkLong (long *p, int n = 1);
  /**
   * Unpacks array of unsigned integers from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkUint (u_int * p, int n = 1);
  /**
   * Unpacks array of unsigned short integers from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkUshort (u_short * p, int n = 1);
  /**
   * Unpacks array of unsigned long integers from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkUlong (u_long * p, int n = 1);
  /**
   * Unpacks array of float reals from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkFloat (float *p, int n = 1);
  /**
   * Unpacks array of double reals from received buffer
   * @param p Pointer to array for which elements must be unpacked
   * @param n Number of elements to be unpacked
   */
	int     UpkDouble (double *p, int n = 1);
  /**
   * Unpacks a string from received buffer
   * @param p Pointer to string to  be unpacked
   */
	int     UpkStr (char *str);
  /**
   * Nonblocking receive. If there is a posted message to
   * be received, receives it and returns 1. Else, retuns 0
   */
	int     Receive ();
  /**
   * Checks on reception status
   */
    bool TestReceive();
  /**
   * Blocking receive. Execution stops and awaits until a 
   * posted message is received
   */
	int     ReceiveBlocking ();

    /**
     * Packs array of characteres into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkByte (const char *p, int n = 1);

    /**
     * Packs array of integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkInt (int *p, int n = 1);

    /**
     * Packs array of short integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkShort (short *p, int n = 1);

    /**
     * Packs array of long integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkLong (long *p, int n = 1);

    /**
     * Packs array of unsigned integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkUint (u_int * p, int n = 1);

    /**
     * Packs array of unsigned short integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkUshort (u_short * p, int n = 1);

    /**
     * Packs array of unsigned long integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkUlong (u_long * p, int n = 1);

    /**
     * Packs array of float reals into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkFloat (float *p, int n = 1);

    /**
     * Packs array of doubles into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
	int     PkDouble (double *p, int n = 1);

    /**
     * Packs a string into send buffer
     * @param p Pointer to string to be packed
     */
	int     PkStr (char *str);

    /**
     * Sends data stored in send buffer and clears it
     * @param msg_id Id of the message to be sent
     */
	int     Send (int msg_id);

    /**
     * Returns the dimension of the stored message
     */
	int     Length ()
	{
		return (f_send_position - sizeof (int));
	}

private:

    /**
     * Generic method to pach data into an MPI buffer
     */
    int PackGeneric(void *ptr, int n, MPI_Datatype mpitype);  

    /**
       * Clears send buffer 
       */

public:
	int     ResetBuffer (int size = 0);  
private:
    /**
       * Expands send buffer dimension
       */
	void    ExpandBuffer (int more_dimension);

public:
  /**
   * Used for error management
   */
	char   *ClassName ()
	{
		return ("OOPMPIStorageBuffer::");
	}

    virtual void Write(int *p, int size = 1);

    virtual void Write(double *p, int size = 1);

    virtual void Write(const char *p, int size = 1);

    virtual void Write(std::string *p, int size = 1);

    virtual void Read(int *p, int size = 1);

    virtual void Read(double *p, int size = 1);

    virtual void Read(char *p, int size = 1);

    virtual void Read(std::string *p, int size = 1);
    void ResetByteCounter();

	long GetBytesTransmitted() const;
	
};
typedef OOPMPIStorageBuffer *POOPMPIStorageBuffer;
#endif
