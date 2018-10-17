//
// Created by Raffaele Montella on 14/10/2018.
//

#include "DataServers.hpp"

DataServers::DataServers(SignalK::SignalKModel *pSignalKModel,nlohmann::json *pSettings):DataThreads(pSignalKModel,pSettings) {
    key="servers";
}

DataServers::~DataServers() {
}

DataThread *DataServers::onCreate(bool enabled, std::string id, std::string type, nlohmann::json options) {
    DataThread *dataThread= nullptr;

    if (type == "servers/signalk/web") {
        dataThread = new WebDataServer(enabled, id, getSignalKModel(), options);
    } else if (type == "servers/signalk/tcpip") {
        dataThread = new TcpIpSignalKDataServer(enabled,id, getSignalKModel(), options);
    } else if (type == "servers/nmea0183/tcpip") {
        dataThread = new TcpIpNMEA0183DataServer(enabled,id, getSignalKModel(), options);
    }

    return dataThread;
}