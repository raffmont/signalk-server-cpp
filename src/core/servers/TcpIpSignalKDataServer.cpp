//
// Created by Raffaele Montella on 07/10/2018.
//




#include "TcpIpSignalKDataServer.hpp"

TcpIpSignalKDataServer::TcpIpSignalKDataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel,  std::string bind, int port) :SignalKDataServer(enabled,id,pSignalKModel){
    this->type="servers/signalk/tcpip";
    this->bind=bind;
    this->port=port;
}

TcpIpSignalKDataServer::TcpIpSignalKDataServer(bool enabled,std::string id,SignalK::SignalKModel *document, nlohmann::json options): TcpIpSignalKDataServer(enabled,id,document,options["bind"],options["port"]) {}

TcpIpSignalKDataServer::~TcpIpSignalKDataServer() {
}



void TcpIpSignalKDataServer::onRun(){
    spdlog::get("console")->info("TcpIpSignalKDataServer/onRun: {0}:{1}",bind,port);

    // Crate the loop
    auto loop = uvw::Loop::getDefault();

    // Create the tcp handler
    std::shared_ptr<uvw::TcpHandle> tcp = loop->resource<uvw::TcpHandle>();

    // Tcp on error event...
    tcp->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
        std::cout << "error " << std::endl;
    });

    // Tcp on listen event...
    tcp->on<uvw::ListenEvent>([this](const uvw::ListenEvent &, uvw::TcpHandle &srv) {
        spdlog::get("console")->info("onRun:tcp->once:ListenEvent/START");

        // Create the client handler
        std::shared_ptr<uvw::TcpHandle> client = srv.loop().resource<uvw::TcpHandle>();


        // On error event...
        client->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &event, uvw::TcpHandle &handle) {
            spdlog::get("console")->info("onRun:client->on:ErrorEvent --:",event.what());
        });


        // On close event...
        client->on<uvw::CloseEvent>([ptr = srv.shared_from_this()](const uvw::CloseEvent &, uvw::TcpHandle &handle) {
            spdlog::get("console")->info("onRun:client->on:CloseEvent");
            ptr->close();
        });


        // On end event
        client->on<uvw::EndEvent>([this](const uvw::EndEvent &, uvw::TcpHandle &handle) {

            std::shared_ptr<std::string> uData=handle.data<std::string>();
            std::string tag=*uData.get();

            spdlog::get("console")->info("onRun:client->on:EndEvent");

            // Remove all subscriptions
            getSubscriptions()->removeAll(tag);

            // Set the handle data to null
            handle.data(nullptr);

            // Close the handle
            handle.close();
        });

        // On data event...
        client->on<uvw::DataEvent>([this](const uvw::DataEvent &event, uvw::TcpHandle &handle) {



            std::string sMessage(event.data.get(), event.length);

            std::shared_ptr<std::string> uData=handle.data<std::string>();
            std::string tag=*uData.get();

            spdlog::get("console")->info("onRun:client->on:DataEvent -- Message: {0} {1} {2} -",handle.sock().ip,handle.sock().port,tag);

            try {
                // Try to parse the message
                nlohmann::json jMessage = nlohmann::json::parse(sMessage);

                // Manage the subscription
                getSubscriptions()->parse(tag,jMessage,[&handle](nlohmann::json update) {
                    std::string updateString = update.dump();

                    //spdlog::get("console")->info("TcpIpSignalKDataServer/onUpdate: {0}", updateString);

                    handle.tryWrite((char *)updateString.c_str(), static_cast<unsigned int>(updateString.length()));
                });

            } catch (nlohmann::detail::parse_error &exception) {
                spdlog::get("console")->error("TcpIpSignalKDataServer: {0}",exception.what());
            }



        });

        srv.accept(*client);

        uvw::Addr local = srv.sock();
        uvw::Addr remote = client->peer();

        // Set the id as user data
        std::shared_ptr<std::string> uData=std::make_shared<std::string>(getId()+"|"+Utils::String::generate_hex(16));

        client->data(uData);

        client->read();
        spdlog::get("console")->info("onRun:tcp->on:ListenEvent/END");





    });


    // On tcp close event
    tcp->once<uvw::CloseEvent>([](const uvw::CloseEvent &, uvw::TcpHandle &handle) {
        spdlog::get("console")->info("onRun:tcp->once:CloseEvent");
        int count = 0;
        handle.loop().walk([&count](uvw::BaseHandle &) { ++count; });
        std::cout << "still alive: " << count << " handles" << std::endl;
        handle.close();
    });


    spdlog::get("console")->info("TcpIpSignalKDataServer/Bind: {0}:{1}",bind,port);
    tcp->bind(bind, port);
    spdlog::get("console")->info("TcpIpSignalKDataServer/Listen: {0}:{1}",bind,port);
    tcp->listen();
    spdlog::get("console")->info("onRun:tcp->once:ListenEvent/tHeartBeat-pre");
    std::thread tHeartBeat([this,tcp](){
        while(!isDone()) {
            std::string hello = pSignalKModel->getHello().dump();
            spdlog::get("console")->info("TcpIpSignalKDataServer/heartbeat: {0}",hello);
            int count = 0;
            tcp->loop().walk([&count,hello](uvw::BaseHandle &handle) {

                //handle.tryWrite((char *)hello.c_str(),hello.length());


                count++;
            });
            spdlog::get("console")->info("TcpIpSignalKDataServer/heartbeat: count {0}",count);

            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }
        tcp->close();
    });
    spdlog::get("console")->info("onRun:tcp->once:ListenEvent/tHeartBeat-post");
    spdlog::get("console")->info("TcpIpSignalKDataServer/Run: {0}:{1}",bind,port);
    loop->run();
    spdlog::get("console")->info("TcpIpSignalKDataServer/Close: {0}:{1}",bind,port);
    loop->close();
    spdlog::get("console")->info("TcpIpSignalKDataServer/ENDED: {0}:{1}",bind,port);
}
