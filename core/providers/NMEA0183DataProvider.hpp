//
// Created by Raffaele Montella on 11/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP


#include "DataProvider.hpp"

class NMEA0183DataProvider: public DataProvider {
public:
    void parse(std::string line);
};


#endif //SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP
