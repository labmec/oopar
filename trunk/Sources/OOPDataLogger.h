/* Generated by Together */

#ifndef OOPDATALOGGER_H
#define OOPDATALOGGER_H
#include "oopsaveable.h"
class OOPDataLogger : public OOPSaveable {
public:    

    /**
     * Simple constructor 
     */
    OOPDataLogger();

    /**
     * Constructor with ofstream object
     * @param out output object 
     */
    OOPDataLogger(ostream & out);

    /**
     * Constructor which takes the filename parameter
     * @param outfilename Logger will be generated on that file 
     */
    OOPDataLogger(string &logfilename);    

    void SendOwnTask(OOPDMOwnerTask *town);
    void ReceiveOwnTask(OOPDMOwnerTask *town);

    void CancelTask(OOPMetaData &data);

private:    
    ostream * fLogger;
};
#endif //OOPDATALOGGER_H
