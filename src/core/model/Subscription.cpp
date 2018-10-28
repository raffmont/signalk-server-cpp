//
// Created by Raffaele Montella on 27/10/2018.
//

#include "Subscription.hpp"

SignalK::Subscription::Subscription(std::string context, nlohmann::json subscribe) {
    this->context=context;
    std::string path=subscribe["path"];
    this->path=path;

    // Escape the regex chars
    std::string subscriptionString=replaceString(context+"."+path,".","[.]");
    subscriptionString=replaceString(subscriptionString,"*",".*");

    // Create the regex
    this->regex=std::regex(subscriptionString);
}

std::string SignalK::Subscription::getKey() {
    return context+"."+path;
}


std::string SignalK::Subscription::replaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}


