#include "oopsaveable.h"
#include "oopsaveable.h"
#include "oopobjectid.h"

class OOPStorage: public OOPSaveable

{


// redefinicao de todos os metodos da classe TSaveable


virtual long SetGlobalId(long id);
virtual long ClassId(); // a unique id for each class
virtual char *ClassName(); // returns the name of the
virtual OOPObjectId GlobalId(); // returns the class id of the 
                         // object within the DM
virtual long SizeToStore(); //amount of storage needed  to 
                            //  store the object
virtual int Store(OOPStorage &st, long &loc); // store the data 
                                           // of the object at location loc

// dados:
     long fSize;
     char * fStore;

// metodos
   OOPStorage(long size); // intialize an object with the give size
   int Resize (long newsize); // resets the size of the object
   long Size(); // returns the size of the object
   int get(long &loc, short &val); // retrieves the value val
                                   // at location loc
                                   // returns: 0 if success full
                                   //          1 if failed

   int get(float &loc, short &val); // retrieves the value val
                                    // at location loc
                                    // returns: 0 if success full
                                    //          1 if failed

   int get(double &loc, short &val); // retrieves the value val
                                     // at location loc
                                     // returns: 0 if success full
                                     //          1 if failed
              
   int get(long &loc,long size, void *dat); // copy "size" bytes 
                                		// to the memory pointed to by dat
   int put(long &loc, short &dat); // store dat
   int put(float &loc, short &dat); // store dat     
   int put(double &loc, short &dat); // store dat
   long DataId(); // returns the Id of the object stored 

   long DataVersion(); // returns the version of the
                       //   object stored
};
