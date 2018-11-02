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

        explicit Node(std::string strVal);

        explicit Node(mpark::variant<std::string, double, bool> vVal);

        explicit Node(double numVal);

        explicit Node(bool boolVal);

        ~Node();




        void replaceChild(std::string key, Node* child);
        bool addChild(std::string key, Node* child);
        bool removeChild(std::string key);
        void removeAllChildren();

        static Node* recursiveLoad(const nlohmann::json& json, bool flatten, bool strict);

        Node* safeCopy();
        Node* safeSubtree(std::string path);

        Node* getChild(const std::string & key) const;
        bool safeChildrenReplace(std::string path, Node* rep);
        bool safeChildrenReplace(std::string path, std::map<std::string, Node*> * children);
        std::string safeSourceUpdate(std::string label, std::string type, std::string timestamp, const std::list<std::pair<std::string, mpark::variant<std::string, double, bool>>>& attributes);

        void recursiveOut(std::ostream& os, std::string path) const;
        static mpark::variant<std::string, double, bool>  jsonToVariant(const nlohmann::json& json);

        nlohmann::json toJson();


        // The value of the Node
        mpark::variant<std::string, double, bool> value;



    private:
        // The value name
        static const std::string valueName;

        // The children map
        std::map<std::string, Node *> *children = nullptr;

        // The safeChildrenHandler
        SignalK::ReaderHandler<std::map<std::string, Node *>> *safeChildrenHandler = nullptr;

        // The mutex used to lock the Node while writing
        std::mutex writeLock;

        // Is this a value node?
        bool valueNode = false;

        // Reecursively render the data structure in JSON
        void toJson(std::ostream& stream);

        static std::string valueAsString(mpark::variant<std::string, double, bool> val);
        Node* safeMove(const std::string & key, bool addIfNotFound = false, const std::string & prevKey = "");
        Node* safeNodeProcess(const std::string & key, std::function<void(Node*)> updater);
        bool safeNodeChildrenReplace(std::map<std::string, Node*> * children);
        bool SameValue(Node *other);

        bool isLeaf() const;
        bool isValueNode() const;


        void declareValueNode();
        void AddSync();

    };
}

#endif //SIGNALK_SERVER_CPP_NODE_HPP
