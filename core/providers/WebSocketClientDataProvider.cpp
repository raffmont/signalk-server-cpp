//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebSocketClientDataProvider.hpp"



WebSocketClientDataProvider::WebSocketClientDataProvider(std::string name,SignalK::SignalKModel *document, std::string url) {
    std::cout << "WebSocketClientDataProvider::WebSocketClientDataProvider(name,document,url)\n";
    this->name=name;
    this->document=document;
    this->url=url;
}
WebSocketClientDataProvider::~WebSocketClientDataProvider() {
    std::cout << "WebSocketClientDataProvider::~WebSocketClientDataProvider()\n";
}


void WebSocketClientDataProvider::run(){
    std::cout << "WebSocketClientDataProvider::run()\n";


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
    h.run();

}