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
    WebAPIDataProvider(std::string id,SignalK::SignalKModel *document, int port);
    virtual ~WebAPIDataProvider();

private:
    std::string bind="localhost";
    std::string baseUrl="/signalk";
    std::string version="/v1";
    httplib::Server svr;
    int port=3000;
};


#endif //SIGNALK_SERVER_CPP_WEBAPIDATAPROVIDER_HPP
