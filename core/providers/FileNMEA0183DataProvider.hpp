//
// Created by Raffaele Montella on 11/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_FILENMEA0183DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_FILENMEA0183DATAPROVIDER_HPP


#include "NMEA0183DataProvider.hpp"
#include <fstream>
#include <sstream>
#include <string>

class FileNMEA0183DataProvider: public NMEA0183DataProvider  {
public:
    virtual void run();
    FileNMEA0183DataProvider(std::string id, SignalK::SignalKModel *document, std::string filePath, int millis);
    virtual ~FileNMEA0183DataProvider();

private:
    std::string filePath;
    long millis=1000;
};


#endif //SIGNALK_SERVER_CPP_FILENMEA0183DATAPROVIDER_HPP
