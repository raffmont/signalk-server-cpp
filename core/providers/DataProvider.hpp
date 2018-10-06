//
// Created by Raffaele Montella on 31/08/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_DATAPROVIDER_HPP


#include <string>
#include <thread>
#include <spdlog/spdlog.h>
#include "../model/SignalKModel.h"


class DataProvider{



public:

    enum class Status : int {
        READY = 0,
        RUNNING = 1,
        STOPPED = 2
    };

    DataProvider();
    DataProvider(bool enabled, std::string id,SignalK::SignalKModel *pSignalKModel);
    DataProvider(bool enabled, std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~DataProvider();
    void start();
    void stop();
    virtual void onStart();
    virtual void onRun();
    virtual void onStop();
    void join();
    std::string getType() { return type; }
    std::string getId() { return id; }
    void run();
    inline bool isEnabled() { return enabled; }

    inline Status getStatus() { return status; }
    inline bool isDone() { return threadStop; }

    //void setStatus(Status status) { this->status=status; }

protected:

    bool enabled=false;
    std::string id="";
    std::string type="providers";
    std::thread t;

    SignalK::SignalKModel *pSignalKModel= nullptr;

private:
    Status status=Status::READY;
    bool threadStop = false;
};

#endif //SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
