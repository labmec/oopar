
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: error.cc
//
// Classe:  TError
//
// Descr.:  Manipula as mensagens de erro.
//
// Versao:  27 / 05 / 95.
//


#include <stdio.h>
#include "ooperror.h"


char OOPError::f_global[128] = "";


/************************ Private ************************/

/************************/
/*** Clear Error File ***/
void OOPError::ClearErrorFile ()
{
	char   *fname = (f_to_global ? f_global : f_local);
	int state = (f_to_global ? 1 : f_lstate);

	if (state && fname != NULL) {
		FILE   *file = fopen (fname, "w+");
		if (file == NULL) {
			fprintf (stderr,
				 "OOPError::ClearErrorFile <can't open %s>.\n",
				 fname);
			exit (1);
		}
		fprintf (file, "\n");
		fclose (file);
	}
}



/**************/
/*** Debug ***/
void OOPError::Debug (char *msg1, char *msg2)
{
	char   *fname = (f_to_global ? f_global : f_local);
	if (!f_to_global && !f_lstate)
		return;

	char msg[256];
	sprintf (msg, "%s%s", ClassName (), msg1);

	if (fname == NULL)
		fprintf (stderr, msg, msg2);
	else {
		FILE   *file = fopen (fname, "a");
		if (file == NULL) {
			fprintf (stderr, "OOPError::Error <error open %s>.\n",
				 fname);
			exit (1);
		}
		fprintf (file, msg, msg2);
		fclose (file);
	}
}


/**************/
/*** Debug ***/
void OOPError::Debug (char *msg1, int msg2)
{
	char   *fname = (f_to_global ? f_global : f_local);
	if (!f_to_global && !f_lstate)
		return;

	char msg[256];
	sprintf (msg, "%s%s", ClassName (), msg1);

	if (fname == NULL)
		fprintf (stderr, msg, msg2);
	else {
		FILE   *file = fopen (fname, "a");
		if (file == NULL) {
			fprintf (stderr, "TError::Error <error open %s>.\n",
				 fname);
			exit (1);
		}
		fprintf (file, msg, msg2);
		fclose (file);
	}
}


/**************/
/*** Debug ***/
void OOPError::Debug (char *msg1, float msg2)
{
	char   *fname = (f_to_global ? f_global : f_local);
	if (!f_to_global && !f_lstate)
		return;

	char msg[256];
	sprintf (msg, "%s%s", ClassName (), msg1);

	if (fname == NULL)
		fprintf (stderr, msg, msg2);
	else {
		FILE   *file = fopen (fname, "a");
		if (file == NULL) {
			fprintf (stderr, "TError::Error <error open %s>.\n",
				 fname);
			exit (1);
		}
		fprintf (file, msg, msg2);
		fclose (file);
	}
}

/*************/
/*** Error ***/
void OOPError::Error (int exit_status, char *msg1, char *msg2)
{
	Debug (msg1, msg2);
	exit (exit_status);
}
