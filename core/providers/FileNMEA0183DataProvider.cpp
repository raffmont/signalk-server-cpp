//
// Created by Raffaele Montella on 11/09/2018.
//

#include "FileNMEA0183DataProvider.hpp"

FileNMEA0183DataProvider::FileNMEA0183DataProvider(std::string name,SignalK::DataBase *document, std::string filePath, int millis) {
    std::cout << "FileNMEA0183DataProvider::FileNMEA0183DataProvider(" << document->getSelf() << "," << filePath <<"," << millis << ")\n";
    this->name=name;
    this->document=document;
    this->filePath=filePath;
    this->millis=millis;
}

void FileNMEA0183DataProvider::run(){
    std::cout << "FileNMEA0183DataProvider::run()\n";
    std::ifstream nmeaFile(filePath);
    if(nmeaFile) {

        std::string line;
        while (std::getline(nmeaFile, line)) {
            //std::cout <<  "FileNMEA0183DataProvider::run:" << line << "\n";
            parse(line);
            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }
        std::cout <<  "FileNMEA0183DataProvider::run:EOF\n";
    }
}

FileNMEA0183DataProvider::~FileNMEA0183DataProvider() {
    std::cout << "FileNMEA0183DataProvider::~FileNMEA0183DataProvider()\n";
}