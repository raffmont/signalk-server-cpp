//
// Created by Raffaele Montella on 26/09/2018.
//

#include "SignalKDataServer.hpp"




SignalKDataServer::SignalKDataServer():DataServer() {
    pSubscriptions=new SignalK::Subscriptions(pSignalKModel);

}

SignalKDataServer::SignalKDataServer(bool enabled, std::string id, SignalK::SignalKModel *pSignalKModel) : DataServer(
        enabled, id, pSignalKModel) {

    pSubscriptions=new SignalK::Subscriptions(pSignalKModel);

}

SignalKDataServer::SignalKDataServer(bool enabled, std::string id, SignalK::SignalKModel *pSignalKModel,
                                     nlohmann::json options) : DataServer(enabled, id, pSignalKModel, options) {

}

SignalKDataServer::~SignalKDataServer() {
    if (pSubscriptions) {
        delete pSubscriptions;
        pSubscriptions= nullptr;
    }

}

void SignalKDataServer::parse(nlohmann::json jMessage) {

}






