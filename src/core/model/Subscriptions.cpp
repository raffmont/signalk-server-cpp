//
// Created by Raffaele Montella on 27/10/2018.
//

#include "Subscriptions.hpp"

SignalK::Subscriptions::Subscriptions(SignalK::SignalKModel *pSignalKModel) {
    this->pSignalKModel=pSignalKModel;
}

SignalK::Subscriptions::~Subscriptions() {

}


void SignalK::Subscriptions::parse(std::string tag, nlohmann::json jMessage, std::function<void(nlohmann::json)> f) {

    // Get the context
    std::string context=jMessage["context"];

    // Substitute self with the actual uuid
    context=Utils::String::replace(context,"self",pSignalKModel->getSelf());

    // Check if subscribe is present and it is an array
    if (jMessage["subscribe"].is_array()) {
        for (const auto &item:jMessage["subscribe"]) {
            Subscription *pSubscription=new Subscription(context,item);
            std::string key=tag+"/"+pSubscription->getKey();
            std::string path=pSubscription->getPath();

            items.insert(std::pair<std::string,Subscription *>(key,pSubscription));

            pSignalKModel->getUpdateBus()->subscribe(pSubscription,f);
        }

    }
    // Check if unsubscribe is present and it is an array
    if (jMessage["unsubscribe"].is_array()) {
        for (const auto &item:jMessage["unsubscribe"]) {
            Subscription subscription(context, item);
            std::string key=tag+"/"+subscription.getKey();
            Subscription *pSubscription = items[key];
            if (pSubscription) {

                pSignalKModel->getUpdateBus()->unsubscribe(pSubscription);
                items.erase(key);
                delete pSubscription;
            }


        }
    }

}

/*
 *
 * Remove all subscriptions with key beginning with tag
 *
 * @param tag The subscription tag (a client unique id)
 *
 */
void SignalK::Subscriptions::removeAll(std::string tag) {

    // For each subscription...
    for (auto it=items.begin(); it!=items.end();)
    {
        // Get the subscription key
        std::string key=it->first;

        // Get the subscription
        Subscription *pSubscription=it->second;


        // Check if the key matches the tag
        if (key.find(tag)==0) {

            // Unsubscribe from the update bus
            pSignalKModel->getUpdateBus()->unsubscribe(pSubscription);

            // Delete the subscription instance
            delete pSubscription;

            // Erase it
            it = items.erase(it);
        } else {
            // Continue iterating
            ++it;
        }
    }


}
