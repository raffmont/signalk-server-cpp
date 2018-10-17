//
// Created by Raffaele Montella on 09/10/2018.
//

#include "SignalKServer.hpp"
#include "core/model/SignalKModel.h"
#include "core/providers/DataProviders.hpp"
#include "core/servers/DataServers.hpp"



SignalKServer::SignalKServer(nlohmann::json settings) {
    this->settings=settings;
    std::string uuid=settings["vessel"]["uuid"];
    pSignalKModel=new SignalK::SignalKModel(uuid, "v0.1.0");
}

SignalKServer::~SignalKServer() {
    if (pSignalKModel) {
        delete pSignalKModel;
        pSignalKModel = nullptr;
    }
}

void SignalKServer::run() {

    DataServers servers(pSignalKModel,&settings);
    DataProviders providers(pSignalKModel,&settings);

    while(!threadStop) {

        providers.heartBeat();
        servers.heartBeat();

        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }
}
