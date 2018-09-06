//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebSocketClientDataProvider.hpp"



WebSocketClientDataProvider::WebSocketClientDataProvider(SignalK::DataBase& document, std::string& url) {
    std::cout << "WebSocketClientDataProvider::WebSocketClientDataProvider(document,url)\n";
    this->document=document;
    this->url=url;
}
WebSocketClientDataProvider::~WebSocketClientDataProvider() {
    std::cout << "WebSocketClientDataProvider::~WebSocketClientDataProvider()\n";
}


void WebSocketClientDataProvider::run(){
    std::cout << "WebSocketClientDataProvider::run()\n";
    int times = 0;

    float processingTicks = 0;

    h.getDefaultGroup<uWS::CLIENT>().onMessage(
            [this](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                clock_t currTime;

                document.update(msg);


            });

    h.onDisconnection([this](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
        h.getDefaultGroup<uWS::SERVER>().close();

    });
    document.SubscribeUpdate([this](std::string x) {
        h.getDefaultGroup<uWS::SERVER>()
                .broadcast(x.c_str(), x.size(), uWS::OpCode::TEXT);
    });


    h.connect(url, nullptr);
    h.run();

}