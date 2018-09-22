//
// Created by Raffaele Montella on 22/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP
#define SIGNALK_SERVER_CPP_DATAPROVIDERS_HPP


#include <cstdio>
#include "../SignalKModel.h"
#include "WebSocketServerDataProvider.hpp"
#include "WebSocketClientDataProvider.hpp"
#include "FileNMEA0183DataProvider.hpp"
#include "WebAPIDataProvider.hpp"
#include "DataProviders.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <ctime>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

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
