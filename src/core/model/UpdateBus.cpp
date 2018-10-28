//
// Created by Raffaele Montella on 29/09/2018.
//

#include "UpdateBus.hpp"

void SignalK::UpdateBus::subscribe(Subscription *pSubscription, std::function<void(nlohmann::json)> f) {

    subscriptions.insert(std::pair<Subscription *,std::function<void(nlohmann::json)>>(pSubscription,f));
}

void SignalK::UpdateBus::unsubscribe(Subscription *pSubscription) {
    // TOTO: Implement the unsubscribe
    subscriptions.erase(pSubscription);
}

/**************
 * Publish updates
 *
 * @param update the update to be published
 *
 */
void SignalK::UpdateBus::publish(nlohmann::json srcUpdate) {


    //spdlog::get("console")->info("SignalK::UpdateBus::publish {0}",srcUpdate.dump(2));

    std::smatch base_match;

    // Get the update context
    std::string updateContext = srcUpdate["context"];

    // For each subscription
    for (auto subscription : subscriptions) {

        // Get the reference to the subscription
        Subscription *pSubscription = subscription.first;

        // Create the delta message
        nlohmann::json dstDelta=nlohmann::json::object();

        // For each update...
        for (auto srcUpdate: srcUpdate["updates"]) {

            // Create the update
            nlohmann::json dstUpdate=nlohmann::json::object();

            // For each value in the update...
            for (auto srcValue: srcUpdate["values"]) {

                // Get the value path as string
                std::string updatePath=srcValue["path"];

                // Prepare the update string
                std::string updateString=updateContext+"."+updatePath;

                // Check if the subscription matches with the update
                if (std::regex_match(updateString, base_match, pSubscription->getRegex())) {

                    //  Check if this is the first update to add
                    if (dstDelta.size()==0) {
                        // Create the delta message
                        dstDelta["context"]=updateContext;
                        dstDelta["updates"]=nlohmann::json::array();

                    }

                    // Check if this is the first update
                    if (dstUpdate.size()==0) {

                        // Create the update
                        dstUpdate["source"] = srcUpdate["source"];
                        dstUpdate["timestamp"]=srcUpdate["timestamp"];
                        dstUpdate["values"]=nlohmann::json::array();
                    }

                    // Append the value to the values
                    dstUpdate["values"].push_back({{"path", srcValue["path"]}, {"value", srcValue["value"]}});


                }
            }

            // Check if an update is present
            if (dstUpdate.size()>0) {
                // Append the update to the updates
                dstDelta["updates"].push_back(dstUpdate);
            }
        }
        // Check if the delta message has to be fired
        if (dstDelta.size()>0) {

            // Fire the update
            subscription.second(dstDelta);
        }
    }
}
