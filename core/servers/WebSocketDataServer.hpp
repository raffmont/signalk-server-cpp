//
// Created by Raffaele Montella on 06/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_WEBSOCKETDATASERVER_HPP
#define SIGNALK_SERVER_CPP_WEBSOCKETDATASERVER_HPP


#include <uWS/Hub.h>
#include "../model/SignalKModel.h"
#include "SignalKDataServer.hpp"
#include <string>
#include <fstream>
#include <streambuf>

class WebSocketDataServer : public SignalKDataServer {
public:
    virtual void run();

    WebSocketDataServer(SignalK::SignalKModel *pSignalKModel, std::string bind, int port, std::string root);
    WebSocketDataServer(SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~WebSocketDataServer();

    const std::string SIGNALK="/signalk";
    const std::string SIGNALK_V1_STREAM="/signalk/v1/stream";
    const std::string SIGNALK_V1_API="/signalk/v1/api/";

private:
    uWS::Hub h;
    std::string root="www";
    std::string bind="localhost";
    int port=3000;

    bool hasEnding (std::string const &fullString, std::string const &ending);


};


#endif //SIGNALK_SERVER_CPP_WEBSOCKETDATASERVER_HPP
