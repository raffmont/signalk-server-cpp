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
    WebSocketServerDataProvider(std::string id,SignalK::SignalKModel *document, int port);
    virtual ~WebSocketServerDataProvider();

private:
    uWS::Hub h;
    int port=3000;
};


#endif //SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP
