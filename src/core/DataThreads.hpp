//
// Created by Raffaele Montella on 15/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATATHREADS_HPP
#define SIGNALK_SERVER_CPP_DATATHREADS_HPP


#include "model/SignalKModel.h"
#include "DataThread.hpp"

class DataThreads {
public:
    DataThreads();
    DataThreads(SignalK::SignalKModel *pSignalKModel,nlohmann::json *pSettings);
    virtual ~DataThreads();
    void heartBeat();

    void setSignalKModel(SignalK::SignalKModel *pSignalKModel) { this->pSignalKModel=pSignalKModel; }
    SignalK::SignalKModel *getSignalKModel() { return pSignalKModel; }

    void setSettings(nlohmann::json *pSettings) { this->pSettings=pSettings; }

    virtual DataThread *onCreate(bool enabled, std::string id, std::string type, nlohmann::json options);

protected:
    std::string key;

private:
    nlohmann::json *pSettings;
    SignalK::SignalKModel *pSignalKModel;
    std::map<std::string, DataThread *> *pDataThreads;
};


#endif //SIGNALK_SERVER_CPP_DATATHREADS_HPP
