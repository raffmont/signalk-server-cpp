//
// Created by Raffaele Montella on 31/08/2018.
//

#ifndef SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
#define SIGNALK_SERVER_CPP_DATAPROVIDER_HPP



#include "../DataThread.hpp"


class DataProvider : public DataThread {

public:
    DataProvider();
    DataProvider(bool enabled, std::string id,SignalK::SignalKModel *pSignalKModel);
    DataProvider(bool enabled, std::string id,SignalK::SignalKModel *pSignalKModel, nlohmann::json options);
    virtual ~DataProvider();
};

#endif //SIGNALK_SERVER_CPP_DATAPROVIDER_HPP
