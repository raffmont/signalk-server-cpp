//
// Created by Raffaele Montella on 31/08/2018.
//



#include "DataProvider.hpp"
#include <thread>


DataProvider::DataProvider() : t() {

}

DataProvider::DataProvider(std::string id,SignalK::SignalKModel *pSignalKModel) {
    this->id=id;
    this->pSignalKModel=pSignalKModel;

}

DataProvider::DataProvider(std::string id,SignalK::SignalKModel *document, nlohmann::json options) {

}

DataProvider::~DataProvider() {

}

void DataProvider::start() {
    t = std::thread(&DataProvider::run, this);
}

void DataProvider::join() {
    t.join();
}

void DataProvider::run(){
}






