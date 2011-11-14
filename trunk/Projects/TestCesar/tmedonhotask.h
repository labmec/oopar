//
// C++ Interface: tmedonhotask
//
// Description: 
//
//
// Author: Edimar Cesar Rylo <cesar@labmec.fec.unicamp.br>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TMEDONHOTASK_H
#define TMEDONHOTASK_H

#include <ooptask.h>

/**
 Teste de entendimento de programa paralelo usando o OOPar
 
 
 @author Edimar Cesar Rylo
 */
class TMedonhoTask : public OOPTask
{
public:
    /**
     * Default Constructor
     */
    TMedonhoTask();
    TMedonhoTask(int Procid, int start, int end);
    
    /**
     * Default Destructor
     */
    ~TMedonhoTask();
    
    /**
     * identificador da classe
     */
    virtual int    ClassId () const {  
		return 37882396;
    }
    
    /**
     * pack method
     */
    virtual void Write (TPZStream & buf, int withclassid=0);
    
    /**
     * unpack method
     */
    virtual void Read (TPZStream & buf, void * context = 0);
    
    //static TPZSaveable* Restore(TPZStream & buf, void * context);
    
    /**
     * acredito que seja a tarefa que realmente é executada
     */
    virtual OOPMReturnType Execute();
    
    
    void SetLimits (int st, int end) {
		fStart = st;
		fEnd = end;
    }
    
protected:
    int fStart;
    
    int fEnd;
    
};

#endif
