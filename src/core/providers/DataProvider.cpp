//
// Created by Raffaele Montella on 31/08/2018.
//



#include "DataProvider.hpp"
#include <thread>


DataProvider::DataProvider() {
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








