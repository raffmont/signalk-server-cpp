//
// Created by Raffaele Montella on 29/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_UPDATEBUS_HPP
#define SIGNALK_SERVER_CPP_UPDATEBUS_HPP


#include <string>
#include <regex>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "Subscription.hpp"


namespace SignalK {

    class UpdateBus {
    public:
        void subscribe(Subscription * pSubscription, std::function<void(nlohmann::json)> f);

        void unsubscribe(Subscription * pSubscription);

        void publish(nlohmann::json update);

    private:
        std::map<Subscription *, std::function<void(nlohmann::json)>> subscriptions;
    };

}

#endif //SIGNALK_SERVER_CPP_UPDATEBUS_HPP
