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
    TSmallTask(int Procid);

    ~TSmallTask();

    static OOPSaveable* Restore(OOPReceiveStorage* buf);
    virtual int Pack(OOPSendStorage* buf);
    virtual int Unpack(OOPReceiveStorage* buf);
    virtual long GetClassID();
    virtual OOPMReturnType Execute();

};

#endif
