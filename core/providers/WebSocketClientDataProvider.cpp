//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebSocketClientDataProvider.hpp"



WebSocketClientDataProvider::WebSocketClientDataProvider(std::string id,SignalK::SignalKModel *document, int perms, std::string url) {
    this->id=id;
    this->type="providers/signalk/websocket/client";
    this->document=document;
    this->url=url;
}

WebSocketClientDataProvider::WebSocketClientDataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options): WebSocketClientDataProvider(id,document,options["perms"],options["url"]) {}


WebSocketClientDataProvider::~WebSocketClientDataProvider() {
}


void WebSocketClientDataProvider::run(){


    h.getDefaultGroup<uWS::CLIENT>().onMessage(
            [this](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                document->update(msg);
            });

    h.onDisconnection([this](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
        h.getDefaultGroup<uWS::SERVER>().close();

    });
    document->SubscribeUpdate([this](std::string x) {
        h.getDefaultGroup<uWS::SERVER>()
                .broadcast(x.c_str(), x.size(), uWS::OpCode::TEXT);
    });


    h.connect(url, nullptr);
    std::cout << "Connecte to " << url << "\n";
    spdlog::get("console")->info("Connected to: {0}",url);
    h.run();

}