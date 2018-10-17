//
// Created by Raffaele Montella on 26/09/2018.
//

#include "DataServer.hpp"


DataServer::DataServer() {
}

DataServer::DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel) {
    this->enabled=enabled;
    this->id=id;
    this->pSignalKModel=pSignalKModel;
}

DataServer::DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options) {
}

DataServer::~DataServer() {
}
