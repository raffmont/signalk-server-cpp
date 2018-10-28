//
// Created by Giuseppe Dragone
//


#include "ReaderHandler.h"
#include "SignalKModel.h"


using json = nlohmann::json;


const std::string SignalK::Node::valueName("value");


SignalK::SignalKModel::SignalKModel()
{
    root = nullptr;
    bus = new UpdateBus();
}
SignalK::SignalKModel::SignalKModel(const std::string & self, const std::string & version)
{
    root = new Node();
    root->addChild("self", new Node(self));
    root->addChild("vessels", new Node());
    root->addChild("sources", new Node());
    root->addChild("version", new Node(version));
    bus = new SignalK::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(const SignalK::SignalKModel& other)
        : SignalK::SignalKModel::SignalKModel()
{
    if (other.root != nullptr)
        root = new Node(*(other.root));
    bus = new SignalK::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(SignalK::SignalKModel&& other)
        : SignalK::SignalKModel::SignalKModel()
{
    if (other.root != nullptr)
    {
        root = other.root;
        other.root = nullptr;
    }
    bus = new SignalK::UpdateBus();
}
SignalK::SignalKModel::SignalKModel(const std::string& json, bool flatten, bool strict)
        : SignalK::SignalKModel::SignalKModel()
{
    load(json, flatten, strict);
    bus = new SignalK::UpdateBus();
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
nlohmann::json SignalK::SignalKModel::subtree(std::string path)
{
    if(root==nullptr) return std::string("");
    Node * res = nullptr;
    if (path.empty()) {
        res= root->safeCopy();
    }
    else
        res = root->safeSubtree(path);
    if (res == nullptr) return  std::string("");
    auto fres = res->toJson();
    //if (res == nullptr) delete res;



    return fres;

}

nlohmann::json SignalK::SignalKModel::getHello() {
    nlohmann::json hello={
        {"name","signalk-dynamo-server"},
        {"version", getVersion()},
        {"timestamp", currentISO8601TimeUTC()},
        {"self", "vessels."+getSelf()},
        {"roles",{ "master","main"}}
    };
    return hello;
}

nlohmann::json SignalK::SignalKModel::getSignalK(std::string bind, int port) {
    nlohmann::json signalk={
        {
            "endpoints",
                {
                    {"v1",
                         {
                                 {"version", getVersion()} ,
                                 {"signalk-http", "http://" + bind + ":" + std::to_string(port) + "/signalk/v1/api/"},
                                 {"signalk-ws","ws://" + bind + ":" + std::to_string(port) + "/signalk/v1/stream"}

                         }
                    }
                }
        },
        {
            "server", {
                 {"id", "signalk-dynamo-server"},
                 {"version", getVersion()}
            }
        }

    };
    return signalk;
}

nlohmann::json SignalK::SignalKModel::getPlugins() {
    return nlohmann::json::array();
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
    try {
        nlohmann::json js = nlohmann::json::parse(update);
        bool fres = this->update(js);
        return fres;
    } catch (std::exception &ex) {
        return false;
    }
}

bool SignalK::SignalKModel::update(nlohmann::json js)
{
    try
    {
        bool changed = false;

        std::string ctx;
        nlohmann::json jId = js["context"];
        bool fres = false;
        if (jId.is_string())
        {
            std::string t = jId;
            ctx = t;
        }
        else ctx = "";
        if (ctx.empty()) ctx = "vessels."+getSelf();

        nlohmann::json jUpdates = js["updates"];
        if (!jUpdates.is_array()) return false;
        std::size_t updatePos = 0;
        std::vector<std::size_t>  allUpdatePos;
        for (auto &el : jUpdates)
        {
            auto jTimestamp = el["timestamp"];
            std::string timestamp;
            if (jTimestamp.is_string())
            {
                std::string tmp = jTimestamp;
                timestamp = tmp;
            }
            if (timestamp.empty()) {
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
            std::string label;
            std::string type;
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
            if (label.empty()) {
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
                if (path.empty()) continue;
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
            //bus->Publish(js.dump());
            bus->publish(js);

        }
        return fres;
    }
    catch (std::exception &ex)
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


std::ostream & SignalK::operator<<(std::ostream & os, const SignalKModel & dt)
{
    if (dt.root == nullptr) os << "-- --" << std::endl;
    else dt.root->recursiveOut(os, "");
    return os;
}


/**
 * Generate a UTC ISO8601-formatted timestamp
 * and return as std::string
 */
std::string SignalK::SignalKModel::currentISO8601TimeUTC() {
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(gmtime(&itt), "%FT%TZ");
    return ss.str();
}


