//
// Created by Giuseppe Dragone
//

#ifndef SIGNALK_SERVER_CPP_SIGNALKMODEL_H
#define SIGNALK_SERVER_CPP_SIGNALKMODEL_H

#include <string>
#include <iostream>
#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <list>
#include <tuple>
#include <mutex>
#include <iomanip>

#include "Node.hpp"

#include <spdlog/spdlog.h>
#include <mpark/variant.hpp>
#include <nlohmann/json.hpp>

#include "UpdateBus.hpp"


namespace SignalK {
    
    class SignalKModel
    {

    public:

        SignalKModel();
        SignalKModel(const std::string& self, const std::string& version);
        SignalKModel(const std::string& json, bool flatten, bool strict);
        SignalKModel(const SignalKModel& other);
        SignalKModel(SignalK::SignalKModel&& other);
        ~SignalKModel();
        SignalKModel& operator=(SignalKModel other) {
            this->root=other.root;
            //this->bus=other.bus;
            return *this;
        }
        void load(const std::string& json, bool flatten=false, bool strict = false);
        void load(std::istream& input, bool flatten=false, bool strict = false);
        std::string toJson();
        std::string subtree(std::string path);
        std::string getVersion();
        std::string getSelf();
        nlohmann::json getHello();
        nlohmann::json getSignalK(std::string bind, int port);
        std::string getSource(std::string id) {
            return subtree("sources." + id);
        }
        std::string getVesselProperty(std::string id, std::string path) {
            return subtree("vessels." + id+"."+path);
        }
        bool update(std::string update);
        bool update(nlohmann::json js);
        std::string readUpdate(std::istream& input);
        std::string currentISO8601TimeUTC();
        UpdateBus *getUpdateBus() { return bus; }
    protected:
        friend std::ostream& operator<< (std::ostream& os, const SignalKModel& dt);
        Node* root;
        UpdateBus * bus;

    };
    std::ostream& operator<< (std::ostream& os, const SignalKModel& dt);
}

#endif //SIGNALK_SERVER_CPP_SIGNALKMODEL_H
