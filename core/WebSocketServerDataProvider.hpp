//
// Created by Raffaele Montella on 06/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP


#include <uWS/Hub.h>
#include "DataBase.h"
#include "DataProvider.hpp"

class WebSocketServerDataProvider : public DataProvider {
public:


    virtual void run();
    WebSocketServerDataProvider(SignalK::DataBase *document, int port);
    virtual ~WebSocketServerDataProvider();

private:
    uWS::Hub h;
    int port=3000;
};


#endif //SIGNALK_SERVER_CPP_WEBSOCKETDATAPROVIDER_HPP
