//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: str_file.cc
//
// Classe:  TSendStorageFile, TReceiveStorageFile.
//
// Descr.:  TSendStorageFile:
//            Classe derivada de TSendStorage que implementa o buffer
//            usando arquivos para a comunicacao entre processos.
//
//          TReceiveStorageFile:
//            Classe derivada de TReceiveStorage que implementa o buffer
//            usando arquivos para a comunicacao entre processos.
//
// Versao:  09 / 08 / 95.
//
#include "oopfilestorage.h"
#include "oopsaveable.h"
#include "cmdefs.h"
class   OOPSendStorageFile;
class   OOPReceiveStorageFile;
#define HEADER_SIZE   (sizeof(int))
/************************ TSendStorageFile ************************/
/*******************/
/*** Constructor ***/
OOPSendStorageFile::OOPSendStorageFile (char *prefix, int my_id)
{
	f_myID = my_id;
	f_prefix = prefix;
	f_wrote = 0;
	f_file_num = 0;
	f_file = NULL;
	// Ja inicializa 'f_file' e 'f_wroten'.
	if (!FindNewBuffer ())
		Error (1, "Constructor <can't find a new buffer>\n");
}
/******************/
/*** Destructor ***/
OOPSendStorageFile::~OOPSendStorageFile ()
{
	// Elimina as mensagens que nao foram enviadas.
	char    file_name[FILE_NAME_SIZE];
	sprintf (file_name, "%s%02d", f_prefix, f_file_num);
	remove (file_name);
}
/***************/
/*** Packing ***/
int OOPSendStorageFile::PkByte (char *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fprintf (f_file, "%+d ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkInt (int *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fprintf (f_file, "%+d ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkShort (short *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fprintf (f_file, "%+d ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkLong (long *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fprintf (f_file, "%+ld ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkUint (u_int * p, int n)
{
	for (int i = 0; i < n; i++)
		if (fprintf (f_file, "%d ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkUshort (u_short * p, int n)
{
	unsigned long i;
	for (i = 0; i < (unsigned long) n; i++)
		if (fprintf (f_file, "%d ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkUlong (u_long * p, int n)
{
	unsigned long i;
	for (i = 0; i < (unsigned long) n; i++)
		if (fprintf (f_file, "%d ", (int) *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkFloat (float *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fprintf (f_file, "%+f ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkDouble (double *p, int n)
{
	for (int i = 0; i < n; i++)
//    if ( fprintf( f_file, "%+f ", *p++ ) == EOF )
		if (fprintf (f_file, "%le ", *p++) == EOF)
			return (i);
	fprintf (f_file, "\n");
	return (n);
}
int OOPSendStorageFile::PkStr (char *str)
{
	int ret = fprintf (f_file, "%s", str);
	fprintf (f_file, "\n");
	return (ret);
}
/******************************/
/*** Has Some Thing To Send ***/
int OOPSendStorageFile::HasSomeThingToSend (char *file_to_send)
{
	// Nao tem nada a ser enviado.
	if (!f_wrote)
		return (0);
	// Finaliza e fecha o buffer (arquivo) a ser enviado.
	Open ();
	char end_of_msg = 0;
	PkByte (&end_of_msg);
	Close ();
	sprintf (file_to_send, "%s%02d", f_prefix, f_file_num);
	if (!FindNewBuffer ())
		Error (1, "HasSomeThingToSend <can't find a new buffer>\n");
	return (1);
}
/************/
/*** Open ***/
int OOPSendStorageFile::Open ()
{
	// Monta o nome do arquivo em que esta ligado atualmente.
	char related_file[FILE_NAME_SIZE];
	sprintf (related_file, "%s%02d", f_prefix, f_file_num);
	if ((f_file = fopen (related_file, "a")) == NULL)
		Error (1, "Open <can't open related file>\n");
	return (1);
}
/*************/
/*** Close ***/
int OOPSendStorageFile::Close ()
{
	if (!f_file)
		return 0;
	f_wrote =
		((unsigned int) ftell (f_file) >
		 (unsigned int) HEADER_SIZE) ? 1 : 0;
	int closeres = fclose (f_file);
	f_file = 0;
	return closeres;
}
/*****************/
/*** File Name ***/
void OOPSendStorageFile::FileName (char *name)
{
	sprintf (name, "%s%02d", f_prefix, f_file_num);
}
/**************** Private *****************/
/***********************/
/*** Find New Buffer ***/
int OOPSendStorageFile::FindNewBuffer ()
{
	char new_file[FILE_NAME_SIZE];
	if (f_file != NULL)
		fclose (f_file);
	f_file = NULL;
	// Procura por um arquivo nao existente para ser usado como buffer.
	int i;
	for (i = 1; i < 100; i++) {
		// Testa se o arquivo ja existe.
		sprintf (new_file, "%s%02d", f_prefix,
			 (f_file_num + i) % 100);
		if ((f_file = fopen (new_file, "r+")) != NULL) {
			fclose (f_file);
			f_file = 0;
		}
		else {
			// Se o arquivo ainda nao existir, apossa-se dele.
			f_file_num = (f_file_num + i) % 100;
			break;
		}
	}
	// Se nao encontrou nenhum arquivo livre.
	if (i == 100)
		Error (1, "FindNewBuffer <cannot find a new file buffer>\n");
	// Indica que o arquivo e' virgem.
	f_wrote = 0;
	// Se encontrou um nome de arquivo inexistente...
	if (f_file == NULL) {
		// Abre o novo arquivo.
		if ((f_file = fopen (new_file, "w")) == NULL)
			return (0);
		// Escreve o HEADER da mensagem.
		PkInt (&f_myID);	// HEADER_SIZE = sizeof(int);
		fclose (f_file);
		f_file = 0;
		return (1);
	}
	else
		return (0);
}
/************************ TReceiveStorageFile ************************/
/*******************/
/*** Constructor ***/
OOPReceiveStorageFile::OOPReceiveStorageFile (char *fname)
{
	strcpy (f_file_name, fname);
	if ((f_file = fopen (f_file_name, "r")) == NULL)
		Error (1, "Constructor <can't open file '%s'>\n",
		       f_file_name);
	// Le o HEADER da mensagem.
	UpkInt (&f_sender);
}
/******************/
/*** Destructor ***/
OOPReceiveStorageFile::~OOPReceiveStorageFile ()
{
	fclose (f_file);
	f_file = 0;
	if (remove (f_file_name))
		Error (1, "Destructor <Nao consegui remover o arquivo %s\n",
		       f_file_name);
}
/*****************/
/*** Unpacking ***/
int OOPReceiveStorageFile::UpkByte (char *p, int n)
{
	int c;
	for (int i = 0; i < n; i++) {
		if (fscanf (f_file, "%d", &c) != 1)
			return (i);
		*p++ = (char) c;
	}
	return (n);
}
int OOPReceiveStorageFile::UpkInt (int *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%d", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkShort (short *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%hd", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkLong (long *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%ld", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkUint (u_int * p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%u", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkUshort (u_short * p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%hu", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkUlong (u_long * p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%lu", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkFloat (float *p, int n)
{
	for (int i = 0; i < n; i++)
		if (fscanf (f_file, "%f", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkDouble (double *p, int n)
{
	for (int i = 0; i < n; i++)
//    if ( fscanf( f_file, "%lf", p++ ) != 1 )
		if (fscanf (f_file, "%le", p++) != 1)
			return (i);
	return (n);
}
int OOPReceiveStorageFile::UpkStr (char *str)
{
	return fscanf (f_file, "%s", str);
}
