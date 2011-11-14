/**
 * @file
 */
#ifndef _STR_MPIHH_
#define _STR_MPIHH_

#ifdef OOP_MPI
#include <mpi.h>
#endif
#ifdef OOP_MPE
#include <mpe.h>
#endif

#include "pzmanvector.h"
#include "pzfilebuffer.h"
#include "pzsave.h"
/**
 * @brief Non abstract class, which implements the receive
 * buffer using MPI (Message Passing Interface) library
 * for communication.
 * @ingroup managercomm
 */

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned long u_long;

const int MAXSIZE = 50000;

class   OOPMPIStorageBuffer: public TPZStream
{
private:
    /**
     * @brief Buffer which stores received messages
     */
    TPZManVector<char,MAXSIZE> m_Buffer;
    //TPZManVector<char,MAXSIZE> f_send_buffr;
    /**
     * @brief Dimension of received message
     */
    int m_Size;
	
    /**
     * @brief Receive buffer position to be unpack
     */
    int m_Length;
	
    /**
     * @brief Id of process that sent received message
     */
    int m_SenderTid;
    /**
     * @brief Tag of received message
     */
    int m_MsgTag;
    /**
     * @brief Request object for non-blocking receive operation
     */
    MPI_Request  m_Request;
    MPI_Status m_Status;
	
    /**
     * @brief Id of process for which message shall be sent
     */
    int m_TargetTid;
	
    /**
     * @brief Flag indicating whether nonblocking reception is initiated. \n
     * Certainly it will hardly be used since Blocking receiving is the new default
     */
    int m_IsReceiving;
    /**
     * @brief Generic method to pach data into an MPI buffer
     */
    int PackGeneric(void *ptr, int n, MPI_Datatype mpitype);
    /**
     * @brief Expands Buffer Dimension
     */
    void    ExpandBuffer (int more_dimension);
public:
    ~OOPMPIStorageBuffer();
    void CancelRequest();
    /**
     * @brief Contructor which initializes the buffer
     */
    OOPMPIStorageBuffer() : m_Buffer(1)
    {
		m_Size = 0;
		m_Length = 0;
		m_SenderTid = -1;
		m_MsgTag = 0;
		m_IsReceiving = 0;
		ResetBuffer();
    }
    /**
     * @brief Restores next object in the buffer
     */
    virtual TPZSaveable *Restore ();
    /**
	 * @brief Unpacks array of characteres from received buffer
	 * @param p Pointer to array for which elements must be unpacked
	 * @param n Number of elements to be unpacked
	 */
    int UpkByte (char *p, int n = 1);
    /**
     * @brief Unpacks array of integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkInt (int *p, int n = 1);
    /**
     * @brief Unpacks array of short integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkShort (short *p, int n = 1);
    /**
     * @brief Unpacks array of long integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkLong (long *p, int n = 1);
    /**
     * @brief Unpacks array of unsigned integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkUint (u_int * p, int n = 1);
    /**
     * @brief Unpacks array of unsigned short integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkUshort (u_short * p, int n = 1);
    /**
     * @brief Unpacks array of unsigned long integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkUlong (u_long * p, int n = 1);
    /**
     * @brief Unpacks array of float reals from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkFloat (float *p, int n = 1);
    /**
     * @brief Unpacks array of double reals from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkDouble (double *p, int n = 1);
    /**
     * @brief Unpacks a string from received buffer
     * @param str Pointer to string to  be unpacked
     */
    int UpkStr (char *str);
    /**
     * @brief Nonblocking receive. If there is a posted message to
     * be received, receives it and returns 1. Else, retuns 0
     */
    int Receive ();
    /**
     * @brief Checks on reception status
     */
    bool TestReceive();
    /**
     * @brief Blocking receive. Execution stops and awaits until a
     * posted message is received
     */
    int ReceiveBlocking ();
	
    /**
     * @brief Packs array of characteres into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkByte (const char *p, int n = 1);
	
    /**
     * @brief Packs array of integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkInt (int *p, int n = 1);
	
    /**
     * @brief Packs array of short integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkShort (short *p, int n = 1);
	
    /**
     * @brief Packs array of long integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkLong (long *p, int n = 1);
	
    /**
     * @brief Packs array of unsigned integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkUint (u_int * p, int n = 1);
	
    /**
     * @brief Packs array of unsigned short integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkUshort (u_short * p, int n = 1);
	
    /**
     * @brief Packs array of unsigned long integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkUlong (u_long * p, int n = 1);
	
    /**
     * @brief Packs array of float reals into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkFloat (float *p, int n = 1);
	
    /**
     * @brief Packs array of doubles into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkDouble (double *p, int n = 1);
	
    /**
     * @brief Packs a string into send buffer
     * @param str Pointer to string to be packed
     */
    int PkStr (char *str);
	
    /**
     * @brief Sends data stored in send buffer and clears it
     * @param msg_id Id of the message to be sent
     */
    int Send (int msg_id);
	
    /**
     * @brief Returns the dimension of the stored message
     */
    int Length ()
    {
		return (m_Length - sizeof (int));
    }
	
    /**
     * @brief Clears buffer
     */
    int ResetBuffer (int size = 0);
    /**
     * @brief Used for error management
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
	
};

typedef OOPMPIStorageBuffer *POOPMPIStorageBuffer;

#endif

