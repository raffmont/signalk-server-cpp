//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebDataServer.hpp"



WebDataServer::WebDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel,  std::string bind, int port, std::string root) {
    this->enabled=enabled;
    this->type="servers/signalk/web";
    this->id=id;
    this->pSignalKModel=pSignalKModel;
    this->port=port;
    this->root=root;
}

WebDataServer::WebDataServer(bool enabled,std::string id,SignalK::SignalKModel *document, nlohmann::json options): WebDataServer(enabled,id,document,options["bind"],options["port"],options["root"]) {}

WebDataServer::~WebDataServer() {
}


void WebDataServer::onRun(){



    // httpRequest borde vara defaultsatt till att hantera upgrades, ta bort onupgrade! (sätter man request avsätts upgrade handlern)
    h.onHttpRequest([this](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t length, size_t remainingBytes) {
        std::string url = req.getUrl().toString();
        spdlog::get("console")->debug("Request: {0}",url);
        if (url == SIGNALK_V1_STREAM) {

        } else if (url.find(SIGNALK_V1_API)!=std::string::npos) {
            std::string path=url;
            path.erase(0,SIGNALK_V1_API.length());
            std::replace( path.begin(), path.end(), '/', '.');
            std::string signalk=pSignalKModel->subtree(path);
            res->end((char *) signalk.c_str(), signalk.length());
            return;

        } else if (url==SIGNALK) {
            std::string signalk=pSignalKModel->getSignalK(bind,port).dump(2);
            res->end((char *) signalk.c_str(), signalk.length());
            return;
        } else {
            spdlog::get("console")->info("WebDataServer/onHttpRequest: url:{0}",url);
            std::string path=url;
            if (path=="" || path[path.length()-1]=='/') {
                path=path+"index.html";
            }
            std::string localPath=root+"/"+path;
            spdlog::get("console")->info("WebDataServer/onHttpRequest: localPath:{0}",localPath);

            try {
                std::stringstream document;
                document << std::ifstream (localPath).rdbuf();
                res->end(document.str().data(), document.str().size());

            } catch (std::exception &ex) {
                spdlog::get("console")->error("WebDataServer/onHttpRequest: {0}",ex.what());
                res->end();
            }
            return;
        }
    });

    h.onMessage(
            [this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                spdlog::get("console")->debug("{0} {1} - Message",ws->getAddress().address,ws->getAddress().port);

                std::string sMessage=std::string(message,length);
                try {
                    nlohmann::json jMessage = nlohmann::json::parse(sMessage);
                    if (jMessage["context"] && jMessage["subscribe"]) {
                        jMessage["tag"]=getId();
                        auto *subscriptions = (std::vector<nlohmann::json *> *) (ws->getUserData());
                        auto *subscription = new nlohmann::json(jMessage);

                        pSignalKModel->getUpdateBus()->subscribe(*subscription, [ws](nlohmann::json update) {
                            spdlog::get("console")->debug("{0} - onUpdate", update.dump());
                            std::string updateString = update.dump();
                            ws->send(updateString.c_str(), updateString.size(), uWS::OpCode::TEXT);
                        });

                        subscriptions->push_back(subscription);
                    }
                } catch (nlohmann::detail::parse_error &exception) {
                    spdlog::get("console")->error("WebDataServer: {0}",exception.what());
                }
            });

    h.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest httpRequest) {
        std::string url = httpRequest.getUrl().toString();
        spdlog::get("console")->debug("{0} {1} {2} - Connected",ws->getAddress().address,ws->getAddress().port, url);

        if (url.find(SIGNALK_V1_STREAM)!=std::string::npos) {
            std::string hello = pSignalKModel->getHello().dump();
            ws->send(hello.c_str(), hello.size(), uWS::OpCode::TEXT);
            auto *subscriptions=new std::vector<nlohmann::json*>();
            ws->setUserData(subscriptions);

            nlohmann::json *subscription= nullptr;
            if (url.find("subscribe=all")!=std::string::npos) {
                subscription= new nlohmann::json(
                    {
                        {"_tag",getId()},
                        { "context", "*"},
                        { "subscribe", { "*" }}
                    }
                );

            } else
            if (url.find("subscribe=none")!=std::string::npos) {

            } else {
                subscription = new nlohmann::json(
                    {
                        {"_tag",getId()},
                        {"context",   "vessels." + pSignalKModel->getSelf()},
                        {"subscribe", {"*"}}
                    }
                );
            }


            if (subscription!= nullptr) {

                pSignalKModel->getUpdateBus()->subscribe(*subscription, [ws](nlohmann::json update) {
                    spdlog::get("console")->debug("{0} - onUpdate", update.dump());
                    std::string updateString = update.dump();
                    ws->send(updateString.c_str(), updateString.size(), uWS::OpCode::TEXT);
                });

                subscriptions->push_back(subscription);
                spdlog::get("console")->debug("subscription: {0}",subscription->dump(2));
            }

        } else {
            ws->terminate();
        }
    });

    h.onDisconnection([this](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
        auto *subscriptions = (std::vector<nlohmann::json*> *) (ws->getUserData());
        std::for_each(subscriptions->begin(),subscriptions->end(),[this](nlohmann::json *subscription) {
            pSignalKModel->getUpdateBus()->unsubscribe(*subscription);
            delete subscription;
        });
        delete subscriptions;

        ws->setUserData(nullptr);
        spdlog::get("console")->debug("{0} {1} - Disconnected",ws->getAddress().address,ws->getAddress().port);
    });

    std::thread tHeartBeat([this](){
        while(!isDone()) {
            std::string hello = pSignalKModel->getHello().dump();
            h.getDefaultGroup<uWS::SERVER>().broadcast(hello.c_str(), hello.size(), uWS::OpCode::TEXT);
            spdlog::get("console")->debug("WebDataServer/heartBeat: {0}",hello);
            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }
        h.getDefaultGroup<uWS::SERVER>().terminate();
    });

    spdlog::get("console")->debug("WebDataServer/Listening: ws://{0}:{1}",bind,port);

    h.listen(port);
    h.run();


    spdlog::get("console")->debug("WebDataServer/ENDED: ws://{0}:{1}",bind,port);
}

bool WebDataServer::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::vector<char> WebDataServer::readAllBytes(std::string filename)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    return result;
}