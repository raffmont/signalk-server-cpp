//
// Created by Raffaele Montella on 09/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_SIGNALKSERVER_HPP
#define SIGNALK_SERVER_CPP_SIGNALKSERVER_HPP


#include <nlohmann/json.hpp>
#include "core/servers/DataServer.hpp"

class SignalKServer {
public:
    SignalKServer(nlohmann::json settings);
    void run();
    ~SignalKServer();

private:
    nlohmann::json settings;
    bool threadStop = false;
    long millis=5000;
    SignalK::SignalKModel *pSignalKModel= nullptr;
};


#endif //SIGNALK_SERVER_CPP_SIGNALKSERVER_HPP
