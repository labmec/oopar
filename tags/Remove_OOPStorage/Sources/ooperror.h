// -*- c++ -*-
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: error.hh
//
// Classe:  TError
//
// Descr.:  Manipula as mensagens de erro. Cada  classe  derivada
//          de TError pode ter um arquivo proprio para gravar  as
//          mensagens ou pode usar os metodos G.... Estes metodos
//          farao com que a  chamada  se  referencie a um arquivo
//          unico (global) para todo o programa.
//
// Versao:  27 / 05 / 95.
//
#ifndef _ERRORHH_
#define _ERRORHH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pzfilebuffer.h"
/**
 * It handle error messages, Each class derived from TError can have its own error log files
 * or it can make use of the methods G... Such methods will reffer to a unique global file.
 * @author Mauro Enrique de Souza Munoz
 * @since 27/05/95
 */
class   OOPError : public TPZStream
{
      public:
  /**
   * Simple constructor
   */
	OOPError ()
	{
		*f_local = '\0';
		f_lstate = f_to_global = 1;
	}
  /**
   * Selects the file where the messages will be written
   * In case file == NULL sends messages to stderr
   * @param file File name
   */
	void    SetErrorFile (char *file)
	{
		strcpy (f_local, file);
	}
static void    GSetErrorFile (char *gfile)
	{
		strcpy (f_global, gfile);
	}
	char   *GetErrorFile ()
	{
		return (f_local);
	}
	char   *GGetErrorFile ()
	{
		return (f_global);
	}
  /**
   * Redirects the calls to the global file
   */
	void    SetToGlobal (int v)
	{
		f_to_global = v;
	}
  /**
   * Erases or initializes the error file
   */
	void    ClearErrorFile ();
  /**
   * Prints error message and terminates the execution
   */
	void    Error (int exit_status, char *m1, char *m2 = "");
  /**
   * Prints an error message but does not terminates the execution
   * @param m1 Holds one of the following %s, %d or %f according to m2
   * @param m2 Undocumented
   */
	void    Debug (char *m1, char *m2 = "");
	void    Debug (char *m1, int m2);
	void    Debug (char *m1, float m2);
  /**
   * Sends error message according to state
   * @param state Decision flag, if state == 0 no messade is sent.
   */
	void    SetErrorState (int state)
	{
		f_lstate = state;
	}
  /**
   * Returns class name. Since it is a virtual method all derived classes must redefine this method.
   */
	virtual char *ClassName ()
	{
		return ("");
	}
      private:
  /**
   * Clear error log file and state.
   * @param fname Log file name.
   * @param state New state to be set.
   */
	void    Clear (char *fname, int state);
	void    IDebug (char *fname, int state, char *m1, char *m2);
	void    IDebug (char *fname, int state, char *m1, int m2);
	void    IDebug (char *fname, int state, char *m1, float m2);
  /**
   * Holds global file name
   */
	static char f_global[128];
  /**
   * Holds local file name
   */
	char    f_local[128];
  /**
   * Decision flag
   */
	int     f_lstate;
  /**
   * Flag for the local global relation
   */
	int     f_to_global;
};
#endif // _ERRORHH_
