//
// Autor: Fábio Amaral de Castro, RA: 991722
//
// E-mail: facastro99@yahoo.com
//
// Arquivo: com_mpi.h
//
// Classe: TMpiComManager
//
// Descr.: Classe para comunicacao utilizando MPI
//
// Versao: 12 / 2002
//

#include "mpi.h"
#include "ooperror.h"
#include "oopcommmanager.h"
#include <stdio.h>
class OOPMPISendStorage;

class OOPMPICommManager: public OOPCommunicationManager
{
 public:
  
  OOPMPICommManager();
  ~OOPMPICommManager();

  int Initialize(int argc, char *argv[]);

  int SendTask(TTask *pTask);
  
  int ReceiveMessages();

  int SendMessages();

  int IAmTheMaster();

  char *ClassName();

 protected:

  OOPMPISendStorage **f_buffer;
  //int *f_proc;

};

    
