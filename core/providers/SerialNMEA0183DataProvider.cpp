//
// Created by Raffaele Montella on 30/09/2018.
//

#include <fstream>
#include "SerialNMEA0183DataProvider.hpp"

SerialNMEA0183DataProvider::SerialNMEA0183DataProvider(std::string id,SignalK::SignalKModel *pSignalKModel, std::string device, unsigned long baud, serial::bytesize_t byteSize) {
    this->type="providers/nmea0183/filestream";
    this->id=id;
    this->pSignalKModel=pSignalKModel;
    this->device=device;
    this->baud=baud;
}

SerialNMEA0183DataProvider::SerialNMEA0183DataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options): SerialNMEA0183DataProvider(id,document,options["device"],options["baud"],options["bytesize"]) {}

void SerialNMEA0183DataProvider::run(){

    // port, baudrate, timeout in milliseconds
    pSerial=new serial::Serial(device,baud,serial::Timeout::simpleTimeout(1000),byteSize);

    pSerial->open();

    if(pSerial->isOpen()) {

        std::string line;
        while (!threadStop) {
            if (pSerial->waitReadable()) {
                line=pSerial->readline();
                parse(line);
            }
        }

        pSerial->close();
    }
}

SerialNMEA0183DataProvider::~SerialNMEA0183DataProvider() {

}
