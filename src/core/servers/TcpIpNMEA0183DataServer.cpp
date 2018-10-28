//
// Created by Raffaele Montella on 14/10/2018.
//

#include <uvw/loop.hpp>
#include <uvw.hpp>
#include <marnav/nmea/sentence.hpp>
#include "TcpIpNMEA0183DataServer.hpp"

TcpIpNMEA0183DataServer::TcpIpNMEA0183DataServer(bool enabled,std::string id,SignalK::SignalKModel *pSignalKModel,  std::string bind, int port) {
    this->enabled=enabled;
    this->type="servers/nmea0183/tcpip";
    this->id=id;
    this->pSignalKModel=pSignalKModel;
    this->port=port;
}

TcpIpNMEA0183DataServer::TcpIpNMEA0183DataServer(bool enabled,std::string id,SignalK::SignalKModel *document, nlohmann::json options): TcpIpNMEA0183DataServer(enabled,id,document,options["bind"],options["port"]) {}

TcpIpNMEA0183DataServer::~TcpIpNMEA0183DataServer() {
}

void TcpIpNMEA0183DataServer::onRun(){
    auto loop = uvw::Loop::getDefault();

    std::shared_ptr<uvw::TcpHandle> tcp = loop->resource<uvw::TcpHandle>();

    tcp->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
        std::cout << "error " << std::endl;
    });

    tcp->once<uvw::ListenEvent>([this](const uvw::ListenEvent &, uvw::TcpHandle &srv) {
        std::cout << "listen" << std::endl;

        std::shared_ptr<uvw::TcpHandle> client = srv.loop().resource<uvw::TcpHandle>();

        client->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &, uvw::TcpHandle &) {
            std::cout << "error " << std::endl;
        });

        client->on<uvw::CloseEvent>([ptr = srv.shared_from_this()](const uvw::CloseEvent &, uvw::TcpHandle &) {
            std::cout << "close" << std::endl;
            ptr->close();
        });

        srv.accept(*client);

        uvw::Addr local = srv.sock();
        uvw::Addr remote = client->peer();

        /*
        std::shared_ptr<std::vector<nlohmann::json*>> uData=std::make_shared<std::vector<nlohmann::json*>>();
        client->data(uData);
        std::vector<nlohmann::json*> *subscriptions=(std::vector<nlohmann::json*> *)uData.get();
        nlohmann::json *subscription= new nlohmann::json(
                {
                        { "_tag", getId() },
                        { "context", "*"},
                        { "subscribe", { "*" }}
                }
        );
        pSignalKModel->getUpdateBus()->subscribe(*subscription, [client, this](nlohmann::json update) {
            spdlog::get("console")->debug("{0} - onUpdate", update.dump());
            std::string updateString = update.dump();
            std::vector<marnav::nmea::sentence *> sentences=makeSentences(update);
            std::for_each(sentences.begin(),sentences.end(),[client,this](marnav::nmea::sentence *sentence) {
                std::string sSentence=marnav::nmea::to_string(*sentence);
                client->tryWrite((char *)sSentence.c_str(),sSentence.length());
                delete sentence;
            });

        });

        subscriptions->push_back(subscription);
        */

        client->on<uvw::DataEvent>([this](const uvw::DataEvent &event, uvw::TcpHandle &handle) {

            std::string msg(event.data.get(), event.length);
            spdlog::get("console")->info("{0} {1} - Message",handle.sock().ip,handle.sock().port);

            std::string sMessage=std::string(event.data.get(), event.length);
            try {
                nlohmann::json jMessage = nlohmann::json::parse(sMessage);
                if (jMessage["context"] && jMessage["subscribe"]) {
                    jMessage["tag"]=getId();
                    auto uData=handle.data();
                    /*
                    std::vector<nlohmann::json*> *subscriptions=(std::vector<nlohmann::json*> *)uData.get();
                    nlohmann::json *subscription = new nlohmann::json(jMessage);

                    pSignalKModel->getUpdateBus()->subscribe(*subscription, [&handle](nlohmann::json update) {
                        spdlog::get("console")->debug("{0} - onUpdate", update.dump());
                        std::string updateString = update.dump();
                        char *pUpdate=new char[updateString.length()];
                        strcpy(pUpdate,updateString.c_str());
                        auto dataTryWrite = std::unique_ptr<char[]>(pUpdate);
                        handle.tryWrite(std::move(dataTryWrite), (unsigned int)updateString.length());
                        //delete[] pUpdate;
                        pUpdate= nullptr;

                    });

                    subscriptions->push_back(subscription);
                     */

                }
            } catch (nlohmann::detail::parse_error exception) {
                spdlog::get("console")->error("TcpIpSignalKDataServer: {0}",exception.what());
            }

        });

        client->on<uvw::EndEvent>([this](const uvw::EndEvent &, uvw::TcpHandle &handle) {

            auto uData=handle.data();
            /*
            std::vector<nlohmann::json*> *subscriptions=(std::vector<nlohmann::json*> *)uData.get();
            std::for_each(subscriptions->begin(),subscriptions->end(),[this](nlohmann::json *subscription) {
                pSignalKModel->getUpdateBus()->unsubscribe(*subscription);
                delete subscription;
            });
            delete subscriptions;
            */
            handle.data(nullptr);



            std::cout << "end" << std::endl;
            int count = 0;
            handle.loop().walk([&count](uvw::BaseHandle &) { ++count; });
            std::cout << "still alive: " << count << " handles" << std::endl;
            handle.close();
        });
        client->read();

    });

    tcp->once<uvw::CloseEvent>([](const uvw::CloseEvent &, uvw::TcpHandle &) {
        std::cout << "close" << std::endl;
    });

    /*
     *

     */

    spdlog::get("console")->info("TcpIpNMEA0183DataServer/Listening: {0}:{1}",bind,port);
    tcp->bind(bind, port);
    tcp->listen();
    loop->run();
    while(!isDone()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }
    loop->close();
    spdlog::get("console")->info("TcpIpNMEA0183DataServer/ENDED: {0}:{1}",bind,port);
}
