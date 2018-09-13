//
// Created by Giuseppe Dragone
//

#include <map>
#include <string>
#include <sstream>
#include <mpark/variant.hpp>
#include <nlohmann/json.hpp>
#include "ReaderHandler.h"
#include "SignalKModel.h"
#include <list>
#include <tuple>
#include <mutex>
//#include <mpm/eventbus.h>
/*
class SignalK::DataBase::UpdateBus
{
	public:
		void Subscribe(std::function<void(std::string)> f)
		{
			auto s=ebus.subscribe<std::string,  std::function<void (const std::string&)>>(
				[f](const std::string& mo) noexcept {
				try {
					f(mo);
				}
				catch (...) {

				}
			}
			);
			subscriptions.push_back(s);
		}
		void Publish(std::string update)
		{
			ebus.publish(update);
		}
		~UpdateBus()
		{
			for (auto s : subscriptions) ebus.unsubscribe(s);
		}
	private:
		mpm::eventbus ebus;
		std::vector<mpm::cookie> subscriptions;
};
*/
class SignalK::SignalKModel::UpdateBus
{
public:
    void Subscribe(std::function<void(std::string)> f)
    {
        subscriptions.push_back(f);
    }
    void Publish(std::string update)
    {
        for (auto f : subscriptions)
            f(update);
    }

private:
    std::vector<std::function<void(std::string)>> subscriptions;
};

class SignalK::SignalKModel::Node
{
public:
    mpark::variant<std::string, double, bool> value;
    Node(std::string strVal)
    {
        value = strVal;
    }
    Node(mpark::variant<std::string, double, bool> vVal)
    {
        value = vVal;
    }
    Node(double numVal)
    {
        value = numVal;
    }
    Node(bool boolVal)
    {
        value = boolVal;
    }
    Node()
    {
        value = nullptr;
        AddSync();
    }
    Node(const Node& other)
    {
        value = other.value;
        if (other.children != nullptr)
        {
            for (auto pair : *other.children)
                addChild(pair.first, new Node(*(pair.second)));
        }
        if (other.safeChildrenHandler != nullptr) AddSync();
        valueNode = other.valueNode;
    }

    Node(Node&& other)
    {
        value = other.value;
        children = other.children;
        other.children = nullptr;
        other.value = nullptr;
        if (other.safeChildrenHandler != nullptr) AddSync();
        valueNode = other.valueNode;
    }
    ~Node()
    {
        if (safeChildrenHandler != nullptr) delete safeChildrenHandler;
        safeChildrenHandler = nullptr;
        removeAllChildren();

    }
    inline bool isLeaf() const {
        return children == nullptr;
    }
    inline bool isValueNode() const {
        return valueNode;
    }
    static std::string valueAsString(mpark::variant<std::string, double, bool> val)
    {
        if (val.index() == 0) return mpark::get<0>(val);
        else if (val.index() == 1) return std::to_string((int)(mpark::get<1>(val)+0.4));
        else  return std::to_string(mpark::get<2>(val));
    }
    Node* getChild(const std::string & key) const
    {
        if (children == nullptr) return nullptr;
        else return (*children)[key];
    }
    Node* safeMove(const std::string & key, bool addIfNotFound = false, const std::string & prevKey = "")
    {
        if (addIfNotFound) writeLock.lock();
        Node * res = nullptr;
        if (safeChildrenHandler == nullptr && !addIfNotFound) return nullptr;
        else {
            auto children = safeChildrenHandler->enterReader();
            if(children!=nullptr) res = (*children)[key];
            safeChildrenHandler->exitReader();
        }
        if (res == nullptr && addIfNotFound) {

            if (children == nullptr)
            {
                AddSync();
                auto newChildren = new std::map<std::string, Node *>();
                (*newChildren)[key] = res = new Node();
                if (prevKey == "vessels") res->addChild("uuid", new Node(key));
                safeChildrenHandler->replace(newChildren);
            }
            else {
                res = new Node();
                if (prevKey == "vessels") res->addChild("uuid", new Node(key));
                safeChildrenHandler->modify([res, key, addIfNotFound](std::map<std::string, Node *>* x)
                                            {
                                                (*x)[key] = res;
                                            });
            }
        }
        if (addIfNotFound) writeLock.unlock();
        return res;
    }
    Node* safeNodeProcess(const std::string & key, std::function<void(Node*)> updater)
    {
        writeLock.lock();
        Node * res = nullptr;
        auto children = safeChildrenHandler->enterReader();
        if(children != nullptr) res = (*children)[key];
        safeChildrenHandler->exitReader();
        if (res == nullptr) {
            if (children == nullptr)
            {
                AddSync();
                auto newChildren = new std::map<std::string, Node *>();
                (*newChildren)[key] = res = new Node();
                updater(res);
                safeChildrenHandler->replace(newChildren);
            }
            else {
                res = new Node();
                updater(res);
                safeChildrenHandler->modify([res, key](std::map<std::string, Node *>* x)
                                            {
                                                (*x)[key] = res;
                                            });
            }
        }
        writeLock.unlock();
        return res;
    }
    Node* safeCopy() {
        auto children = this->children;
        if (safeChildrenHandler != nullptr)
            children = safeChildrenHandler->enterReader();

        auto res = new Node();
        if (children != nullptr)
        {
            for (auto pair : *children)
                res->addChild(pair.first, pair.second == nullptr ? nullptr : pair.second->safeCopy());
        }
        res->value = value;
        res->valueNode = valueNode;
        if (safeChildrenHandler != nullptr)
            safeChildrenHandler->exitReader();
        return res;
    }
    Node* safeSubtree(std::string path) {
        std::istringstream f(path);
        std::string s;
        Node * curr = this;;
        while (curr != nullptr && std::getline(f, s, '.'))
        {
            curr = curr->safeMove(s);
        }
        if (curr == nullptr) return nullptr;
        else return curr->safeCopy();
    }
    bool safeNodeChildrenReplace(std::map<std::string, Node*> * children)
    {
        writeLock.lock();
        if (safeChildrenHandler == nullptr) AddSync();
        bool res=safeChildrenHandler->replaceIf(children,
                                                [](std::map<std::string, Node*> *oldChildren, std::map<std::string, Node*> *newChildren)->bool
                                                {
                                                    if (oldChildren == nullptr) return true;
                                                    else
                                                    {
                                                        /*auto newValue = (*newChildren)[SignalK::DataBase::Node::valueName];
                                                        if (newValue == nullptr) return false;
                                                        if (newValue->SameValue((*oldChildren)[SignalK::DataBase::Node::valueName])) return false;*/
                                                        auto  oldTime = (*oldChildren)["timestamp"];
                                                        if (oldTime == nullptr) return true;
                                                        auto  newTime = (*newChildren)["timestamp"];
                                                        if (newTime == nullptr) return false;
                                                        auto oldVariant = (*oldTime).value;
                                                        auto newVariant = (*newTime).value;
                                                        std::string oldTimeS = oldVariant.index() == 0 ? mpark::get<0>(oldVariant) : "";
                                                        std::string newTimeS = newVariant.index() == 0 ? mpark::get<0>(newVariant) : "";
                                                        return newTimeS >= oldTimeS;
                                                    }

                                                });
        writeLock.unlock();
        return res;
    }
    bool safeChildrenReplace(std::string path, Node* rep)
    {
        auto children = rep->children;
        rep->children = nullptr;
        return safeChildrenReplace(path, children);
    }

    bool safeChildrenReplace(std::string path, std::map<std::string, Node*> * children) {
        std::istringstream f(path);
        std::string s;
        Node * curr = this;
        std::string prevKey = "";
        while (curr != nullptr && std::getline(f, s, '.'))
        {
            curr = curr->safeMove(s, true, prevKey);
            prevKey = s;
        }
        assert(curr != nullptr);
        return curr->safeNodeChildrenReplace(children);
    }
    std::string safeSourceUpdate(std::string label, std::string type, std::string timestamp, const std::list<std::pair<std::string, mpark::variant<std::string, double, bool>>>& attributes)
    {
        Node * curr = this;
        curr = curr->safeMove("sources", true);
        curr = curr->safeNodeProcess(label, [label, type](Node* n) {
            n->addChild("label", new Node(label));
            n->addChild("type", new Node(type));
        });
        std::string res = label;
        for (auto pair : attributes)
        {
            auto currKey = valueAsString(pair.second);
            curr = curr->safeNodeProcess(currKey, [pair](Node* n) {
                n->addChild(pair.first, new Node(pair.second));
            });
            res = res + "." + currKey;
        }
        curr->safeChildrenHandler->modify([timestamp](std::map<std::string, Node *>* x)
                                          {
                                              auto t=(*x)["timestamp"];
                                              if (t == nullptr) {
                                                  (*x)["timestamp"] = new Node(timestamp);
                                              }
                                              else if(t->value.index()==0 && mpark::get<0>(t->value)<timestamp)
                                              {
                                                  delete t;
                                                  (*x)["timestamp"] = new Node(timestamp);
                                              }
                                          });
        return res;
    }
    bool SameValue(Node *other) {
        if (other == nullptr) return false;
        return value == other->value;
    }
    void toJson(std::ostream& stream)
    {

        if (isLeaf())
        {
            nlohmann::json emptyObj;
            if (value.index() == 0)
            {
                std::string val= mpark::get<0>(value);
                emptyObj = val;
            }
            else if (value.index() == 1)
            {
                double val = mpark::get<1>(value);
                emptyObj = val;
            }
            else if (value.index() == 2)
            {
                bool val = mpark::get<2>(value);
                emptyObj = val;
            }
            stream << emptyObj.dump();
        }
        else {

            stream << "{" << std::endl;
            bool first= true;
            for (auto pair : *children)
            {
                if (pair.second == nullptr) continue;
                if (first) first = false;
                else
                {
                    stream << "," << std::endl;
                }

                stream << "\"" << pair.first << "\":" << std::endl;
                (pair.second)->toJson(stream);


            }
            stream << std::endl << "}" << std::endl;
        }

    }
    std::string toJson()
    {
        std::ostringstream s1;
        toJson(s1);
        nlohmann::json j = nlohmann::json::parse(s1.str());
        return j.dump(4);
    }

    void replaceChild(std::string key, Node* child)
    {
        auto val = getChild(key);
        if (children == nullptr) {
            children = new std::map<std::string, Node*>();
            AddSync();
        }
        if (val != nullptr) delete val;
        (*children)[key] = child;


    }
    bool addChild(const std::string key, Node* child)
    {
        if (getChild(key) != nullptr) return false;
        else
        {
            if (children == nullptr)
                children = new std::map<std::string, Node*>();
            (*children)[key] = child;
            return true;
        }
    }
    bool removeChild(const std::string key)
    {
        auto child = getChild(key);
        if (child == nullptr) return false;
        children->erase(key);
        delete child;
        return true;
    }
    void removeAllChildren()
    {
        if (children != nullptr)
        {
            for (auto pair : *children)
            {
                if (pair.second != nullptr)
                    delete pair.second;
            }


            delete children;
            children = nullptr;
        }
    }


    void recursiveOut(std::ostream& os, std::string path) const
    {
        if (isLeaf())
        {
            os << path << " ";
            if (value.index() == 0)
                os << mpark::get<0>(value);
            else if (value.index() == 1)
                os << mpark::get<1>(value);
            else if (value.index() == 2)
                os << mpark::get<2>(value);
            os << std::endl;
        }
        else {
            for (auto pair : *children)
            {
                if (pair.second != nullptr)
                    pair.second->recursiveOut(os, path + (isValueNode() ? "*" : "") + "/" + pair.first);
            }
        }
    }
    static mpark::variant<std::string, double, bool>  jsonToVariant(const nlohmann::json& json)
    {
        mpark::variant<std::string, double, bool> res = false;
        if (json.is_boolean())
        {
            bool val = json;
            res = val;
        }
        else if (json.is_number())
        {
            double  val = json;
            res = val;
        }
        else if (json.is_string())
        {
            std::string  val = json;
            res = val;
        }
        return res;
    }
    static Node* recursiveLoad(const nlohmann::json& json, bool flatten, bool strict)
    {
        if (json.is_null()) return nullptr;
        else if (json.is_boolean())
        {
            bool val = json;
            return new Node(val);
        }
        else if (json.is_number())
        {
            double  val = json;
            return new Node(val);
        }
        else if (json.is_string())
        {
            std::string  val = json;
            return new Node(val);
        }
        else if (json.is_object())
        {
            if (strict)
            {
                for (auto& cval : json.items())
                {
                    if (cval.key() == valueName)
                    {
                        auto pres = recursiveLoad(cval.value(), flatten, strict);
                        pres->declareValueNode();
                        return pres;
                    }

                }
            }
            Node * result = new Node();
            for (auto& cval : json.items())
            {
                if (flatten && cval.key() == valueName && cval.value().is_object())
                {
                    for (auto innerVal : cval.value().items())
                        result->addChild(innerVal.key(), recursiveLoad(innerVal.value(), flatten, strict));
                }
                else
                {
                    result->addChild(cval.key(), recursiveLoad(cval.value(), flatten, strict));
                }
                if (cval.key() == valueName) result->declareValueNode();
            }
            return result;
        }
        return nullptr;
    }
    void declareValueNode()
    {
        valueNode = true;
    }
    void AddSync()
    {
        if (safeChildrenHandler == nullptr)
            safeChildrenHandler =
                    new SignalK::ReaderHandler<std::map<std::string, Node *>>(&children, [this]() ->void {this->removeAllChildren(); });
    }
private:
    static const std::string valueName;
    std::map<std::string, Node *> *children = nullptr;
    SignalK::ReaderHandler<std::map<std::string, Node *>> *safeChildrenHandler = nullptr;
    std::mutex writeLock;
    bool valueNode = false;
};
const std::string SignalK::SignalKModel::Node::valueName("value");


SignalK::SignalKModel::SignalKModel()
{
    root = nullptr;
    bus = new SignalK::SignalKModel::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(const std::string & self, const std::string & version)
{
    root = new Node();
    root->addChild("self", new Node(self));
    root->addChild("vessels", new Node());
    root->addChild("sources", new Node());
    root->addChild("version", new Node(version));
    bus = new SignalK::SignalKModel::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(const SignalK::SignalKModel& other)
        : SignalK::SignalKModel::SignalKModel()
{
    if (other.root != nullptr)
        root = new Node(*(other.root));
    bus = new SignalK::SignalKModel::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(SignalK::SignalKModel&& other)
        : SignalK::SignalKModel::SignalKModel()
{
    if (other.root != nullptr)
    {
        root = other.root;
        other.root = nullptr;
    }
    bus = new SignalK::SignalKModel::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(const std::string& json, bool flatten, bool strict)
        : SignalK::SignalKModel::SignalKModel()
{
    load(json, flatten, strict);
    bus = new SignalK::SignalKModel::UpdateBus();
}

SignalK::SignalKModel::~SignalKModel()
{
    if (root != nullptr) delete root;
    if (bus != nullptr) delete bus;
}
void SignalK::SignalKModel::load(const std::string& json, bool flatten, bool strict)
{
    if (root != nullptr) delete root;
    root = nullptr;
    auto tree = nlohmann::json::parse(json);
    root  = Node::recursiveLoad(tree, flatten, strict);
}
void SignalK::SignalKModel::load(std::istream& input, bool flatten, bool strict)
{
    if (root != nullptr) delete root;
    root = nullptr;
    nlohmann::json tree;
    input >> tree;
    root = Node::recursiveLoad(tree, flatten, strict);
}
std::string SignalK::SignalKModel::toJson()
{
    if (root == nullptr) return std::string();
    else return root->toJson();
}
std::string SignalK::SignalKModel::subtree(std::string path)
{
    if(root==nullptr) return std::string("");
    Node * res = nullptr;
    if (path == "") {
        res= root->safeCopy();
    }
    else
        res = root->safeSubtree(path);
    if (res == nullptr) return  std::string("");
    auto fres = res->toJson();
    if (res == nullptr) delete res;
    return fres;

}
std::string SignalK::SignalKModel::getVersion()
{
    if (root == nullptr) return std::string();
    else {
        auto child = root->getChild("version");
        if (child == nullptr ) return std::string();
        if (child->value.index() == 0) return mpark::get<0>(child->value);
        else return std::string();
    }
}
std::string SignalK::SignalKModel::getSelf()
{
    if (root == nullptr) return std::string();
    else {
        auto child = root->getChild("self");
        if (child == nullptr ) return std::string();
        if (child->value.index() == 0) return mpark::get<0>(child->value);
        else return std::string();
    }
}
bool SignalK::SignalKModel::update(std::string update)
{
    try
    {
        bool changed = false;
        nlohmann::json js = nlohmann::json::parse(update);
        std::string ctx;
        nlohmann::json jId = js["context"];
        bool fres = false;
        if (jId.is_string())
        {
            std::string t = jId;
            ctx = t;
        }
        else ctx = "";
        if (ctx == "") ctx = "vessels."+getSelf();

        nlohmann::json jUpdates = js["updates"];
        if (!jUpdates.is_array()) return false;
        std::size_t updatePos = 0;
        std::vector<std::size_t>  allUpdatePos;
        for (auto &el : jUpdates)
        {
            auto jTimestamp = el["timestamp"];
            std::string timestamp = "";
            if (jTimestamp.is_string())
            {
                std::string tmp = jTimestamp;
                timestamp = tmp;
            }
            if (timestamp == "") {
                allUpdatePos.push_back(updatePos);
                updatePos++;
                continue;
            }
            auto jSource = el["source"];
            if (!jSource.is_object()) {
                allUpdatePos.push_back(updatePos);
                updatePos++;
                continue;
            }
            std::string label = "";
            std::string type = "";
            std::list<std::pair<std::string, mpark::variant<std::string, double, bool>>> attributes;
            for (auto& attribute : jSource.items())
            {
                if (attribute.key() == "label" && attribute.value().is_string())
                {
                    std::string tmp = attribute.value();
                    label = tmp;

                }
                else if (attribute.key() == "type" && attribute.value().is_string())
                {
                    std::string tmp = attribute.value();
                    type = tmp;
                }
                else {

                    attributes.push_front(std::make_pair(attribute.key(),
                                                         Node::jsonToVariant(attribute.value())));
                }
            }
            if (label == "") {
                allUpdatePos.push_back(updatePos);
                updatePos++;
                continue;
            }
            attributes.sort([](std::pair<std::string, mpark::variant<std::string, double, bool>> a,
                    std::pair<std::string, mpark::variant<std::string, double, bool>> b) -> bool {
                    return a.first < b.first;
            });
            auto jValues = el["values"];
            if (!jValues.is_array()) {
                allUpdatePos.push_back(updatePos);
                updatePos++;
                continue;
            }
            std::string sourceReference = root->safeSourceUpdate(label, type, timestamp, attributes);

            std::size_t valuePos = 0;
            std::vector<std::size_t>  allValuePos;
            bool vChange = false;
            for (auto &val : jValues)
            {
                auto curr = Node::recursiveLoad(val, false, false);
                if (curr == nullptr) {
                    allValuePos.push_back(valuePos);
                    valuePos++;
                    continue;
                }
                auto pathNode = curr->getChild("path");

                if (pathNode == nullptr || pathNode->value.index() != 0)
                {
                    allValuePos.push_back(valuePos);
                    valuePos++;
                    continue;
                }
                std::string path = mpark::get<0>(pathNode->value);
                if (path == "") continue;
                curr->removeChild("path");
                curr->addChild("timestamp", new Node(timestamp));
                curr->addChild("$source", new Node(sourceReference));
                fres = true;
                if (root->safeChildrenReplace(ctx + "." + path, curr))
                {
                    vChange = true;
                }
                else allValuePos.push_back(valuePos);
                valuePos++;
                delete curr;
            }
            if (vChange) {
                changed = true;
                std::reverse(allValuePos.begin(), allValuePos.end());
                for (auto &pos : allValuePos) js["updates"][updatePos]["values"].erase(pos);
            }
            else {
                allUpdatePos.push_back(updatePos);

            }
            updatePos++;
        }
        if (changed ) {
            std::reverse(allUpdatePos.begin(), allUpdatePos.end());
            for (auto &upos : allUpdatePos) js["updates"].erase(upos);
            bus->Publish(js.dump());
        }
        return fres;
    }
    catch (std::exception ex)
    {
        return false;
    }
}

std::string SignalK::SignalKModel::readUpdate(std::istream & input)
{
    try {


        nlohmann::json json;
        input >> json;
        if (json.is_null())
            return std::string();
        return json.dump(4);
    }
    catch(...){
        return std::string();
    }
}

void SignalK::SignalKModel::SubscribeUpdate(std::function<void(std::string)> f)
{
    bus->Subscribe(f);
}

std::ostream & SignalK::operator<<(std::ostream & os, const SignalKModel & dt)
{
    if (dt.root == nullptr) os << "-- --" << std::endl;
    else dt.root->recursiveOut(os, "");
    return os;
}
