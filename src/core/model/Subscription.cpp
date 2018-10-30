//
// Created by Raffaele Montella on 27/10/2018.
//

#include "Subscription.hpp"

SignalK::Subscription::Subscription(std::string context, nlohmann::json subscribe) {
    this->context=context;
    std::string path=subscribe["path"];
    this->path=path;

    // Escape the regex chars
    std::string subscriptionString=Utils::String::replace(context+"."+path,".","[.]");
    subscriptionString=Utils::String::replace(subscriptionString,"*",".*");

    // Create the regex
    this->regex=std::regex(subscriptionString);
}

std::string SignalK::Subscription::getKey() {
    return context+"."+path;
}





