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

        } else if (url.find(SIGNALK_V1_API)==0) {
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
            // why would the client send a new request at this point?
            //res->getHttpSocket()->terminate();
            std::string path=url;
            if (path=="/") {
                path="index.html";
            }
            std::ifstream t(root+"/"+path);
            std::string document((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            res->end((char *) document.c_str(), document.length());
            /*
            if (hasEnding(path,".html")) {
                std::string document((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                res->end((char *) document.c_str(), document.length());
            } else if (hasEnding(path,".png")) {
                std::string document((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                res->end((char *) document.c_str(), document.length());
            }
            */
            return;
        }
    });

    h.onMessage(
            [this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                spdlog::get("console")->info("{0} {1} - Message",ws->getAddress().address,ws->getAddress().port);
            });

    h.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest httpRequest) {
        spdlog::get("console")->info("{0} {1} - Connected",ws->getAddress().address,ws->getAddress().port);
        std::string hello = pSignalKModel->getHello().dump(2);
        ws->send(hello.c_str(), hello.size(), uWS::OpCode::TEXT);
    });

    h.onDisconnection([this](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
        //h.getDefaultGroup<uWS::SERVER>().close();
        spdlog::get("console")->info("{0} {1} - Disconnected",ws->getAddress().address,ws->getAddress().port);
    });


    pSignalKModel->SubscribeUpdate([this](std::string x) {
        h.getDefaultGroup<uWS::SERVER>()
                .broadcast(x.c_str(), x.size(), uWS::OpCode::TEXT);
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