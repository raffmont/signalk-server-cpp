//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebSocketServerDataProvider.hpp"



WebSocketServerDataProvider::WebSocketServerDataProvider(std::string name,SignalK::SignalKModel *document, int port) {
    std::cout << "WebSocketServerDataProvider::WebSocketServerDataProvider(name,document,port)\n";
    this->name=name;
    this->document=document;
    this->port=port;
}
WebSocketServerDataProvider::~WebSocketServerDataProvider() {
    std::cout << "WebSocketServerDataProvider::~WebSocketServerDataProvider()\n";
}


void WebSocketServerDataProvider::run(){
    std::cout << "WebSocketServerDataProvider::run()\n";


    h.getDefaultGroup<uWS::CLIENT>().onMessage(
            [this](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode x) {
                std::string msg(message, length);
                clock_t currTime;

                document->update(msg);


            });

    h.onDisconnection([this](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
        h.getDefaultGroup<uWS::SERVER>().close();

    });


    document->SubscribeUpdate([this](std::string x) {
        h.getDefaultGroup<uWS::SERVER>()
                .broadcast(x.c_str(), x.size(), uWS::OpCode::TEXT);
    });

    std::cout << "Listening on port " << port << "\n";
    h.listen(port);
    h.run();

}