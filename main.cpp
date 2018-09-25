#include <cstdio>
#include "core/SignalKModel.h"
#include "core/providers/WebSocketServerDataProvider.hpp"
#include "core/providers/WebSocketClientDataProvider.hpp"
#include "core/providers/FileNMEA0183DataProvider.hpp"
#include "core/providers/WebAPIDataProvider.hpp"
#include "core/providers/DataProviders.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <ctime>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


using namespace std;


int main(int argc, char* argv[]) {
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");

    if (argc < 1) {
        err_logger->error("Usage: {0} settings.json",argv[0]);
        return 1;
    }

    console->info("Welcome to DYNAMO/FairWomd SignalK Server!");

    std::ifstream t(argv[1]);
    std::string sSettings((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    nlohmann::json settings=nlohmann::json::parse(sSettings);

    std::string uuid=settings["vessel"]["uuid"];

    SignalK::SignalKModel *pSignalKModel=new SignalK::SignalKModel(uuid, "v1.0.0");

    DataProviders dataProviders(pSignalKModel,settings);
    dataProviders.run();

    delete(pSignalKModel);
    return 0;
}