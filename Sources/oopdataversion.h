/* Generated by Together */
#ifndef TDATAVERSION_H
#define TDATAVERSION_H
#include <iostream>
#include <vector>
#include <stack>
#include "pzfilebuffer.h"
using namespace std;
/**
 * Implements the data versioning functionality.
 * Incorporates cardinality for the data versioning.
 * Allows for a more complex data versioning treatment.
 */
class   OOPDataVersion
{
public:
	/**
	 * Sets the version for the specified level
	 * @param level Level to be changed
	 * @param version Version to be set on level
	 */
	void    SetLevelVersion (int level, int version);
	/**
	 * Packs the object in buffer so it can be sent through the network.
	 * @param buf Buffer which implements the communication.
	 */
	void     Write (TPZStream & buf);
	/**
	 * Unpacks the object in buffer so it can be received through the network.
	 * @param buf Buffer which implements the communication.
	 */
	void     Read (TPZStream & buf);
	/**
	 * Returns class names
	 */
	const char *MyName ();
	/**
	 * Checks wheter two versions are compatible
	 * @param version : object TDataVersion to checked against
	 * The checked parameter is mostly the current version of
	 * the MetaData on the DM. In a scenario where a Task requires a MetaData with
	 * a version which is newer than the actual MetaData version, false will be 
	 * returned.
	 * This is due to the fact that a version will NEVER decrease (i.e. The
	 * version I am needing will never be reached (again).
	 * @return Returns true if the checked parameter is older than me. Returns
	 * false otherwise.
	 */
	bool    AmICompatible (const OOPDataVersion & version) const;
	/**
	 * Returns true when the state of the current data is sufficient
	 * for a task to start executing
	 */
	bool    CanExecute (const OOPDataVersion & version) const;
	/**
     * Increments version with a call to Increment method
	 */
	void    operator ++ ();
	/**
     * Streams out TDataVersion data structure
	 */
	void    Print (ostream & out = cout) const;
	ostream & ShortPrint(ostream &out = cout) const;
	/**
	 * Operator overloaded
	 */
	        OOPDataVersion & operator = (const OOPDataVersion & version);
	/**
	 * Operator overloaded
	 */
	bool    operator == (const OOPDataVersion & version) const;
	/**
	 * Overloaded operator
	 */
	bool    operator < (const OOPDataVersion & version);
	bool    operator > (const OOPDataVersion & version);
	/**
     * Implements the abstraction of data version incrementation.
	 * The class will perform necessary checks to either increment only
	 * its cardinality or also its level. 
     */
	void    Increment ();
	/**
	 * Adds a new level to the dataversion object
	 * @param cardinality : New level is created with depth cardinality
	 */
	void    IncrementLevel (int cardinality);
	/**
	 * Deletes last level
	 */
	void    DecreaseLevel ();
    /**
     * Simple constructor 
     */
	        OOPDataVersion ();
    /**
     * Constructor setting n Level parameter. Depth will be set to 1.
     * @param Level : Level to be set 
     */
	        OOPDataVersion (int cardinality);
    /**
     * Copy constructor 
     */
	        OOPDataVersion (const::OOPDataVersion & version);
	/**
	 * Ordinary destructor
	 */
	       ~OOPDataVersion ();
    /**
     * Returns current counter for specified Level
     */
	int     GetLevelVersion (int level) const;
	/**
	 * Returns level cardinality
	 */
	int     GetLevelCardinality (int level) const;
	/**
	 * Returns total number of levels
	 */
	int     GetNLevels () const;
	/**
	 * Returns vector containing all levels depths
	 */
	        vector < int >GetLevelCardinality () const;
	/**
	 * Returns vector containing all level counters
	 */
	        vector < int >GetLevelVersion () const;
	/**
	 * Sets the cardinality vector
	 */
	void    SetData (vector < int >&card, vector < int >&version);
 
	/**
	 * Get the cardinality vector
	 */
        void    GetData (vector < int >&card, vector < int >&version);
    /**
     * Sets the Cardinality depth 
     */
	void    SetLevelCardinality (int level, int depth);
    /**
     * Method used for testing.
     * Mostly performs tests on all functionalities provided by the class 
     */
	static void main ();
      private:
	/**
	 * The cardinality of each level
	 */
	        vector < int >fLevelCardinality;
	/**
	 * The version for each level
	 */
	        vector < int >fVersion;
};
inline ostream & operator<<(ostream &out, const OOPDataVersion &obj) {
	return obj.ShortPrint(out);
}
#endif // TDATAVERSION_H
