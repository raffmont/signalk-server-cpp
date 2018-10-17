//
// Created by Raffaele Montella on 07/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_TCPIPSIGNALKDATASERVER_HPP
#define SIGNALK_SERVER_CPP_TCPIPSIGNALKDATASERVER_HPP


#include "SignalKDataServer.hpp"
#include <uvw.hpp>
#include <cassert>
#include <iostream>
#include <memory>
#include <chrono>

class TcpIpSignalKDataServer : public SignalKDataServer {
public:


    TcpIpSignalKDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, std::string bind, int port);
    TcpIpSignalKDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~TcpIpSignalKDataServer();

    virtual void onRun();

private:
    long millis=1000;
    std::string bind="localhost";
    int port=55555;


};


#endif //SIGNALK_SERVER_CPP_TCPIPSIGNALKDATASERVER_HPP
