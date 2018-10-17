//
// Created by Raffaele Montella on 29/09/2018.
//

#ifndef SIGNALK_SERVER_CPP_UPDATEBUS_HPP
#define SIGNALK_SERVER_CPP_UPDATEBUS_HPP


#include <string>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace SignalK {

    class UpdateBus {
    public:
        void subscribe(nlohmann::json key, std::function<void(nlohmann::json)> f);

        void unsubscribe(nlohmann::json key);

        void publish(nlohmann::json update);

    private:
        std::map<nlohmann::json, std::function<void(nlohmann::json)>> subscriptions;
    };

}

#endif //SIGNALK_SERVER_CPP_UPDATEBUS_HPP
