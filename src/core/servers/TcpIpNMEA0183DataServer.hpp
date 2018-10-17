//
// Created by Raffaele Montella on 14/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_TCPIPNMEA0183DATASERVER_HPP
#define SIGNALK_SERVER_CPP_TCPIPNMEA0183DATASERVER_HPP


#include "NMEA0183DataServer.hpp"

class TcpIpNMEA0183DataServer: public NMEA0183DataServer  {
public:
    TcpIpNMEA0183DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, std::string bind, int port);
    TcpIpNMEA0183DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~TcpIpNMEA0183DataServer();

    virtual void onRun();

private:
    long millis=1000;
    std::string bind="localhost";
    int port=10110;


};


#endif //SIGNALK_SERVER_CPP_TCPIPNMEA0183DATASERVER_HPP
