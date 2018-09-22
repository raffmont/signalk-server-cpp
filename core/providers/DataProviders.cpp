//
// Created by Raffaele Montella on 22/09/2018.
//

#include "DataProviders.hpp"


DataProviders::DataProviders() {
    pItems=new std::vector<DataProvider *>(std::thread::hardware_concurrency());
}

DataProviders::DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json settings) {
    for (nlohmann::json provider:settings["providers"]) {
        DataProvider *pDataProvider=NULL;
        if (provider["type"]=="providers/nmea0183/filestream") {
            pDataProvider=new FileNMEA0183DataProvider(provider["id"],pSignalKModel,provider["options"]);
        } else if (provider["type"]=="providers/signalk/webapi/server") {
            pDataProvider=new WebAPIDataProvider(provider["id"],pSignalKModel,provider["options"]);
        } else if (provider["type"]=="providers/signalk/websocket/client") {
            pDataProvider=new WebSocketClientDataProvider(provider["id"],pSignalKModel,provider["options"]);
        }  else if (provider["type"]=="providers/signalk/websocket/server") {
            pDataProvider=new WebSocketServerDataProvider(provider["id"],pSignalKModel,provider["options"]);
        }
        if (pDataProvider!=NULL) {
            pItems->push_back(pDataProvider);
        }
    }
}

DataProviders::~DataProviders() {
    std::cout << "DataProvider::~DataProvider()\n";

    for (DataProvider *pDataProvider :  *pItems) {
        if (pDataProvider != NULL) {
            delete(pDataProvider);
        }
    }
    delete(pItems);
    pItems=NULL;
}

void  DataProviders::run() {
    for (DataProvider *pDataProvider :  *pItems) {
        if (pDataProvider != NULL) {
            std::cout << "Starting: " << pDataProvider->getId() << "\n";
            pDataProvider->start();
        }
    }

    for (DataProvider *pDataProvider :  *pItems){
        if (pDataProvider != NULL) {
            std::cout << "Joining: " << pDataProvider->getId() << "\n";
            pDataProvider->join();
        }
    }
}

