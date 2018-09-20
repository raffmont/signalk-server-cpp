#include <cstdio>
#include "core/SignalKModel.h"
#include "core/providers/WebSocketServerDataProvider.hpp"
#include "core/providers/WebSocketClientDataProvider.hpp"
#include "core/providers/FileNMEA0183DataProvider.hpp"
#include "core/providers/WebAPIDataProvider.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <ctime>
#include <chrono>
#include <thread>


using namespace std;


int main(int argc, char* argv[]) {

    if (argc < 3) { // We expect 3 arguments: the program name, the source path and the destination path
        std::cerr << "Usage: " << argv[0] << "uuid nmeafile" << std::endl;
        return 1;
    }

    std::string uuid=argv[1]; // "urn:mrn:signalk:uuid:705f5f1a-efaf-44aa-9cb8-a0fd6305567c"
    string filePath=argv[2]; //"/Users/raffaelemontella/Desktop/20060114.nmea"

    SignalK::SignalKModel *document=new SignalK::SignalKModel(uuid, "v1.0.0");
    //string init = "{\"updates\":[{\"source\":{\"sentence\":\"GLL\",\"talker\":\"GP\",\"type\":\"NMEA0183\",\"label\":\"nmeaFromFile\"},\"timestamp\":\"2017-05-03T10:17:52.000Z\",\"values\":[{\"path\":\"navigation.position\",\"value\":{\"longitude\":13.505133333333333,\"latitude\":50.0541}}]}],\"context\":\"vessels.urn:mrn:signalk:uuid:c0d79334-4e25-4245-8892-54e8ccc8021d\"}";

    //document->update(init);





    long millis=250;
    string url="http://demo.signalk.org/signalk/v1/stream?subscribe=all";
    int port=3000;

    WebSocketServerDataProvider p1("WebSocket Server",document,port);
    //WebSocketClientDataProvider p2("WebSocket Client", document,url);
    FileNMEA0183DataProvider p3("NMEA0183 file reader",document,filePath,millis);
    WebAPIDataProvider p4("API",document,3000);

    std::vector<DataProvider *> dataProviders(std::thread::hardware_concurrency());
    dataProviders.push_back(&p1);
    //dataProviders.push_back(&p2);
    dataProviders.push_back(&p3);
    dataProviders.push_back(&p4);
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