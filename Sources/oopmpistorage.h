
#ifndef _STR_MPIHH_
#define _STR_MPIHH_

#ifdef OOP_MPI
#include "mpi.h"
#endif
#ifdef OOP_MPE
#include "mpe.h"
#endif

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
    /**
     * Buffer which stores received messages 
     */
    TPZManVector<char,MAXSIZE> m_Buffer;
    //TPZManVector<char,MAXSIZE> f_send_buffr;
    /**
     * Dimension of received message 
     */
    int m_Size;
    //int f_send_size;
    /**
     * Receive buffer position to be unpack 
     */
    int m_Length;
    //int f_send_position;
    /**
     * Id of process that sent received message 
     */
    int m_SenderTid;
    /**
     * Tag of received message 
     */
    int m_MsgTag;
    /**
     * request object for non-blocking receive operation 
     */
    MPI_Request  m_Request;
    MPI_Status m_Status;

    /**
     * Id of process for which message shall be sent 
     */
    int m_TargetTid;

    /**
     * Flag indicating whether nonblocking reception is initiated 
     * Certainly it will hardly be used since Blocking receiving is the new default
     */
    int m_IsReceiving;
    /**
     * Generic method to pach data into an MPI buffer
     */
    int PackGeneric(void *ptr, int n, MPI_Datatype mpitype);
    /**
     * Expands Buffer Dimension
     */
    void    ExpandBuffer (int more_dimension);
#ifdef MTSENDONLY
    static void * SendMT(void *Data);
#endif
  public:
    ~OOPMPIStorageBuffer();
    void CancelRequest();
    /**
     * Contructor which initializes the buffer
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
     * Restores next object in the buffer
     */
    virtual TPZSaveable *Restore ();
    /**
    * Unpacks array of characteres from received buffer
    * @param p Pointer to array for which elements must be unpacked
    * @param n Number of elements to be unpacked
    */
    int UpkByte (char *p, int n = 1);
    /**
     * Unpacks array of integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkInt (int *p, int n = 1);
    /**
     * Unpacks array of short integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkShort (short *p, int n = 1);
    /**
     * Unpacks array of long integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkLong (long *p, int n = 1);
    /**
     * Unpacks array of unsigned integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkUint (u_int * p, int n = 1);
    /**
     * Unpacks array of unsigned short integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkUshort (u_short * p, int n = 1);
    /**
     * Unpacks array of unsigned long integers from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkUlong (u_long * p, int n = 1);
    /**
     * Unpacks array of float reals from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkFloat (float *p, int n = 1);
    /**
     * Unpacks array of double reals from received buffer
     * @param p Pointer to array for which elements must be unpacked
     * @param n Number of elements to be unpacked
     */
    int UpkDouble (double *p, int n = 1);
    /**
     * Unpacks a string from received buffer
     * @param p Pointer to string to  be unpacked
     */
    int UpkStr (char *str);
    /**
     * Nonblocking receive. If there is a posted message to
     * be received, receives it and returns 1. Else, retuns 0
     */
    int Receive ();
    /**
     * Checks on reception status
     */
    bool TestReceive();
    /**
     * Blocking receive. Execution stops and awaits until a 
     * posted message is received
     */
    int ReceiveBlocking ();

    /**
     * Packs array of characteres into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkByte (const char *p, int n = 1);

    /**
     * Packs array of integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkInt (int *p, int n = 1);

    /**
     * Packs array of short integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkShort (short *p, int n = 1);

    /**
     * Packs array of long integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkLong (long *p, int n = 1);

    /**
     * Packs array of unsigned integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkUint (u_int * p, int n = 1);

    /**
     * Packs array of unsigned short integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkUshort (u_short * p, int n = 1);

    /**
     * Packs array of unsigned long integers into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkUlong (u_long * p, int n = 1);

    /**
     * Packs array of float reals into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkFloat (float *p, int n = 1);

    /**
     * Packs array of doubles into send buffer
     * @param p Pointer to array which has the elements to be packed
     * @param n Number of elements to be packed
     */
    int PkDouble (double *p, int n = 1);

    /**
     * Packs a string into send buffer
     * @param p Pointer to string to be packed
     */
    int PkStr (char *str);

    /**
     * Sends data stored in send buffer and clears it
     * @param msg_id Id of the message to be sent
     */
    int Send (int msg_id);

    /**
     * Returns the dimension of the stored message
     */
    int Length ()
    {
      return (m_Length - sizeof (int));
    }

    /**
     * Clears buffer 
     */
    int ResetBuffer (int size = 0);
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

};
typedef OOPMPIStorageBuffer *POOPMPIStorageBuffer;
#endif


//OOP_MPI
