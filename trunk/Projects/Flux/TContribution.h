/* Generated by Together */

#ifndef TCONTRIBUTION_H
#define TCONTRIBUTION_H

#include "oopobjectid.h"
#include <vector>
using namespace std;
/**
 * Implements the necessary information regarded to contribution from one parti-
 * tion �i� to a partition �j�.
 * Objects of this class populates a matrix which relates partition �i� to par-
 * tition �j�. Each (i,j) member of this matrix holds a copy of TComtribution.
 * Implements the number of contributing vectors the partition �i� contains
 * addressed to partition �j�.
 */


class TContribution {
public:

    /**
     * Returns the contribution vector identified by the index parameter
     * @param Index Identifies each vector to be returned
     * @since 23/05/2003 
     */
    vector<int> &DestinationVector(int index);

    void InitializeRandom();

    TContribution();

    int NContributions() { return fNContributions;}

private:    

    /**
     * ObjectId of the destination mesh
     */
    OOPObjectId fDestinationMesh;

    /**
     * Holds the volume indices of the origin partition. 
     */
    vector<int> fFrom;

    /**
     * Holds the volume indices which will receive the contributions. 
     */
    vector<int> fTo;

    /**
     * Holds the indexes of local TMBData which will contribute on neighbor par-
     * tition. Each element on the TMBData has its destination indexes on each
     * TMBData of the fDestination
     */
    vector<vector<int> > fLocalIndices;

    /**
     * Indicates the number of contributions between fFrom and fTo partitions. 
     * This data could be obtained from the size of the vectors
     */
    int fNContributions;

};
#endif //TCONTRIBUTION_H