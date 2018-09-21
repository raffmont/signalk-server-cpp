//
// Created by Raffaele Montella on 31/08/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_DATAPROVIDER_HPP

#include <iostream>
#include <thread>
#include <uWS/Hub.h>
#include <nlohmann/json.hpp>


#include "../SignalKModel.h"


class DataProvider{
public:
    DataProvider();
    DataProvider(std::string id,SignalK::SignalKModel *document);
    virtual ~DataProvider();
    void start();
    void join();
    std::string getType() { return type; }
    std::string getId() { return id; }
    virtual void run();

protected:
    std::string id="";
    std::string type="providers";
    std::thread t;
    bool threadStop = false;
    SignalK::SignalKModel *document=NULL;
};



/*


class SerialNMEA0183DataProvider : public NMEA0183DataProvider {

};

class TcpIpNMEA0183DataProvider : public NMEA0183DataProvider {

};

class UdpNMEA0183DataProvider : public NMEA0183DataProvider {

};
*/
#endif //SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
