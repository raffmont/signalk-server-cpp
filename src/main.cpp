#include <cstdio>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <ctime>
#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <uWS/uWS.h>

#include "core/model/SignalKModel.h"
#include "core/servers/WebDataServer.hpp"
#include "core/servers/TcpIpSignalKDataServer.hpp"
#include "core/providers/WebSocketClientDataProvider.hpp"
#include "core/providers/FileNMEA0183DataProvider.hpp"
#include "core/providers/DataProviders.hpp"

#include "SignalKServer.hpp"

using namespace std;


int main(int argc, char* argv[]) {
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");

    if (argc != 2) {
        err_logger->error("Usage: {0} settings.json",argv[0]);
        return 1;
    }

    std::ifstream t(argv[1]);
    std::string sSettings((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());
    nlohmann::json settings=nlohmann::json::parse(sSettings);

    console->info("Welcome to DYNAMO/FairWind SignalK Server! http://fairwind.uniparthenope.it");

    SignalKServer server(settings);
    server.run();

    return 0;
}