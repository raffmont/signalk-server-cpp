//
// Created by Raffaele Montella on 01/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_NODE_HPP
#define SIGNALK_SERVER_CPP_NODE_HPP

#include <spdlog/spdlog.h>
#include <mpark/variant.hpp>
#include <nlohmann/json.hpp>
#include <list>
#include <sstream>

#include "ReaderHandler.h"

namespace SignalK {
    class Node {
    public:


        Node();
        Node(const Node& other);
        Node(Node&& other);

        Node(std::string strVal);
        Node(mpark::variant<std::string, double, bool> vVal);
        Node(double numVal);
        Node(bool boolVal);

        ~Node();

        bool isLeaf() const;
        bool isValueNode() const;

        static std::string valueAsString(mpark::variant<std::string, double, bool> val);
        Node* getChild(const std::string & key) const;
        Node* safeMove(const std::string & key, bool addIfNotFound = false, const std::string & prevKey = "");
        Node* safeNodeProcess(const std::string & key, std::function<void(Node*)> updater);
        Node* safeCopy();
        Node* safeSubtree(std::string path);
        bool safeNodeChildrenReplace(std::map<std::string, Node*> * children);
        bool safeChildrenReplace(std::string path, Node* rep);
        bool safeChildrenReplace(std::string path, std::map<std::string, Node*> * children);
        std::string safeSourceUpdate(std::string label, std::string type, std::string timestamp, const std::list<std::pair<std::string, mpark::variant<std::string, double, bool>>>& attributes);
        bool SameValue(Node *other);
        void toJson(std::ostream& stream);
        std::string toJson();
        void replaceChild(std::string key, Node* child);
        bool addChild(std::string key, Node* child);
        bool removeChild(std::string key);
        void removeAllChildren();
        void recursiveOut(std::ostream& os, std::string path) const;
        static mpark::variant<std::string, double, bool>  jsonToVariant(const nlohmann::json& json);
        static Node* recursiveLoad(const nlohmann::json& json, bool flatten, bool strict);
        void declareValueNode();
        void AddSync();


        mpark::variant<std::string, double, bool> value;

    private:
        static const std::string valueName;
        std::map<std::string, Node *> *children = nullptr;
        SignalK::ReaderHandler<std::map<std::string, Node *>> *safeChildrenHandler = nullptr;
        std::mutex writeLock;
        bool valueNode = false;

    };
}

#endif //SIGNALK_SERVER_CPP_NODE_HPP
