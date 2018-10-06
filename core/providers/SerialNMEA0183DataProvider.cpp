//
// Created by Raffaele Montella on 30/09/2018.
//

#include <fstream>
#include "SerialNMEA0183DataProvider.hpp"

SerialNMEA0183DataProvider::SerialNMEA0183DataProvider(bool enabled, std::string id,SignalK::SignalKModel *pSignalKModel, std::string device, unsigned long baud, serial::bytesize_t byteSize, std::string parity, serial::stopbits_t stopbits) {
    this->type="providers/nmea0183/serial";
    this->enabled=enabled;
    this->id=id;
    this->pSignalKModel=pSignalKModel;
    this->device=device;
    this->baud=baud;
    this->byteSize=byteSize;
    this->parity=parity;
    this->stopBits=stopBits;
}

SerialNMEA0183DataProvider::SerialNMEA0183DataProvider(bool enabled, std::string id, SignalK::SignalKModel *pSignalKModel, nlohmann::json options): SerialNMEA0183DataProvider(enabled, id,pSignalKModel,options["device"],options["baud"],options["bytesize"],options["parity"],options["stopbits"]) {}

void SerialNMEA0183DataProvider::onRun(){
    pSerial= nullptr;

    spdlog::get("console")->info("SerialNMEA0183DataProvider: {0} {1} {2} {3} {4}",device,baud,byteSize,parity,stopBits);

    serial::parity_t parityCode=serial::parity_t::parity_none;
    if (parity=="odd") { parityCode=serial::parity_t::parity_odd; }
    else if (parity=="even") { parityCode=serial::parity_t::parity_even; }
    else if (parity=="mark") { parityCode=serial::parity_t::parity_mark; }
    else if (parity=="space") { parityCode=serial::parity_t::parity_space; }

    // port, baudrate, timeout in milliseconds
    try {
        pSerial=new serial::Serial(device,baud,serial::Timeout::simpleTimeout(1000),byteSize,parityCode,stopBits);
    } catch (serial::IOException exception) {
        spdlog::get("console")->error("SerialNMEA0183DataProvider: {0}",exception.what());
        delete pSerial;
        pSerial= nullptr;
    }

    if (pSerial!=nullptr && pSerial->isOpen()) {

        std::string line;
        while (!isDone()) {
            try {
                if (pSerial->waitReadable()) {
                    line = pSerial->readline();
                    parse(id, line);
                }
            } catch (serial::SerialException exception) {
                spdlog::get("console")->error("SerialNMEA0183DataProvider: {0}",exception.what());
                stop();
            }
        }
        if (pSerial->isOpen()) {
            pSerial->close();
            delete pSerial;
            pSerial= nullptr;
        }
    }
}

SerialNMEA0183DataProvider::~SerialNMEA0183DataProvider() {

}
