//
// Created by Raffaele Montella on 11/09/2018.
//

#include "NMEA0183DataProvider.hpp"

void NMEA0183DataProvider::parse(std::string line) {

    if (document!=NULL) {
        std::cout <<  "NMEA0183DataProvider::parse:" << line << "\n";
    }
};