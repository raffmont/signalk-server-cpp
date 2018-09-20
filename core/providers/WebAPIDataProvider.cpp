//
// Created by Raffaele Montella on 20/09/2018.
//

#include "WebAPIDataProvider.hpp"

using namespace httplib;

WebAPIDataProvider::WebAPIDataProvider(std::string name,SignalK::SignalKModel *document, int port) {
    std::cout << "WebAPIDataProvider::WebAPIDataProvider(name,document,port)\n";
    this->name=name;
    this->document=document;
    this->port=port;
}
WebAPIDataProvider::~WebAPIDataProvider() {
    std::cout << "WebAPIDataProvider::~WebAPIDataProvider()\n";
}


void WebAPIDataProvider::run(){
    std::cout << "WebAPIDataProvider::run()\n";

    svr.Get("/signalk/v1/api/stream", [](const Request& req, Response& res) {
        res.set_content("Upgrade Required", "text/plain");
    });

    svr.Get("/signalk/v1/api/", [this](const Request& req, Response& res) {
        res.set_content(document->toJson(), "application/json");
    });

    // http://localhost:3000/signalk/v1/api/self
    svr.Get(R"(/signalk/v1/api/(.*))", [this](const Request& req, Response& res) {
        auto queryString = req.matches[1];
        std::string path=queryString;
        std::replace( path.begin(), path.end(), '/', '.');
        res.set_content(document->subtree(path), "application/json");
    });

    svr.listen("localhost", port);

}