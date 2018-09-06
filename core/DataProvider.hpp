//
// Created by Raffaele Montella on 31/08/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_DATAPROVIDER_HPP

#include <thread>
#include <uWS/Hub.h>
#include "DataBase.h"

class DataProvider{
public:
    DataProvider();
    DataProvider(SignalK::DataBase& document);
    virtual ~DataProvider();
    void start();
    void join();
    virtual void run();

protected:
    std::thread t;
    bool threadStop = false;
    SignalK::DataBase document;
};



/*
class NMEA0183DataProvider : public DataProvider {

};

class FileNMEA0183DataProvider : public NMEA0183DataProvider {

};

class SerialNMEA0183DataProvider : public NMEA0183DataProvider {

};

class TcpIpNMEA0183DataProvider : public NMEA0183DataProvider {

};

class UdpNMEA0183DataProvider : public NMEA0183DataProvider {

};
*/
#endif //SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
