/**
 * @file
 */
#ifndef TDATAVERSION_H
#define TDATAVERSION_H
#include <iostream>
#include <vector>
#include <stack>
#include "pzfilebuffer.h"
using namespace std;
/**
 * @brief Implements the data versioning functionality.
 * @ingroup managerdata
 */
/**
 * Incorporates cardinality for the data versioning.
 * Allows for a more complex data versioning treatment.
 */
class   OOPDataVersion
{
public:
	/**
	 * @brief Sets the version for the specified level
	 * @param level Level to be changed
	 * @param version Version to be set on level
	 */
	void    SetLevelVersion (int level, int version);
	/**
	 * @brief Packs the object in buffer so it can be sent through the network.
	 * @param buf Buffer which implements the communication.
	 */
	void     Write (TPZStream & buf, int withclassid = 0);
	/**
	 * @brief Unpacks the object in buffer so it can be received through the network.
	 * @param buf Buffer which implements the communication.
	 */
	void     Read (TPZStream & buf, void * context = NULL);
	/**
	 * @brief Returns class names
	 */
	const char *MyName ();
	/**
	 * @brief Checks wheter two versions are compatible
	 * @param version : object TDataVersion to checked against
	 * @note The checked parameter is mostly the current version of
	 * the MetaData on the DM. In a scenario where a Task requires a MetaData with
	 * a version which is newer than the actual MetaData version, false will be 
	 * returned. \n
	 * This is due to the fact that a version will NEVER decrease (i.e. The
	 * version I am needing will never be reached (again).
	 * @return Returns true if the checked parameter is older than me. Returns false otherwise.
	 */
	bool    AmICompatible (const OOPDataVersion & version) const;
	/**
	 * @brief Returns true when the state of the current data is sufficient
	 * for a task to start executing.
	 * @note The current object stands for the requested version
	 * the parameter stands for the version of the data
	 */
	bool    CanExecute (const OOPDataVersion & dataversion) const;
	/**
     * @brief Increments version with a call to Increment method
	 */
	void operator ++ ();
	void operator ++ (int);
	/**
	 * @brief Streams out TDataVersion data structure
	 */
	void    Print (std::ostream & out) const;
	std::ostream & ShortPrint(std::ostream &out) const;
	/**
	 * @brief Operator overloaded
	 */
	OOPDataVersion & operator = (const OOPDataVersion & version);
	/**
	 * @brief Operator overloaded
	 */
	bool    operator == (const OOPDataVersion & version) const;
	/**
	 * @brief Overloaded operator
	 */
	bool    operator < (const OOPDataVersion & version) const;
	bool    operator > (const OOPDataVersion & version);
	bool operator >= (const OOPDataVersion & version);
	/**
	 * @brief Implements the abstraction of data version incrementation. \n
	 * The class will perform necessary checks to either increment only
	 * its cardinality or also its level. 
	 */
	void    Increment ();
	/**
	 * @brief Adds a new level to the dataversion object
	 * @param cardinality : New level is created with depth cardinality
	 */
	void    IncrementLevel (int cardinality);
	/**
	 * @brief Deletes last level
	 */
	void    DecreaseLevel ();
	/**
	 * @brief Simple constructor 
	 */
	OOPDataVersion ();
	/**
	 * @brief Constructor setting n Level parameter. Depth will be set to 1.
	 * @param cardinality Level to be set 
	 */
	OOPDataVersion (int cardinality);
    /**
     * @brief Copy constructor 
     */
	OOPDataVersion (const::OOPDataVersion & version);
	/**
	 * @brief Ordinary destructor
	 */
	~OOPDataVersion ();
    /**
     * @brief Returns current counter for specified Level
     */
	int     GetLevelVersion (int level) const;
	/**
	 * @brief Returns level cardinality
	 */
	int     GetLevelCardinality (int level) const;
	/**
	 * @brief Returns total number of levels
	 */
	int     GetNLevels () const;
	/**
	 * @brief Returns vector containing all levels depths
	 */
	vector < int >GetLevelCardinality () const;
	/**
	 * @brief Returns vector containing all level counters
	 */
	vector < int >GetLevelVersion () const;
	/**
	 * @brief Sets the cardinality vector
	 */
	void    SetData (vector < int >&card, vector < int >&version);
	
	/**
	 * @brief Get the cardinality vector
	 */
	void    GetData (vector < int >&card, vector < int >&version);
    /**
     * @brief Sets the Cardinality depth 
     */
	void    SetLevelCardinality (int level, int depth);
    /**
     * @brief Method used for testing.
     * Mostly performs tests on all functionalities provided by the class 
     */
	static void main ();
private:
	/**
	 * @brief The cardinality of each level
	 */
	std::vector < int >fLevelCardinality;
	/**
	 * @brief The version for each level
	 */
	std::vector < int >fVersion;
};

inline std::ostream & operator<<(std::ostream &out, const OOPDataVersion &obj) {
	return obj.ShortPrint(out);
}

#endif // TDATAVERSION_H
