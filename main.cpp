#include <stdio.h>
#include "core/DataBase.h"
#include "core/providers/WebSocketServerDataProvider.hpp"
#include "core/providers/WebSocketClientDataProvider.hpp"
#include "core/providers/FileNMEA0183DataProvider.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <time.h>
#include <chrono>
#include <thread>


using namespace std;


int main() {

    SignalK::DataBase *document=new SignalK::DataBase("urn:mrn:signalk:uuid:705f5f1a-efaf-44aa-9cb8-a0fd6305567c", "v1.0.0");
    string init = "{\"updates\":[{\"source\":{\"sentence\":\"GLL\",\"talker\":\"GP\",\"type\":\"NMEA0183\",\"label\":\"nmeaFromFile\"},\"timestamp\":\"2017-05-03T10:17:52.000Z\",\"values\":[{\"path\":\"navigation.position\",\"value\":{\"longitude\":13.505133333333333,\"latitude\":50.0541}}]}],\"context\":\"vessels.urn:mrn:signalk:uuid:c0d79334-4e25-4245-8892-54e8ccc8021d\"}";

    document->update(init);




    string filePath="/Users/raffaelemontella/Desktop/20060114.nmea";
    long millis=1000;
    string url="ws://localhost:3000/signalk/v1/stream?subscribe=all";
    int port=2000;

    WebSocketServerDataProvider p1("WebSocket Server",document,port);
    WebSocketClientDataProvider p2("WebSocket Client", document,url);
    FileNMEA0183DataProvider p3("NMEA0183 file reader",document,filePath,millis);

    std::vector<DataProvider *> dataProviders(std::thread::hardware_concurrency());
    dataProviders.push_back(&p1);
    dataProviders.push_back(&p2);
    dataProviders.push_back(&p3);
    cout << "------------------------\n";

    for (DataProvider *dataProvider :  dataProviders) {
        if (dataProvider != NULL) {
            std::cout << "Starting: " << dataProvider->getName() << "\n";
            dataProvider->start();
        }
    }

    for (DataProvider *dataProvider :  dataProviders){
        if (dataProvider != NULL) {
            std::cout << "Joining: " << dataProvider->getName() << "\n";
            dataProvider->join();
        }
    }

    delete(document);
    return 0;
}