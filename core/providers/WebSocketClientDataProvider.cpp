//
// Created by Raffaele Montella on 06/09/2018.
//


#include "WebSocketClientDataProvider.hpp"



WebSocketClientDataProvider::WebSocketClientDataProvider(std::string id,SignalK::SignalKModel *document, int perms, std::string url) {
    this->id=id;
    this->type="providers/signalk/websocket/client";
    this->pSignalKModel=pSignalKModel;
    this->url=url;
}

WebSocketClientDataProvider::WebSocketClientDataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options): WebSocketClientDataProvider(id,document,options["perms"],options["url"]) {}


WebSocketClientDataProvider::~WebSocketClientDataProvider() {
}


void WebSocketClientDataProvider::run(){


    h.onMessage(
            [this](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                pSignalKModel->update(msg);
            });

    h.connect(url, nullptr);
    spdlog::get("console")->info("Connected to: {0}",url);
    h.run();

}