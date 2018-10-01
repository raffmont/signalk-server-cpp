//
// Created by Raffaele Montella on 30/09/2018.
//

#include <fstream>
#include "SerialNMEA0183DataProvider.hpp"

SerialNMEA0183DataProvider::SerialNMEA0183DataProvider(std::string id,SignalK::SignalKModel *pSignalKModel, std::string device, unsigned long baud, serial::bytesize_t byteSize, std::string parity, serial::stopbits_t stopbits) {
    this->type="providers/nmea0183/serial";
    this->id=id;
    this->pSignalKModel=pSignalKModel;
    this->device=device;
    this->baud=baud;
    this->byteSize=byteSize;
    this->parity=parity;
    this->stopBits=stopBits;
}

SerialNMEA0183DataProvider::SerialNMEA0183DataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options): SerialNMEA0183DataProvider(id,document,options["device"],options["baud"],options["bytesize"],options["parity"],options["stopbits"]) {}

void SerialNMEA0183DataProvider::run(){
    spdlog::get("console")->info("SerialNMEA0183DataProvider: {0} {1} {2} {3} {4}",device,baud,byteSize,parity,stopBits);

    serial::parity_t parityCode=serial::parity_t::parity_none;
    if (parity=="odd") { parityCode=serial::parity_t::parity_odd; }
    else if (parity=="even") { parityCode=serial::parity_t::parity_even; }
    else if (parity=="mark") { parityCode=serial::parity_t::parity_mark; }
    else if (parity=="space") { parityCode=serial::parity_t::parity_space; }

    // port, baudrate, timeout in milliseconds
    pSerial=new serial::Serial(device,baud,serial::Timeout::simpleTimeout(1000),byteSize,parityCode,stopBits);

    if (pSerial!=nullptr && pSerial->isOpen()) {

        std::string line;
        while (!threadStop) {
            if (pSerial->waitReadable()) {
                line = pSerial->readline();
                parse(id, line);
            }
        }

        pSerial->close();
    }
}

SerialNMEA0183DataProvider::~SerialNMEA0183DataProvider() {

}
