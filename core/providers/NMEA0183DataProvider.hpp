//
// Created by Raffaele Montella on 11/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP



#include "DataProvider.hpp"

#include <marnav/nmea/angle.hpp>
#include <marnav/nmea/nmea.hpp>
#include <marnav/nmea/zda.hpp>
#include <marnav/nmea/gll.hpp>
#include <marnav/utils/unique.hpp>

class NMEA0183DataProvider: public DataProvider {
public:
    void parse(std::string line);

private:
    bool replace(std::string& str, const std::string& from, const std::string& to);
};


#endif //SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP
