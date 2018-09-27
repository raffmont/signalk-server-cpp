//
// Created by Raffaele Montella on 22/09/2018.
//

#include "DataProviders.hpp"
#include "FileNMEA0183DataProvider.hpp"
#include "WebSocketClientDataProvider.hpp"

DataProviders::DataProviders() {

}

DataProviders::DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json settings) {




    pItems=new std::vector<DataProvider *>(std::thread::hardware_concurrency());
    for (nlohmann::json provider:settings["providers"]) {
        DataProvider *pDataProvider=NULL;

        std::string id=provider["id"];
        std::string type=provider["type"];
        nlohmann::json options=provider["options"];

        spdlog::get("console")->info("Data Provider: {0} {1}",id,type);
        if (type=="providers/nmea0183/filestream") {
            pDataProvider=new FileNMEA0183DataProvider(id,pSignalKModel,options);
        } else if (type=="providers/signalk/websocket/client") {
            pDataProvider=new WebSocketClientDataProvider(id,pSignalKModel,options);
        }
        if (pDataProvider!=NULL) {
            pItems->push_back(pDataProvider);
        }

    }
}

DataProviders::~DataProviders() {

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
            spdlog::get("console")->info("Starting: {0}",pDataProvider->getId());
            pDataProvider->start();
        }
    }

    for (DataProvider *pDataProvider :  *pItems){
        if (pDataProvider != NULL) {
            spdlog::get("console")->info("Joining: {0}",pDataProvider->getId());
            pDataProvider->join();
        }
    }
}

