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
#include "../DataThreads.hpp"


class DataProviders: public DataThreads {
public:
    DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json *pSettings);
    virtual ~DataProviders();

    virtual DataThread *onCreate(bool enabled, std::string id, std::string type, nlohmann::json options);
};


#endif //SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP
