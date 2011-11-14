/**
 * @file
 */

#ifndef OOPINT_H
#define OOPINT_H
#include "pzsave.h"
#include "ooppardefs.h"
class OOPInt : public TPZSaveable {
public:    
    int fValue;
	OOPInt();
    void Write(TPZStream & buf, int withclassid);
	
    void Read(TPZStream & buf, void * context);
	
    virtual int ClassId() const ;
	
    
};

#endif //OOPINT_H
