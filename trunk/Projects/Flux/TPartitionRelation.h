/**
 * @file
 */
#ifndef TPARTITIONRELATION_H
#define TPARTITIONRELATION_H

#include "TContribution.h"
#include "fluxdefs.h"

#include <vector>
#include "pzsave.h"

/**
 * @brief This class describe the relationship between the partitions of a mesh
 */
class TPartitionRelation : public TPZSaveable
{
	
public:
	virtual ~TPartitionRelation(){}
	virtual int ClassId () const {
		return TPARTITIONRELATION_ID;
	}
	TPartitionRelation ();
	TPartitionRelation (int npart);
	static TPartitionRelation *CreateRandom (int numpart);
    /**
     * @brief Returns the total number of 
     */
	int     GetNPartitions ();
	
	TContribution & GetRelation (int parfrom, int parto);
	int     IncomingContribution (int partition);
    /**
     * @brief Returns a vector containing the ID of all the partitions to each the local partitions will contribute to. 
     */
	int     OutgoingContribution (int partition);
	/**
	 * @brief Returns the processor associated with the partition
	 */
	int Processor(int partition) {
		return fProcessor[partition];
	}
	void    Print (ostream & out);
    /**
     * @brief Inform to the relation table the mesh id
     */
	void    SetMeshId (int index, OOPObjectId & meshid);
	/**
	 * @brief Packs the object in on the buffer so it can be transmitted through the network. \n
	 * The Pack function  packs the object's class_id while function Unpack() doesn't,
	 * allowing the user to identify the next object to be unpacked.
	 * @param buf A pointer to TSendStorage class to be packed.
	 */
	virtual void Write (TPZStream & buf, int withclassid = 0);
	/**
	 * @brief Unpacks the object class_id
	 * @param buf A pointer to TSendStorage class to be unpacked.
	 */
	virtual void Read (TPZStream & buf, void * context = 0);
	static TPZSaveable *Restore (TPZStream & buf, void * context = 0);
private:
	int     fNumPartitions;
	/**
	 * @brief Indicates the processor where each partition resides
	 */
	vector<int> fProcessor;
	
	vector < vector < TContribution > >fRelation;
};


#endif // TPARTITIONRELATION_H
