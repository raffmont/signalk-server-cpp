//
// Created by Raffaele Montella on 31/08/2018.
//


#include "DataProvider.hpp"



DataProvider::DataProvider() : t() {

}

DataProvider::DataProvider(std::string id,SignalK::SignalKModel *document) {
    this->id=id;
    this->document=document;

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





