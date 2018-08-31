#include <stdio.h>
#include "core/DataBase.h"
#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <time.h>
#include <chrono>
#include <thread>


using namespace std;
using namespace uWS;

int main() {
    int max = 100;//Server
    int period = 1;//Server

    int clients = 1;//Client
    bool ourServer = true;//Client

    uWS::Hub h;

    SignalK::DataBase * pdb3 = new SignalK::DataBase("urn:mrn:signalk:uuid:705f5f1a-efaf-44aa-9cb8-a0fd6305567c", "v1.0.0");
    string init = "{\"updates\":[{\"source\":{\"sentence\":\"GLL\",\"talker\":\"GP\",\"type\":\"NMEA0183\",\"label\":\"nmeaFromFile\"},\"timestamp\":\"2017-05-03T10:17:52.000Z\",\"values\":[{\"path\":\"navigation.position\",\"value\":{\"longitude\":13.505133333333333,\"latitude\":50.0541}}]}],\"context\":\"vessels.urn:mrn:signalk:uuid:c0d79334-4e25-4245-8892-54e8ccc8021d\"}";

    pdb3->update(init);


    int times = 0;

    float processingTicks = 0;

    h.getDefaultGroup<uWS::CLIENT>().onMessage([pdb3, &times, max, period, &processingTicks](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode x) {
        string msg(message, length);
        clock_t currTime;

        pdb3->update(msg);


    });

    h.onDisconnection([pdb3, &processingTicks, &times, &h](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
        h.getDefaultGroup<uWS::SERVER>().close();

    });
    pdb3->SubscribeUpdate([&h](std::string x) {
        h.getDefaultGroup<uWS::SERVER>()
                .broadcast(x.c_str(), x.size(), uWS::OpCode::TEXT);
    });
    h.connect("ws://localhost:3000/signalk/v1/stream?subscribe=all", nullptr);
    h.listen(2000);

    h.run();

    delete pdb3;
    return 0;

    return 0;
}