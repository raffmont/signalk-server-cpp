//
// Created by Raffaele Montella on 26/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_SIGNALKDATASERVER_HPP
#define SIGNALK_SERVER_CPP_SIGNALKDATASERVER_HPP


#include "DataServer.hpp"
#include "../model/Subscriptions.hpp"
#include <algorithm>





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SignalKDataServer : public DataServer {
public:
    SignalKDataServer();
    SignalKDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel);
    SignalKDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    ~SignalKDataServer();

    void parse(nlohmann::json jMessage);
    SignalK::Subscriptions *getSubscriptions() { return pSubscriptions; }
private:
    SignalK::Subscriptions *pSubscriptions;


};


#endif //SIGNALK_SERVER_CPP_SIGNALKDATASERVER_HPP
