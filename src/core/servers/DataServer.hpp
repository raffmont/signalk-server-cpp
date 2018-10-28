//
// Created by Raffaele Montella on 26/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATASERVER_HPP
#define SIGNALK_SERVER_CPP_DATASERVER_HPP


#include <thread>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "../model/SignalKModel.h"
#include "../DataThread.hpp"


class DataServer : public DataThread {
public:

    DataServer();
    DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel);
    DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~DataServer();



};


#endif //SIGNALK_SERVER_CPP_DATASERVER_HPP
