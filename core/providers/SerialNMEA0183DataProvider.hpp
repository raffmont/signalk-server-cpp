//
// Created by Raffaele Montella on 30/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_SERIALNMEA0183DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_SERIALNMEA0183DATAPROVIDER_HPP

#include <serial/serial.h>

#include "NMEA0183DataProvider.hpp"

class SerialNMEA0183DataProvider : public NMEA0183DataProvider {
public:
    virtual void run();
    SerialNMEA0183DataProvider(std::string id, SignalK::SignalKModel *document, std::string device, unsigned long baud, serial::bytesize_t byteSize);
    SerialNMEA0183DataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options);
    virtual ~SerialNMEA0183DataProvider();

private:
    serial::Serial *pSerial;
    std::string device;
    unsigned long baud=4800;
    serial::bytesize_t byteSize=serial::bytesize_t::eightbits;
};


#endif //SIGNALK_SERVER_CPP_SERIALNMEA0183DATAPROVIDER_HPP
