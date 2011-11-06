#include "TPartitionRelation.h"

#include "oopcommmanager.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include <iostream>
#include "TParAnalysis.h"
#include "TParCompute.h"
#include "TLocalCompute.h"
#include "TTaskComm.h"
#include "fluxdefs.h"
#include "OOPDataLogger.h"

using namespace std;


void    ParAddClass ();
int     multimain ();
int     singlemain ();

#ifdef OOP_MPI
int     mpimain (int argc, char *argv[]);
#endif

int main (int argc, char **argv)
{
#ifdef OOP_MPI
	return mpimain (argc, argv);
#else
	return multimain();
#endif
}

