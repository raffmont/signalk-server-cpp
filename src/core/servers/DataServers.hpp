//
// Created by Raffaele Montella on 14/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATASERVERS_HPP
#define SIGNALK_SERVER_CPP_DATASERVERS_HPP

#include <thread>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "../model/SignalKModel.h"
#include "../DataThreads.hpp"

#include "WebDataServer.hpp"
#include "TcpIpSignalKDataServer.hpp"
#include "TcpIpNMEA0183DataServer.hpp"


class DataServers: public DataThreads {
public:
    DataServers(SignalK::SignalKModel *pSignalKModel, nlohmann::json *Settings);

    virtual ~DataServers();

    virtual DataThread *onCreate(bool enabled, std::string id, std::string type, nlohmann::json options);

};




#endif //SIGNALK_SERVER_CPP_DATASERVERS_HPP
