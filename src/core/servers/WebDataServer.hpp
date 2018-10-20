//
// Created by Raffaele Montella on 06/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_WEBSOCKETDATASERVER_HPP
#define SIGNALK_SERVER_CPP_WEBSOCKETDATASERVER_HPP


#include <uWS/Hub.h>
#include "../model/SignalKModel.h"
#include "SignalKDataServer.hpp"
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <iostream>
#include <sstream>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>


class WebDataServer : public SignalKDataServer {
public:

    const std::string SIGNALK="/signalk";
    const std::string SIGNALK_V1_STREAM="/signalk/v1/stream";
    const std::string SIGNALK_V1_API="/signalk/v1/api/";

    WebDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, std::string bind, int port, std::string root);
    WebDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~WebDataServer();

    virtual void onRun();


private:
    long millis=1000;
    uWS::Hub h;
    std::string root="www";
    std::string bind="localhost";
    int port=3000;

    static bool hasEnding (std::string const &fullString, std::string const &ending);
    static unsigned char random_char();
    static std::string generate_hex(const unsigned int len);
};


#endif //SIGNALK_SERVER_CPP_WEBSOCKETDATASERVER_HPP
