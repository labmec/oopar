// -*- c++ -*-
/* Generated by Together */
#ifndef TCONTRIBUTION_H
#define TCONTRIBUTION_H
#include "oopobjectid.h"
#include "fluxdefs.h"
#include <vector>
class   OOPDataVersion;
using namespace std; 
/**
 * Implements the necessary information regarded to contribution from one parti-
 * tion �i� to a partition �j�.
 * Objects of this class populates a matrix which relates partition �i� to par-
 * tition �j�. Each (i,j) member of this matrix holds a copy of TComtribution.
 * Implements the number of contributing vectors the partition �i� contains
 * addressed to partition �j�.
 */
class   TContribution
{
      public:
	virtual int ClassId () const
	{
		return TCONTRIBUTION_ID;
	}
		  
    /**
     * Returns the contribution vector identified by the index parameter
     * @param Index Identifies each vector to be returned
     * @since 23/05/2003 
     */
	vector < int >&DestinationVector (int index);
    /**
     * Initializes the structure randomically
     * @since 13/06/2003 
     */
	void    InitializeRandom ();
	        TContribution ();
    /**
     * returns the number of volume indices to which relation will contribute
     */
	int     NContributions ()
	{
		return fNContributions;
	}
    /**
     * returns whether this contribution holds a single index or not
     * @return true if no relation
     */
	int     IsEmpty ();
	/**
	 * Operator overloaded
	 */
	TContribution & operator = (const TContribution & contr);
	/**
	 * Operator overloaded
	 */
	bool    operator == (const TContribution & contr);
	void    SetMeshId (OOPObjectId & id)
	{
		fDestinationMesh = id;
	}
  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
	void Write (TPZStream & buf);
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
	void Read (TPZStream & buf, void * context = 0);
      private:
    /**
     * ObjectId of the destination mesh
     */
	OOPObjectId fDestinationMesh;
    /**
     * Holds the volume indices of the origin partition. 
     */
	vector < int >fFrom;
    /**
     * Holds the volume indices which will receive the contributions. 
     */
	vector < int >fTo;
    /**
     * Holds the indexes of local TMBData which will contribute on neighbor par-
     * tition. Each element on the TMBData has its destination indexes on each
     * TMBData of the fDestination
     */
	vector < vector<int> >fLocalIndices;
    /**
     * Indicates the number of contributions between fFrom and fTo partitions. 
     * This data could be obtained from the size of the vectors
     */
	int     fNContributions;
};
#endif // TCONTRIBUTION_H
