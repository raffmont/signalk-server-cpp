//
// Created by Raffaele Montella on 01/10/2018.
//


#include "Node.hpp"

namespace SignalK {

    Node::Node(std::string strVal) {
        value = strVal;
    }

    Node::Node(mpark::variant<std::string, double, bool> vVal) {
        value = vVal;
    }

    Node::Node(double numVal) {
        value = numVal;
    }

    Node::Node(bool boolVal) {
        value = boolVal;
    }

    Node::Node() {
        value = nullptr;
        AddSync();
    }

    Node::Node(const Node &other) {
        value = other.value;
        if (other.children != nullptr) {
            for (auto pair : *other.children)
                addChild(pair.first, new Node(*(pair.second)));
        }
        if (other.safeChildrenHandler != nullptr) AddSync();
        valueNode = other.valueNode;
    }

    Node::Node(Node && other) {
        value = other.value;
        children = other.children;
        other.
        children = nullptr;
        other.
        value = nullptr;
        if (other.safeChildrenHandler != nullptr)

        AddSync();

        valueNode = other.valueNode;
    }


    Node::~Node() {
        if (safeChildrenHandler != nullptr) delete safeChildrenHandler;
        safeChildrenHandler = nullptr;
        removeAllChildren();

    }

    inline bool Node::isLeaf() const {
        return children == nullptr;
    }

    inline bool Node::isValueNode() const {
        return valueNode;
    }

     std::string Node::valueAsString(mpark::variant<std::string, double, bool> val) {
        if (val.index() == 0) return mpark::get<0>(val);
        else if (val.index() == 1) return std::to_string((int) (mpark::get<1>(val) + 0.4));
        else return std::to_string(mpark::get<2>(val));
    }

    Node *Node::getChild(const std::string &key) const {
        if (children == nullptr) return nullptr;
        else return (*children)[key];
    }

    Node *Node::safeMove(const std::string &key, bool addIfNotFound, const std::string &prevKey) {
        if (addIfNotFound) writeLock.lock();
        Node * res = nullptr;
        if (safeChildrenHandler == nullptr && !addIfNotFound) return nullptr;
        else {
            auto children = safeChildrenHandler->enterReader();
            if (children != nullptr) res = (*children)[key];
            safeChildrenHandler->exitReader();
        }
        if (res == nullptr && addIfNotFound) {

            if (children == nullptr) {
                AddSync();
                auto newChildren = new std::map<std::string, Node *>();
                (*newChildren)[key] = res = new Node();
                if (prevKey == "vessels") res->addChild("uuid", new Node(key));
                safeChildrenHandler->replace(newChildren);
            } else {
                res = new Node();
                if (prevKey == "vessels") res->addChild("uuid", new Node(key));
                safeChildrenHandler->modify([res, key, addIfNotFound](std::map<std::string, Node *> *x) {
                    (*x)[key] = res;
                });
            }
        }
        if (addIfNotFound) writeLock.unlock();
        return res;
    }

    Node *Node::safeNodeProcess(const std::string &key, std::function<void(Node * )> updater) {
        writeLock.lock();
        Node * res = nullptr;
        auto children = safeChildrenHandler->enterReader();
        if (children != nullptr) res = (*children)[key];
        safeChildrenHandler->exitReader();
        if (res == nullptr) {
            if (children == nullptr) {
                AddSync();
                auto newChildren = new std::map<std::string, Node *>();
                (*newChildren)[key] = res = new Node();
                updater(res);
                safeChildrenHandler->replace(newChildren);
            } else {
                res = new Node();
                updater(res);
                safeChildrenHandler->modify([res, key](std::map<std::string, Node *> *x) {
                    (*x)[key] = res;
                });
            }
        }
        writeLock.unlock();
        return res;
    }

    Node *Node::safeCopy() {
        auto children = this->children;
        if (safeChildrenHandler != nullptr)
            children = safeChildrenHandler->enterReader();

        auto res = new Node();
        if (children != nullptr) {
            for (auto pair : *children)
                res->addChild(pair.first, pair.second == nullptr ? nullptr : pair.second->safeCopy());
        }
        res->value = value;
        res->valueNode = valueNode;
        if (safeChildrenHandler != nullptr)
            safeChildrenHandler->exitReader();
        return res;
    }

    Node *Node::safeSubtree(std::string path) {
        std::istringstream f(path);
        std::string s;
        Node * curr = this;;
        while (curr != nullptr && std::getline(f, s, '.')) {
            curr = curr->safeMove(s);
        }
        if (curr == nullptr) return nullptr;
        else return curr->safeCopy();
    }

    bool Node::safeNodeChildrenReplace(std::map<std::string, Node *> *children) {
        writeLock.lock();
        if (safeChildrenHandler == nullptr) AddSync();
        bool res = safeChildrenHandler->replaceIf(children,
          [](std::map<std::string, Node *> *oldChildren,
             std::map<std::string, Node *> *newChildren) -> bool {
              if (oldChildren == nullptr) return true;
              else {
                  /*auto newValue = (*newChildren)[SignalK::DataBase::Node::valueName];
                  if (newValue == nullptr) return false;
                  if (newValue->SameValue((*oldChildren)[SignalK::DataBase::Node::valueName])) return false;*/
                  auto oldTime = (*oldChildren)["timestamp"];
                  if (oldTime == nullptr) return true;
                  auto newTime = (*newChildren)["timestamp"];
                  if (newTime == nullptr) return false;
                  auto oldVariant = (*oldTime).value;
                  auto newVariant = (*newTime).value;
                  std::string oldTimeS =
                          oldVariant.index() == 0 ? mpark::get<0>(oldVariant) : "";
                  std::string newTimeS =
                          newVariant.index() == 0 ? mpark::get<0>(newVariant) : "";
                  return newTimeS >= oldTimeS;
              }

        });
        writeLock.unlock();
        return res;
    }

    bool Node::safeChildrenReplace(std::string path, Node *rep) {
        auto children = rep->children;
        rep->children = nullptr;
        return safeChildrenReplace(path, children);
    }

    bool Node::safeChildrenReplace(std::string path, std::map<std::string, Node *> *children) {
        std::istringstream f(path);
        std::string s;
        Node * curr = this;
        std::string prevKey = "";
        while (curr != nullptr && std::getline(f, s, '.')) {
            curr = curr->safeMove(s, true, prevKey);
            prevKey = s;
        }
        assert(curr != nullptr);
        return curr->safeNodeChildrenReplace(children);
    }

    std::string Node::safeSourceUpdate(std::string label, std::string type, std::string timestamp,
                                 const std::list<std::pair<std::string, mpark::variant<std::string, double, bool>>> &attributes) {
        Node * curr = this;
        curr = curr->safeMove("sources", true);
        curr = curr->safeNodeProcess(label, [label, type](Node *n) {
            n->addChild("label", new Node(label));
            n->addChild("type", new Node(type));
        });
        std::string res = label;
        for (auto pair : attributes) {
            auto currKey = valueAsString(pair.second);
            curr = curr->safeNodeProcess(currKey, [pair](Node *n) {
                n->addChild(pair.first, new Node(pair.second));
            });
            res = res + "." + currKey;
        }
        curr->safeChildrenHandler->modify([timestamp](std::map<std::string, Node *> *x) {
            auto t = (*x)["timestamp"];
            if (t == nullptr) {
                (*x)["timestamp"] = new Node(timestamp);
            } else if (t->value.index() == 0 && mpark::get<0>(t->value) < timestamp) {
                delete t;
                (*x)["timestamp"] = new Node(timestamp);
            }
        });
        return res;
    }

    bool Node::SameValue(Node * other) {
        if (other == nullptr) return false;
        return value == other->value;
    }

    void Node::toJson(std::ostream &stream) {

        if (isLeaf()) {
            nlohmann::json emptyObj;
            if (value.index() == 0) {
                std::string val = mpark::get<0>(value);
                emptyObj = val;
            } else if (value.index() == 1) {
                double val = mpark::get<1>(value);
                emptyObj = val;
            } else if (value.index() == 2) {
                bool val = mpark::get<2>(value);
                emptyObj = val;
            }
            stream << emptyObj.dump();
        } else {

            stream << "{" << std::endl;
            bool first = true;
            for (auto pair : *children) {
                if (pair.second == nullptr) continue;
                if (first) first = false;
                else {
                    stream << "," << std::endl;
                }

                stream << "\"" << pair.first << "\":" << std::endl;
                (pair.second)->toJson(stream);


            }
            stream << std::endl << "}" << std::endl;
        }

    }

    std::string Node::toJson() {
        std::ostringstream s1;
        toJson(s1);
        nlohmann::json j = nlohmann::json::parse(s1.str());
        return j.dump(4);
    }

    void Node::replaceChild(std::string key, Node *child) {
        auto val = getChild(key);
        if (children == nullptr) {
            children = new std::map<std::string, Node *>();
            AddSync();
        }
        if (val != nullptr) delete val;
        (*children)[key] = child;
    }

    bool Node::addChild(const std::string key, Node *child) {
        if (getChild(key) != nullptr) return false;
        else {
            if (children == nullptr)
                children = new std::map<std::string, Node *>();
            (*children)[key] = child;
            return true;
        }
    }

    bool Node::removeChild(const std::string key) {
        auto child = getChild(key);
        if (child == nullptr) return false;
        children->erase(key);
        delete child;
        return true;
    }

    void Node::removeAllChildren() {
        if (children != nullptr) {
            for (auto pair : *children) {
                if (pair.second != nullptr)
                    delete pair.second;
            }


            delete children;
            children = nullptr;
        }
    }


    void Node::recursiveOut(std::ostream &os, std::string path) const {
        if (isLeaf()) {
            os << path << " ";
            if (value.index() == 0)
                os << mpark::get<0>(value);
            else if (value.index() == 1)
                os << mpark::get<1>(value);
            else if (value.index() == 2)
                os << mpark::get<2>(value);
            os << std::endl;
        } else {
            for (auto pair : *children) {
                if (pair.second != nullptr)
                    pair.second->recursiveOut(os, path + (isValueNode() ? "*" : "") + "/" + pair.first);
            }
        }
    }

    mpark::variant<std::string, double, bool> Node::jsonToVariant(const nlohmann::json &json) {
        mpark::variant<std::string, double, bool> res = false;
        if (json.is_boolean()) {
            bool val = json;
            res = val;
        } else if (json.is_number()) {
            double val = json;
            res = val;
        } else if (json.is_string()) {
            std::string val = json;
            res = val;
        }
        return res;
    }

    Node *Node::recursiveLoad(const nlohmann::json &json, bool flatten, bool strict) {
        if (json.is_null()) return nullptr;
        else if (json.is_boolean()) {
            bool val = json;
            return new Node(val);
        } else if (json.is_number()) {
            double val = json;
            return new Node(val);
        } else if (json.is_string()) {
            std::string val = json;
            return new Node(val);
        } else if (json.is_object()) {
            if (strict) {
                for (auto &cval : json.items()) {
                    if (cval.key() == valueName) {
                        auto pres = recursiveLoad(cval.value(), flatten, strict);
                        pres->declareValueNode();
                        return pres;
                    }

                }
            }
            Node * result = new Node();
            for (auto &cval : json.items()) {
                if (flatten && cval.key() == valueName && cval.value().is_object()) {
                    for (auto innerVal : cval.value().items())
                        result->addChild(innerVal.key(), recursiveLoad(innerVal.value(), flatten, strict));
                } else {
                    result->addChild(cval.key(), recursiveLoad(cval.value(), flatten, strict));
                }
                if (cval.key() == valueName) result->declareValueNode();
            }
            return result;
        }
        return nullptr;
    }

    void Node::declareValueNode() {
        valueNode = true;
    }

    void Node::AddSync() {
        if (safeChildrenHandler == nullptr)
            safeChildrenHandler =
                new SignalK::ReaderHandler<std::map<std::string, Node *>>(&children,
                  [this]() -> void { this->removeAllChildren();
                });
    }

}