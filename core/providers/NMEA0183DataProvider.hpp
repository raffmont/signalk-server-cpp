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
#include <marnav/nmea/gga.hpp>
#include <marnav/nmea/rmc.hpp>
#include <marnav/nmea/mwd.hpp>
#include <marnav/nmea/vtg.hpp>
#include <marnav/nmea/dbt.hpp>
#include <marnav/nmea/hdg.hpp>
#include <marnav/nmea/hdm.hpp>
#include <marnav/nmea/hdt.hpp>
#include <marnav/nmea/rsa.hpp>
#include <marnav/nmea/mwv.hpp>
#include <marnav/nmea/vhw.hpp>
#include <marnav/nmea/vpw.hpp>
#include <marnav/nmea/vwr.hpp>
//#include <marnav/nmea/vwt.hpp>
#include <marnav/utils/unique.hpp>

class NMEA0183DataProvider: public DataProvider {
public:
    void parse(std::string line);

private:
    bool replace(std::string& str, const std::string& from, const std::string& to);
    nlohmann::json makeUpdate(std::string talker, std::string id);
    double PI=3.141592653589793238463L;
    double D2R=0.0174533;
    double r2d=57.2957795;
    double KT2MS=0.514444;
    double ms2kt=1.94384;
    double KMH2MS=0.277777778;
    double deg2rad(double deg) {
        return deg * D2R;
    }
    double knots2ms(double knots) {
        return knots*KT2MS;
    }
    double kmh2ms(double kmh) {
        return kmh*KMH2MS;
    }
};


#endif //SIGNALK_SERVER_CPP_NMEA0183DATAPROVIDER_HPP
