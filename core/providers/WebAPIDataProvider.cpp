//
// Created by Raffaele Montella on 20/09/2018.
//

#include "WebAPIDataProvider.hpp"

using namespace httplib;

WebAPIDataProvider::WebAPIDataProvider(std::string id,SignalK::SignalKModel *document, std::string bind, int port, std::string root) {
    this->type="providers/signalk/webapi/server";
    this->id=id;
    this->document=document;
    this->bind=bind;
    this->port=port;
    this->root=root;
}

WebAPIDataProvider::WebAPIDataProvider(std::string id, SignalK::SignalKModel *document, nlohmann::json options): WebAPIDataProvider(id,document,options["bind"],options["port"],options["root"]) {}


WebAPIDataProvider::~WebAPIDataProvider() {

}


void WebAPIDataProvider::run(){


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

    svr.set_base_dir(root.c_str());
    spdlog::get("console")->info("Listening: {0}:{1}",bind,port);
    svr.listen(bind.data(), port);

}