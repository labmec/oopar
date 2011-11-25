/**
 * @file
 * @brief Contains the TParVector class.
 */
#ifndef TPARVECTOR_H
#define TPARVECTOR_H

#include <vector>

#include "pzsave.h"
#include "fluxdefs.h"

using namespace std;

/**
 * @brief Defines the vector to parallel process. \ref parutil "Utilitaries"
 * @ingroup parutil
 */
class TParVector:public TPZSaveable
{
public:
	virtual ~TParVector(){}
	TParVector ();
	virtual int    ClassId () const{
		return TPARVECTOR_ID;
	}
	virtual void Write (TPZStream & buf, int withclassid=0);
	virtual void Read (TPZStream & buf, void * context = 0);
	void    Resize (int size);
	void    SetVector (vector < double >data);
	static TPZSaveable * Restore(TPZStream & buf, void * context = 0);
private:
	vector < double >fData;
};

#endif // TPARRHS_H
