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
#include "oopstorage.h"
#include "pzmanvector.h"
#include "mpi.h"
/** 
 * Non abstract class, which implements the send buffer
 * using MPI (Message Passing Interface) library for 
 * communication.
*/
class   OOPMPISendStorage:public OOPSendStorage
{
      public:
  /**
   * Constructor
   * @param f_target Processor Id for which buffer may be sent
   */
	OOPMPISendStorage ();
  /**
   * Packs array of characteres into send buffer
   * @param p Pointer to array which has the elements to be packed
   * @param n Number of elements to be packed
   */
	int     PkByte (char *p, int n = 1);
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
	// Retorna tamanho da mensagem contida no buffer
	// Ignora os dois primeiros caracteres do buffer, que é utilizado
	// apenas
	// para guardar a sua dimensao quando este é enviado. Ou nao?
  /**
   * Returns the dimension of the stored message
   */
	int     Length ()
	{
		return (f_position - sizeof (int));
	}
  /**
   * Used for error management
   */
	char   *ClassName ()
	{
		return ("TSendStorageMpi::");
	}
private:

/**
 * Generic method to pach data into an MPI buffer
 */
 int PackGeneric(void *ptr, int n, int mpitype);
  /**
   * Clears send buffer 
   */
	int     ResetBuffer ();
  /**
   * Expands send buffer dimension
   */
	void    ExpandBuffer (int more_dimension);
  /** Stores the message to be sent */
	TPZManVector<char,50000>   f_buffr;
  /** Length os message to be sent. Must aways
   * be equal or lower than number of elements of f_buffr */
	int     f_position;
  /** Id of process for which message shall be sent */
	int     f_target_tid;
};
typedef OOPMPISendStorage *POOPMPISendStorage;
/** 
 * Non abstract class, which implements the receive
 * buffer using MPI (Message Passing Interface) library 
 * for communication.
*/
class   OOPMPIReceiveStorage:public OOPReceiveStorage
{
private:
  /** Buffer which stores received messages */
	TPZManVector<char,5000> f_buffr;
  /** Dimension of received message */
	int     f_size;
  /** Receive buffer position to be unpack */
	int     f_position;
  /** Id of process that sent received message */
	int     f_sender_tid;
  /** Tag of received message */
	int     f_msg_tag;
  /** request object for non-blocking receive operation */
	MPI_Request f_request;
  /** flag indicating whether nonblocking reception is initiated */
    int     f_isreceiving;

public:
	/**
     * Contructor which initializes the buffer
	 */
   OOPMPIReceiveStorage() : f_buffr(50000) {
	   f_size = 0; f_position = 0; f_sender_tid = -1; f_msg_tag = 0; f_isreceiving = 0;
   }
  /**
   * Restores next object in the buffer
   */
	virtual OOPSaveable *Restore ();
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
   * Used for error management
   */
	char   *ClassName ()
	{
		return ("TReceiveStorageMpi::");
	}
};
typedef OOPMPIReceiveStorage *POOPMPIReceiveStorage;
#endif
