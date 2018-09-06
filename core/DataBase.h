//
// Created by Giuseppe Dragone
//

#ifndef SIGNALK_SERVER_CPP_DATABASE_H
#define SIGNALK_SERVER_CPP_DATABASE_H

#include <string>
#include <iostream>
#include <functional>

namespace SignalK {
    class DataBase
    {

    public:

        DataBase();
        DataBase(const std::string& self, const std::string& version);
        DataBase(const std::string& json, bool flatten, bool strict);
        DataBase(const DataBase& other);
        DataBase(SignalK::DataBase&& other);
        ~DataBase();
        DataBase& operator=(DataBase other) {
            this->root=other.root;
            this->bus=other.bus;
            return *this;
        }
        void load(const std::string& json, bool flatten=false, bool strict = false);
        void load(std::istream& input, bool flatten=false, bool strict = false);
        std::string toJson();
        std::string subtree(std::string path);
        std::string getVersion();
        std::string getSelf();
        std::string getSource(std::string id) {
            return subtree("sources." + id);
        }
        std::string getVesselProperty(std::string id, std::string path) {
            return subtree("vessels." + id+"."+path);
        }
        bool update(std::string update);
        std::string readUpdate(std::istream& input);
        void SubscribeUpdate(std::function<void(std::string)> f);
    protected:
        friend std::ostream& operator<< (std::ostream& os, const DataBase& dt);
        class Node;
        Node* root;
        class UpdateBus;
        UpdateBus * bus;
    };
    std::ostream& operator<< (std::ostream& os, const DataBase& dt);
}

#endif //SIGNALK_SERVER_CPP_DATABASE_H
