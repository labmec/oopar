// -*- c++ -*-
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// E-mail:  mauroesm@dcc.unicamp.br
//
// Arquivo: com_file.hh
//
// Classe:  TFileComManager
//
// Descr.:  Classe para comunicacao usando arquivos.
//
// Versao:  07 / 08 / 95.
//
#ifndef _COM_FILEHH_
#define _COM_FILEHH_
#include "ooperror.h"
#include "oopcommmanager.h"
#include "oopfilestorage.h"
#include "oopcommmanager.h"
class   OOPTask;
class   OOPSendStorageFile;
class   OOPTaskManager;
class   OOPSaveable;
/**
 * Implements communication manager functionality through files.
 * Very useful in a NIS / LDAP environment.
 * @author Mauro Enrique de Souza Munoz
 * @since 07/08/95
 */
class   OOPFileComManager:public OOPCommunicationManager
{
      public:
  /**
   Cria os buffers a serem usados na comunicacao e seta o prefixo
    "process_name" a ser usado nos nomes dos arquivos de
    comunicacao.
   Os nomes dos arquivos sao montados da seguinte forma:
            prefixo + '.' + proc_ID + XX
   Onde:
        prefixo = string de no maximo 8 bytes (DOS compativel).
        proc_ID = 1 digito ['0'..'9'] que identifica o processo.
        XX      = 2 digitos que Identificam a funcao do arquivo
                  == '00' - arquivo de recepcao de dados,
                  != '00' - arquivo usado como buffer temporario.
  */
	OOPFileComManager (char *prefix, int num_proc, int myID);
	        OOPFileComManager ();
	       ~OOPFileComManager ();
  /**
   Armazena "*pObject" no buffer enderecado para o processo
    "processID". O buffer so' e' enviado atraves de "SendMessage".
   Como "TSavable" e' uma classe base para varias outras,
    "SendObject" pode fazer um tratamento diferenciado de
    acordo com o objeto a ser enviado.
   Ex.: Passar todas as TTasks na frente.
  */
	int     SendTaskVrt (OOPTask * pObject /* , int processID */ );
  /**
   Recebe uma mensagem qualquer que tenha chegado. Se nao houver
   nenhuma mensagem, retorna 0.
   */
	int     ReceiveMessages ();
	int     ReceiveBlocking ()
	{
		return (ReceiveMessages ());
	}
  /**Envia todas as mensagens de todos os buffers.*/
	int     SendMessages ();
	char   *ClassName ()
	{
		return ("TFileComManager::");
	}
	int     SendTask (OOPTask * pTask);
      private:
  /**Prefixo para a montagem dos nomes dos arquivos.*/
	char    f_prefix[FILE_NAME_SIZE];
  /**
   Prefixo relativo a este processo. E' necessario manter uma
    string para o prefixo e outra para o prefixo deste processo
    porque os buffer mantem apenas um ponteiro para este prefixo
    evitando assim que cada buffer precise armazenar toda a
    string.
  */
	char    f_my_prefix[FILE_NAME_SIZE];
  /**1 buffer para cada processo do grupo.*/
	OOPSendStorageFile **f_buffer;
};
#endif // _COM_FILEHH_
