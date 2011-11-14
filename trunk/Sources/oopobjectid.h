/**
 * @file
 */
#ifndef TOBJECTID_H
#define TOBJECTID_H
#include <iostream>
#include "pzfilebuffer.h"

/**
 * @brief Implements functionalities on the identification of any entity encountered
 * in the OOPar environment.
 * @ingroup managercomm
 */
/**
 * OOPar previous versions used simple long Identifiers.
 * Last created object on the DataManager need to be compliant to this class, in
 * the sense that fLastCreated object must keep track of an object of this type
 */
class   OOPObjectId
{
public:
	
    /**
     * @brief Returns true if the id is zero
     * @note The name changed to IsZeroOOP because of error on the CFDk integration,
     * be changed to the correct name as soon as possible.
     */
    int IsZeroOOP() const;
    operator bool() const
    {
		return (fId || fProcId);
    }
	
    /**
	 * @brief Packs itself in a buffer so it can be sent through the network.
	 * @param buf Buffer which is sent through the net.
	 */
    void     Write (TPZStream  & buf, int withclassid = 0);
    /**
     * @brief Unpacks itself from a buffer so it can be received through the network.
     * @param buf Buffer which is received through the net.
     */
    void    Read (TPZStream  & buf, void * context = NULL);
    /**
     * @brief Assign zero to ObjectId data structure
     */
    void    Zero ();
	
    /**
     * @brief Simple construtor 
     */
    OOPObjectId ();
    /**
     * @brief Copy constructor 
     */
    OOPObjectId (const::OOPObjectId & obj);
    /**
     * @brief Constructor with parameters
     * @param ProcId Initial processor Id for fProcId.
     * @param Id Initial Id.
     */
    OOPObjectId (long ProcId, long Id);
    /**
     * @brief Sets object id
     * @param id Id to be set 
     */
    void    SetId (int id);
    /**
     * @brief Sets processor id
     * @param id id to be set 
     */
    void    SetProcId (int id);
    /**
     * @brief Returns object Id 
     */
    int     GetId () const;
    /**
     * @brief Returns Id from the object
     */
    OOPObjectId Id ();
    /**
     * @brief Returns processor id 
     */
    int     GetProcId () const;
    /**
     * @brief Sets both object and processor id
     * @param id object Id to be set
     * @param procid processor Id to be set 
     */
    void    SetData (int id, int procid);
    /**
	 * @brief Operator attribution (=) overloaded
	 */
    OOPObjectId & operator= (const OOPObjectId & obj);
    /**
     * @brief Logical operator overloaded
     */
    bool    operator == (const OOPObjectId & obj) const;
    /**
     * @brief Logical operator overloaded
     */
    bool    operator >= (const OOPObjectId & obj) const;
    /**
     * @brief Logical operator which compares and OOPObjectId against an int/long value. \n
     * The check is made only with fId.
     */
    bool    operator >= (int val) const;
    bool    operator < (int val) const;
    bool    operator < (const OOPObjectId & id) const;
	
    /**
     * @brief Used for testing purposes 
     */
    static int main ();
    /**
     * @brief Prints TObjectId data 
     */
    void    Print (std::ostream & out) const;
    /**
     * @brief Prints TObjectId data 
     */
    std::ostream & ShortPrint (std::ostream & out) const;
private:
    /**
     * @brief Id of object 
     */
    int     fId;
    /**
     * @brief Identifies processor Id 
     */
    int     fProcId;
};

inline std::ostream &operator<<(std::ostream &out, const OOPObjectId &obj)
{
	return obj.ShortPrint(out);
}

#endif // TOBJECTID_H

