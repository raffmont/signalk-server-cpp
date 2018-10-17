//
// Created by Raffaele Montella on 11/09/2018.
//

#include "FileNMEA0183DataProvider.hpp"

FileNMEA0183DataProvider::FileNMEA0183DataProvider(bool enabled, std::string id,SignalK::SignalKModel *pSignalKModel, std::string filePath, int millis) {
    this->enabled=enabled;
    this->type="providers/nmea0183/filestream";
    this->id=id;
    this->pSignalKModel=pSignalKModel;
    this->filePath=filePath;
    this->millis=millis;
}

FileNMEA0183DataProvider::FileNMEA0183DataProvider(bool enabled, std::string id, SignalK::SignalKModel *document, nlohmann::json options): FileNMEA0183DataProvider(enabled, id,document,options["path"],options["millis"]) {}

void FileNMEA0183DataProvider::onRun(){
    spdlog::get("console")->info("FileNMEA0183DataProvider: {0}",filePath);
    std::ifstream nmeaFile(filePath);
    if(nmeaFile) {

        std::string line;
        while (!isDone() && std::getline(nmeaFile, line)) {
            parse(id,line);
            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }
    }
}

FileNMEA0183DataProvider::~FileNMEA0183DataProvider() {

}