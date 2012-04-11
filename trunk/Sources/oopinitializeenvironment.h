//
//  oopinitializeenvironment.h
//  OOPAR
//
//  Created by Philippe Devloo on 4/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef OOPAR_oopinitializeenvironment_h
#define OOPAR_oopinitializeenvironment_h

#include "oopcommmanager.h"

TPZAutoPointer<OOPTaskManager> InitializeEnvironment(int numproc, int numthreads);

#endif
