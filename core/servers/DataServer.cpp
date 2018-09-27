//
// Created by Raffaele Montella on 26/09/2018.
//

#include "DataServer.hpp"


DataServer::DataServer() : t() {

}

DataServer::DataServer(SignalK::SignalKModel *pSignalKModel) {
    this->pSignalKModel=pSignalKModel;

}

DataServer::DataServer(SignalK::SignalKModel *pSignalKModel, nlohmann::json options) {

}

DataServer::~DataServer() {

}

void DataServer::start() {
    t = std::thread(&DataServer::run, this);
}

void DataServer::join() {
    t.join();
}

void DataServer::run(){
}