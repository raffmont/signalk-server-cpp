//
// Created by Raffaele Montella on 31/08/2018.
//



#include "DataProvider.hpp"
#include <thread>


DataProvider::DataProvider() : t() {

}

DataProvider::DataProvider(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel) {
    this->enabled=enabled;
    this->id=id;
    this->pSignalKModel=pSignalKModel;

}

DataProvider::DataProvider(bool enabled,std::string id,SignalK::SignalKModel *document, nlohmann::json options) {

}

DataProvider::~DataProvider() {

}

void DataProvider::start() {
    if (status==DataProvider::Status::READY) {
        t = std::thread(&DataProvider::run, this);
        threadStop = false;
    }
}

void DataProvider::stop() {
    threadStop=true;
}

void DataProvider::join() {
    t.join();
}

void DataProvider::onStart() {}
void DataProvider::onRun() {}
void DataProvider::onStop() {}

void DataProvider::run(){
    status=Status::RUNNING;
    onStart();
    onRun();
    onStop();
    status=Status::STOPPED;
}






