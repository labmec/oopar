//
// C++ Interface: tsmalltask
//
// Description: 
//
//
// Author: Philippe R. B. Devloo <phil@fec.unicamp.br>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TSMALLTASK_H
#define TSMALLTASK_H

#include <ooptask.h>

#define TSMALLTASKID 5000
/**
This is a class which only increments the version counter of the objects from which it depends

@author Philippe R. B. Devloo
*/
class TSmallTask : public OOPTask
{
public:
    TSmallTask(int Procid = -1);

    ~TSmallTask();

    virtual int    ClassId () const;
    virtual void Write (TPZStream & buf, int withclassid=0);
    virtual void Read (TPZStream & buf, void * context = 0);
    static TPZSaveable* Restore(TPZStream & buf, void * context);
    virtual OOPMReturnType Execute();

};



#endif
