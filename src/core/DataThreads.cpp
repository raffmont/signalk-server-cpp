//
// Created by Raffaele Montella on 15/10/2018.
//

#include "DataThreads.hpp"


DataThreads::DataThreads() {
    pSettings=NULL;
    pSignalKModel=NULL;
    pDataThreads=new std::map<std::string, DataThread *>();
}

DataThreads::DataThreads(SignalK::SignalKModel *pSignalKModel,nlohmann::json *pSettings): DataThreads() {
    this->pSignalKModel=pSignalKModel;
    this->pSettings=pSettings;
}

DataThreads::~DataThreads() {
    if (pDataThreads) {
        delete pDataThreads;
        pDataThreads= NULL;
    }
}

DataThread* DataThreads::onCreate(bool enabled, std::string id, std::string type, nlohmann::json options) {
    return nullptr;
}

void  DataThreads::heartBeat() {
    spdlog::get("console")->debug("DataThreads::heartBeat BEGIN {0}",key);

    for (nlohmann::json provider:pSettings->at(key)) {


        std::string id=provider["id"];
        std::string type=provider["type"];
        bool enabled=provider["enabled"];
        nlohmann::json options=provider["options"];

        spdlog::get("console")->debug("DataThreads::heartBeat: id:{0} type:{1} enabled:{2}",id,type,enabled);


        try {
            spdlog::get("console")->debug("DataThreads::heartBeat: Size:{0}",pDataThreads->size());
            auto *pDataThread = pDataThreads->at(id);

            spdlog::get("console")->debug("DataThreads::heartBeat: {0} is instanced",id);
            if (!enabled && pDataThread->getStatus()==DataThread::Status::RUNNING) {
                spdlog::get("console")->debug("DataThreads::heartBeat: {0} must be stopped.");
                pDataThread->stop();
                while(pDataThread->getStatus()!=DataThread::Status::STOPPED);
                spdlog::get("console")->debug("DataThreads::heartBeat: {0} has been stopped.",id);
            }

            if (pDataThread->getStatus()==DataThread::Status::STOPPED) {
                pDataThreads->erase(id);
                delete pDataThread;
                spdlog::get("console")->debug("DataThreads::heartBeat: {0} has been destroyed because stopped.",id);
            }
        } catch (std::out_of_range &ex) {

            spdlog::get("console")->debug("DataThreads::heartBeat: {0} is NOT instanced",id);
            if (enabled) {
                spdlog::get("console")->debug("DataThreads::heartBeat: {0} must be instanced.",id);
                DataThread *pDataThread=onCreate(enabled,id,type,options);
                if (pDataThread) {
                    pDataThreads->insert(std::pair<std::string, DataThread * > (id, pDataThread));
                    pDataThread->start();
                    spdlog::get("console")->debug("DataThreads::heartBeat: {0} has been started. Size: {1}",id,pDataThreads->size());
                }
            }
        }
    }
    spdlog::get("console")->debug("DataThreads::heartBeat END {0}",key);

}