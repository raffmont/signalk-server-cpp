//
// Created by Raffaele Montella on 31/08/2018.
//

#include <iostream>
#include "DataProvider.hpp"



DataProvider::DataProvider() : t() {
    std::cout << "DataProvider::DataProvider()\n";
}

DataProvider::DataProvider(std::string name,SignalK::DataBase *document) {
    this->name=name;
    this->document=document;
    std::cout << "DataProvider::DataProvider()\n";
}

DataProvider::~DataProvider() {
    std::cout << "DataProvider::~DataProvider()\n";
}

void DataProvider::start() {
    t = std::thread(&DataProvider::run, this);
}

void DataProvider::join() {
    t.join();
}

void DataProvider::run(){
}





