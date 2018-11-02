//
// Created by Raffaele Montella on 06/09/2018.
//

#include "WebDataServer.hpp"



WebDataServer::WebDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel,  std::string bind, int port, std::string root):SignalKDataServer(enabled,id,pSignalKModel) {
    this->type="servers/signalk/web";
    this->port=port;
    this->root=root;
}

WebDataServer::WebDataServer(bool enabled,std::string id,SignalK::SignalKModel *document, nlohmann::json options): WebDataServer(enabled,id,document,options["bind"],options["port"],options["root"]) {}

WebDataServer::~WebDataServer() {
}


void WebDataServer::onRun(){

    /*
     * 1. GET /signalk
     * {"endpoints":{"v1":{"version":"1.7.1","signalk-http":"http://localhost:3000/signalk/v1/api/","signalk-ws":"ws://localhost:3000/signalk/v1/stream","signalk-tcp":"tcp://localhost:3858"}},"server":{"id":"signalk-server-node","version":"1.7.1"}}
     *
     * 2. GET /signalk/v1/api/self
     * "vessels.urn:mrn:signalk:uuid:c0d79334-4e25-4245-8892-54e8ccc8021d"
     *
     * 3. GET /plugins
     * [..]
     *
     * 4. GET /signalk/v1/api/
     * {"vessels":
     */


    // httpRequest borde vara defaultsatt till att hantera upgrades, ta bort onupgrade! (sätter man request avsätts upgrade handlern)
    h.onHttpRequest([this](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t length, size_t remainingBytes) {
        std::string url = req.getUrl().toString();
        spdlog::get("console")->info("Request: {0}",url);

        // Check if the url is equals to /signalk/v1/stream
        if (url == SIGNALK_V1_STREAM) {

        } else if (url.find(SIGNALK_V1_API_SELF)!=std::string::npos) { // Check if the url is equal to /signalk/v1/api/
            // Perform the query on the SignalK document
            std::string signalk="\"vessels."+pSignalKModel->getSelf()+"\"";

            // End the request
            res->end((char *) signalk.c_str(), signalk.length());
            return;

        } else if (url.find(SIGNALK_V1_API)!=std::string::npos) { // Check if the url is equal to /signalk/v1/api/

            // Copy the url in path
            std::string path=url;

            // Erase the trailing URL
            path.erase(0,SIGNALK_V1_API.length());


            // Replace all / with .
            path=Utils::String::replace(path,"/",".");

            // Replace the self reference with the actual one
            path=Utils::String::replace(path,"vessels.self","vessels."+pSignalKModel->getSelf());

            // Perform the query on the SignalK document
            std::string signalk=pSignalKModel->subtree(path).dump();

            // End the request
            res->end((char *) signalk.c_str(), signalk.length());
            return;

        } else if (url==SIGNALK) { // Check if the url is equal to /signalk

            // Get the server hello message
            std::string signalk=pSignalKModel->getSignalK(bind,port).dump();

            // End the request
            res->end((char *) signalk.c_str(), signalk.length());
            return;
        } else if (url==PLUGINS) { // Check if the url is equal to /plugins

            // Get the server plugin manifest
            std::string plugins=pSignalKModel->getPlugins().dump();

            // End the request
            res->end((char *) plugins.c_str(), plugins.length());
            return;
        }else {
            // The url represents an internal resource
            spdlog::get("console")->info("WebDataServer/onHttpRequest: url:{0}",url);

            // Copy the path
            std::string path=url;

            // Check if the path is empty or is just the root
            if (path=="" || Utils::String::endsWith(path,"/")) {
                // Add to the path the default document
                path=path+"index.html";
            }

            // Compose the file system local path
            std::string localPath=root+"/"+path;

            spdlog::get("console")->info("WebDataServer/onHttpRequest: localPath:{0}",localPath);

            // Try to read the rosource
            try {
                std::stringstream document;

                // Read the resource
                document << std::ifstream (localPath).rdbuf();

                // End the request
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

                // Create a message string
                std::string sMessage(message, length);

                // Get the pointer to the client id
                auto *pTag = (std::string *) (ws->getUserData());

                // Show info on console
                spdlog::get("console")->debug("{0} {1} {2} - Message",*pTag,ws->getAddress().address,ws->getAddress().port);



                try {
                    // Try to parse the message
                    nlohmann::json jMessage = nlohmann::json::parse(sMessage);

                    // Manage the subscription
                    getSubscriptions()->parse(*pTag,jMessage,[ws](nlohmann::json update) {
                        std::string updateString = update.dump();

                        spdlog::get("console")->info("{0} - onUpdate", updateString);

                        ws->send(updateString.c_str(), updateString.size(), uWS::OpCode::TEXT);
                    });


                } catch (nlohmann::detail::parse_error &exception) {
                    spdlog::get("console")->error("WebDataServer: {0}",exception.what());
                }
            });

    h.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest httpRequest) {

        // Get the requested url
        std::string url = httpRequest.getUrl().toString();

        // Show on console
        spdlog::get("console")->info("{0} {1} {2} - Connected",ws->getAddress().address,ws->getAddress().port, url);

        if (url.find(SIGNALK_V1_STREAM)!=std::string::npos) {

            // Generate a random id
            auto *pTag=new std::string(getId()+"|"+Utils::String::generate_hex(16));

            // Set the id as user data
            ws->setUserData(pTag);

            // Check is it
            if (url.find("subscribe=none")==std::string::npos) {
                // Create a none subscription
                nlohmann::json jMessage;

                if (url.find("subscribe=all") != std::string::npos) {
                    jMessage = nlohmann::json::parse(
                            R"({"context": "*","subscribe": [{"path": "*" }]})");
                } else {
                    jMessage = nlohmann::json::parse(
                            R"({"context": "vessels.self","subscribe": [{"path": "*" }]})");
                }

                // Manage the subscription
                getSubscriptions()->parse(*pTag, jMessage, [ws](nlohmann::json update) {
                    std::string updateString = update.dump();

                    spdlog::get("console")->info("{0} - onUpdate", updateString);

                    ws->send(updateString.c_str(), updateString.size(), uWS::OpCode::TEXT);
                });
            }

        } else {
            ws->terminate();
        }
    });

    h.onDisconnection([this](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {

        // Get the pointer to the client id
        auto *pTag = (std::string *) (ws->getUserData());

        // Remove all subscriptions
        getSubscriptions()->removeAll(*pTag);

        // Delete the tag
        delete pTag;

        // Set the user data as null
        ws->setUserData(nullptr);

        // Show on console
        spdlog::get("console")->info("{0} {1} - Disconnected",ws->getAddress().address,ws->getAddress().port);
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



