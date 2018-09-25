//
// Created by Raffaele Montella on 11/09/2018.
//

#include "FileNMEA0183DataProvider.hpp"

FileNMEA0183DataProvider::FileNMEA0183DataProvider(std::string id,SignalK::SignalKModel *document, std::string filePath, int millis) {
    this->type="providers/nmea0183/filestream";
    this->id=id;
    this->document=document;
    this->filePath=filePath;
    this->millis=millis;
}

FileNMEA0183DataProvider::FileNMEA0183DataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options): FileNMEA0183DataProvider(id,document,options["path"],options["millis"]) {}

void FileNMEA0183DataProvider::run(){

    std::ifstream nmeaFile(filePath);
    if(nmeaFile) {

        std::string line;
        while (std::getline(nmeaFile, line)) {
            //std::cout <<  "FileNMEA0183DataProvider::run:" << line << "\n";
            parse(line);
            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }

    }
}

FileNMEA0183DataProvider::~FileNMEA0183DataProvider() {

}