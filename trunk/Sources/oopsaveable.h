
//
// Autor:   Mauro Enrique de Souza Munoz,  RA: 911472.
//
// Arquivo: saveable.hh
//
// Classe:  TSaveable.
//
// Descr.:  Classe base para todos objetos que quiserem usufruir do
//          Communication Manager (ver "communic.hh")
//
// Versao:  02 / 04 / 96.
//


#ifndef _SAVEABLEHH_
#define _SAVEABLEHH_


#include <stdio.h>
#include <string>
#include <iostream>
#include "oopobjectid.h"
#include "oopstorage.h"
#include "cmdefs.h"
class OOPDataVersion;

using namespace std;

/**
 * Base class for any class subjected to parallelization.
 * Implements basic functionalities for a parallel environment such as Packing and Unpacking.
 * Its virtual methods makes all derived classes compliant with the communication protocol
 * stablished by the OOPar.
 */
class OOPSaveable
{
public:

  /**
   * Simple Constructor 
   */
  OOPSaveable()  {}

  /**
   * Simple Desctructor 
   */
  virtual ~OOPSaveable() {}

  /**
   * Returns a Class ID identifier
   */
  virtual long GetClassID() = 0;

  /**
   * Packs the object in on the buffer so it can be transmitted through the network.
   * The Pack function  packs the object's class_id while function Unpack() doesn't,
   * allowing the user to identify the next object to be unpacked.
   * @param *buff A pointer to TSendStorage class to be packed.
   */
  virtual int Pack(OOPSendStorage * buf);
  /**
   * Unpacks the object class_id
   * @param *buff A pointer to TSendStorage class to be unpacked.
   */
  virtual int Unpack( OOPReceiveStorage *buf );

  /**
   * Sets an ID to the class
   * @param id to be set
   */
  void SetGlobalId(const OOPObjectId &id);

  /**
   * Returns the class id of the object within the DM
   */
  OOPObjectId GlobalId();
  



protected:
  /**
   * ID of the Data Manager
   */
  OOPObjectId fDMId;

};
#endif //_SAVEABLEHH_
