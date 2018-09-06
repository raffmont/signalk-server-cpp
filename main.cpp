#include <stdio.h>
#include "core/DataBase.h"
#include "core/WebSocketServerDataProvider.hpp"
#include "core/WebSocketClientDataProvider.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <time.h>
#include <chrono>
#include <thread>


using namespace std;


int main() {

    SignalK::DataBase document=SignalK::DataBase("urn:mrn:signalk:uuid:705f5f1a-efaf-44aa-9cb8-a0fd6305567c", "v1.0.0");
    string init = "{\"updates\":[{\"source\":{\"sentence\":\"GLL\",\"talker\":\"GP\",\"type\":\"NMEA0183\",\"label\":\"nmeaFromFile\"},\"timestamp\":\"2017-05-03T10:17:52.000Z\",\"values\":[{\"path\":\"navigation.position\",\"value\":{\"longitude\":13.505133333333333,\"latitude\":50.0541}}]}],\"context\":\"vessels.urn:mrn:signalk:uuid:c0d79334-4e25-4245-8892-54e8ccc8021d\"}";

    document.update(init);


    //std::vector<DataProvider *> dataProviders(std::thread::hardware_concurrency());

    string url="ws://localhost:3000/signalk/v1/stream?subscribe=all";
    int port=2000;
    WebSocketServerDataProvider p1(document,port);
    //WebSocketClientDataProvider p2(document,url);
    p1.start();
    //p2.start();
    //p2.join();
    p1.join();

    return 0;
}