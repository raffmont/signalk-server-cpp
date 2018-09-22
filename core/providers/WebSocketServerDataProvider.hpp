//
// Created by Raffaele Montella on 06/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP


#include <uWS/Hub.h>
#include "../SignalKModel.h"
#include "DataProvider.hpp"
#include "SignalKDataProvider.hpp"

class WebSocketServerDataProvider : public SignalKDataProvider {
public:
    virtual void run();
    WebSocketServerDataProvider(std::string id,SignalK::SignalKModel *document, std::string bind, int port);
    WebSocketServerDataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options);
    virtual ~WebSocketServerDataProvider();

private:
    uWS::Hub h;
    std::string bind="localhost";
    int port=3000;
};


#endif //SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP
