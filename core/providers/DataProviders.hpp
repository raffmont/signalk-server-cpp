//
// Created by Raffaele Montella on 22/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP
#define SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP

#include <thread>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "../model/SignalKModel.h"
#include "DataProvider.hpp"
#include "SerialNMEA0183DataProvider.hpp"
#include "FileNMEA0183DataProvider.hpp"
#include "WebSocketClientDataProvider.hpp"



class DataProviders {
public:
    DataProviders();
    DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json settings);
    virtual ~DataProviders();
    virtual void run();

private:
    nlohmann::json settings;
    bool threadStop = false;
    long millis=5000;
    SignalK::SignalKModel *pSignalKModel= nullptr;
    //std::vector<DataProvider *> *pItems=NULL;
    std::map<std::string, DataProvider *> providers;
};


#endif //SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP
