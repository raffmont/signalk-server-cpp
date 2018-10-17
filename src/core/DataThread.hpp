//
// Created by Raffaele Montella on 14/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATATHREAD_HPP
#define SIGNALK_SERVER_CPP_DATATHREAD_HPP

#include <string>
#include <thread>
#include <spdlog/spdlog.h>
#include "model/SignalKModel.h"

class DataThread {
public:

    enum class Status : int {
        READY = 0,
        RUNNING = 1,
        STOPPED = 2
    };

    DataThread();

    void start();
    void stop();
    void join();
    void run();
    std::string getType() { return type; }
    std::string getId() { return id; }

    inline bool isEnabled() { return enabled; }
    inline Status getStatus() { return status; }
    inline bool isDone() { return threadStop; }

    virtual void onStart();
    virtual void onRun();
    virtual void onStop();

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


#endif //SIGNALK_SERVER_CPP_DATATHREAD_HPP
