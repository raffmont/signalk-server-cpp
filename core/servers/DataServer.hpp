//
// Created by Raffaele Montella on 26/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATASERVER_HPP
#define SIGNALK_SERVER_CPP_DATASERVER_HPP


#include <thread>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "../model/SignalKModel.h"


class DataServer {
public:
    DataServer();
    DataServer(SignalK::SignalKModel *pSignalKModel);
    DataServer(SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~DataServer();
    void start();
    void join();
    virtual void run();


protected:
    std::thread t;
    bool threadStop = false;
    SignalK::SignalKModel *pSignalKModel=NULL;

};


#endif //SIGNALK_SERVER_CPP_DATASERVER_HPP
