//
// Created by Raffaele Montella on 22/09/2018.
//

#include "DataProviders.hpp"


DataProviders::DataProviders() {

}

DataProviders::DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json settings) {
    this->pSignalKModel=pSignalKModel;
    this->settings=settings;
}

DataProviders::~DataProviders() {
/*
    for (DataProvider *pDataProvider :  *pItems) {
        if (pDataProvider != nullptr) {
            delete(pDataProvider);
        }
    }
    delete(pItems);
    pItems=nullptr;
    */
}

void  DataProviders::run() {
    while(!threadStop) {

        for (nlohmann::json provider:settings["providers"]) {
            DataProvider *pDataProvider=nullptr;

            std::string id=provider["id"];
            std::string type=provider["type"];
            bool enabled=provider["enabled"];
            nlohmann::json options=provider["options"];

            spdlog::get("console")->info("Data Provider: {0} {1} {2}",id,type,enabled);

            pDataProvider=(DataProvider *)providers[id];
            if (pDataProvider!= nullptr) {

                if (!enabled) {
                    pDataProvider->stop();
                    while(pDataProvider->getStatus()!=DataProvider::Status::STOPPED);
                }

                if (pDataProvider->getStatus()==DataProvider::Status::STOPPED) {
                    providers.erase(id);
                    delete pDataProvider;
                    pDataProvider= nullptr;
                }
            }
            if (pDataProvider== nullptr) {
                if (enabled) {
                    if (type == "providers/nmea0183/serial") {
                        pDataProvider = new SerialNMEA0183DataProvider(enabled, id, pSignalKModel, options);
                    } else if (type == "providers/nmea0183/filestream") {
                        pDataProvider = new FileNMEA0183DataProvider(enabled,id, pSignalKModel, options);
                    } else if (type == "providers/signalk/websocket/client") {
                        pDataProvider = new WebSocketClientDataProvider(enabled,id, pSignalKModel, options);
                    }
                    if (pDataProvider) {
                        providers.insert(std::pair<std::string, DataProvider * > (id, pDataProvider));
                        pDataProvider->start();
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }
}

