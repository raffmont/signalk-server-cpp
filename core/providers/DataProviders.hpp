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



class DataProviders {
public:
    DataProviders();
    DataProviders(SignalK::SignalKModel *pSignalKModel,nlohmann::json settings);
    virtual ~DataProviders();
    virtual void run();

private:
    SignalK::SignalKModel *pSignalKModel=NULL;
    std::vector<DataProvider *> *pItems=NULL;
};


#endif //SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP
