//
// Created by Raffaele Montella on 20/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_WEBAPIDATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_WEBAPIDATAPROVIDER_HPP


#include <httplib.h>
#include <algorithm>
#include <string>
#include "DataProvider.hpp"

class WebAPIDataProvider : public DataProvider{
public:
    virtual void run();
    WebAPIDataProvider(std::string id,SignalK::SignalKModel *document, std::string bind, int port, std::string root);
    WebAPIDataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options);
    virtual ~WebAPIDataProvider();

private:

    std::string baseUrl="/signalk";
    std::string version="/v1";
    httplib::Server svr;
    std::string root="www";
    std::string bind="localhost";
    int port=3000;
};


#endif //SIGNALK_SERVER_CPP_WEBAPIDATAPROVIDER_HPP
