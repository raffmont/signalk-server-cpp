//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebSocketDataServer.hpp"



WebSocketDataServer::WebSocketDataServer(SignalK::SignalKModel *pSignalKModel,  std::string bind, int port, std::string root) {
    this->pSignalKModel=pSignalKModel;
    this->port=port;
    this->root=root;
}

WebSocketDataServer::WebSocketDataServer(SignalK::SignalKModel *document, nlohmann::json options): WebSocketDataServer(document,options["bind"],options["port"],options["root"]) {}

WebSocketDataServer::~WebSocketDataServer() {
}


void WebSocketDataServer::run(){



    // httpRequest borde vara defaultsatt till att hantera upgrades, ta bort onupgrade! (sätter man request avsätts upgrade handlern)
    h.onHttpRequest([this](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t length, size_t remainingBytes) {
        std::string url = req.getUrl().toString();
        spdlog::get("console")->info("Request: {0}",url);
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
            std::string path=url;
            if (path=="/") {
                path="index.html";
            }
            std::ifstream t(root+"/"+path);
            std::string document((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            res->end((char *) document.c_str(), document.length());

            return;
        }
    });

    h.onMessage(
            [this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                spdlog::get("console")->info("{0} {1} - Message",ws->getAddress().address,ws->getAddress().port);

                std::string sMessage=std::string(message,length);
                try {
                    nlohmann::json jMessage = nlohmann::json::parse(sMessage);
                    if (jMessage["context"] && jMessage["subscribe"]) {
                        std::vector<nlohmann::json *> *subscriptions = (std::vector<nlohmann::json *> *) (ws->getUserData());
                        nlohmann::json *subscription = new nlohmann::json(sMessage);

                        pSignalKModel->getUpdateBus()->subscribe(*subscription, [ws](nlohmann::json update) {
                            spdlog::get("console")->debug("{0} - onUpdate", update.dump());
                            std::string updateString = update.dump();
                            ws->send(updateString.c_str(), updateString.size(), uWS::OpCode::TEXT);
                        });

                        subscriptions->push_back(subscription);
                    }
                } catch (nlohmann::detail::parse_error exception) {
                    spdlog::get("console")->error("WebSocketDataServer: {0}",exception.what());
                }
            });

    h.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest httpRequest) {
        std::string url = httpRequest.getUrl().toString();
        spdlog::get("console")->info("{0} {1} {2} - Connected",ws->getAddress().address,ws->getAddress().port, url);

        if (url.find(SIGNALK_V1_STREAM)!=std::string::npos) {
            std::string hello = pSignalKModel->getHello().dump();
            ws->send(hello.c_str(), hello.size(), uWS::OpCode::TEXT);
            std::vector<nlohmann::json*> *subscriptions=new std::vector<nlohmann::json*>();
            ws->setUserData(subscriptions);

            nlohmann::json *subscription= nullptr;
            if (url.find("subscribe=all")!=std::string::npos) {
                subscription= new nlohmann::json(
                    {
                         { "context", "*"},
                         { "subscribe", { "*" }}
                    }
                );

            } else
            if (url.find("subscribe=none")!=std::string::npos) {

            } else {
                subscription = new nlohmann::json(
                    {
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
                spdlog::get("console")->info("subscription: {0}",subscription->dump(2));
            }

        } else {
            ws->terminate();
        }
    });

    h.onDisconnection([this](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
        std::vector<nlohmann::json*> *subscriptions = (std::vector<nlohmann::json*> *) (ws->getUserData());
        std::for_each(subscriptions->begin(),subscriptions->end(),[this](nlohmann::json *subscription) {
            pSignalKModel->getUpdateBus()->unsubscribe(*subscription);
            delete subscription;
        });
        delete subscriptions;

        ws->setUserData(NULL);
        spdlog::get("console")->info("{0} {1} - Disconnected",ws->getAddress().address,ws->getAddress().port);
    });


    spdlog::get("console")->info("Listening: ws://{0}:{1}",bind,port);
    h.listen(port);
    h.run();

}

bool WebSocketDataServer::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}