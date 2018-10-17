//
// Created by Raffaele Montella on 22/09/2018.
//

#include "DataProviders.hpp"




DataProviders::DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json *pSettings):DataThreads(pSignalKModel,pSettings) {
    key="providers";
}

DataProviders::~DataProviders() {
}

DataThread *DataProviders::onCreate(bool enabled, std::string id, std::string type, nlohmann::json options) {
    DataThread *dataThread= nullptr;

    if (type == "providers/nmea0183/serial") {
        dataThread = new SerialNMEA0183DataProvider(enabled, id, getSignalKModel(), options);
    } else if (type == "providers/nmea0183/filestream") {
        dataThread = new FileNMEA0183DataProvider(enabled,id, getSignalKModel(), options);
    } else if (type == "providers/signalk/websocket/client") {
        dataThread = new WebSocketClientDataProvider(enabled,id, getSignalKModel(), options);
    }

    return dataThread;
}

