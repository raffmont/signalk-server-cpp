//
// Created by Raffaele Montella on 27/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_SUBSCRIPTIONS_HPP
#define SIGNALK_SERVER_CPP_SUBSCRIPTIONS_HPP


#include <string>
#include "SignalKModel.h"
#include "Subscription.hpp"

namespace SignalK {


    class Subscriptions {
    public:
        Subscriptions(SignalK::SignalKModel *pSignalKModel);
        ~Subscriptions();

        void parse(std::string tag, nlohmann::json jMessage, std::function<void(nlohmann::json)> f);
        void removeAll(std::string tag);

    private:
        bool replace(std::string& str, const std::string& from, const std::string& to);

        SignalK::SignalKModel *pSignalKModel;
        std::map<std::string, Subscription *> items;
    };

}
#endif //SIGNALK_SERVER_CPP_SUBSCRIPTIONS_HPP
