//
// Created by Raffaele Montella on 29/09/2018.
//

#include "UpdateBus.hpp"

void SignalK::UpdateBus::subscribe(nlohmann::json key, std::function<void(nlohmann::json)> f) {

    subscriptions.insert(std::pair<nlohmann::json,std::function<void(nlohmann::json)>>(key,f));
}

void SignalK::UpdateBus::unsubscribe(nlohmann::json key) {
    // TOTO: Implement the unsubscribe
    subscriptions.erase(key);
}
void SignalK::UpdateBus::publish(nlohmann::json update) {
    std::string updateContext=update["context"];
    for (auto subscription : subscriptions) {

        try {
            nlohmann::json key=subscription.first;
            std::string subscriptionContext=key["context"];
            spdlog::get("console")->debug("updateContext: {0} subscriptionContext: {1}",updateContext,subscriptionContext);
            if (subscriptionContext=="*" && key["subscribe"].size()==1 && key["subscribe"][0]=="*") {

                subscription.second(update);
            } else if (updateContext.find(subscriptionContext)==0 && key["subscribe"].size()==1 && key["subscribe"][0]=="*") {
                subscription.second(update);
            }

        } catch ( std::exception ex) {
            spdlog::get("console")->error("Exception: {0}",ex.what());
        }
    }
}