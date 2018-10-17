//
// Created by Raffaele Montella on 14/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_NMEA0183DATASERVER_HPP
#define SIGNALK_SERVER_CPP_NMEA0183DATASERVER_HPP


#include <marnav/nmea/sentence.hpp>
#include "DataServer.hpp"

class NMEA0183DataServer: public DataServer {
public:
    std::vector<marnav::nmea::sentence *> makeSentences(nlohmann::json update);
};


#endif //SIGNALK_SERVER_CPP_NMEA0183DATASERVER_HPP
