//
// Created by Raffaele Montella on 11/09/2018.
//


#include "NMEA0183DataProvider.hpp"

using namespace marnav;
using json = nlohmann::json;

void NMEA0183DataProvider::parse(std::string line) {

    if (document!=NULL && line.empty()==false) {
        replace(line, "\n", "");
        replace(line, "\r", "");
        //std::cout <<  "NMEA0183DataProvider::parse:" << line << "\n";
        try {
            auto sentence = nmea::make_sentence(line);

            json source,update,updates;

            source["sentence"]="GLL";
            source["talker"]="II";
            source["type"]="NMEA0183";
            source["label"]="nmeaFromFile";

            update["source"]=source;
            update["timestamp"]="2017-05-03T10:17:52.000Z";
            updates["context"]="vessels."+document->getSelf();

            if (sentence->id() == nmea::sentence_id::ZDA) {
                auto zda = nmea::sentence_cast<nmea::zda>(sentence.get());
                utils::optional<nmea::time> utcTime = zda->get_time_utc();
                std::cout << "UTC:" << nmea::to_string(utcTime.value()) << "\n";

                




            } else if (sentence->id() == nmea::sentence_id::GLL) {
                auto gll = nmea::sentence_cast<nmea::gll>(sentence.get());
                //if (gll->get_data_valid()) {
                utils::optional<geo::longitude> longitude = gll->get_longitude();
                utils::optional<geo::latitude> latitude = gll->get_latitude();

                nlohmann::json value;
                value["path"]="navigation.position";
                value["value"]["longitude"]=longitude.value();
                value["value"]["latitude"]=latitude.value();
                update["values"]={ value };

                std::cout << "Pos:" << nmea::to_string(longitude.value()) << "," << nmea::to_string(latitude.value())
                          << "\n";
                //}
            } else if (sentence->id() == nmea::sentence_id::MWD) {

            } else if (sentence->id() == nmea::sentence_id::VTG) {

            } else if (sentence->id() == nmea::sentence_id::APB) {

            } else if (sentence->id() == nmea::sentence_id::BWC) {

            } else if (sentence->id() == nmea::sentence_id::DBT) {

            } else if (sentence->id() == nmea::sentence_id::GGA) {

            } else if (sentence->id() == nmea::sentence_id::HDG) {

            } else if (sentence->id() == nmea::sentence_id::HDM) {

            } else if (sentence->id() == nmea::sentence_id::HDT) {

            } else if (sentence->id() == nmea::sentence_id::RSA) {

            } else if (sentence->id() == nmea::sentence_id::MWV) {

            } else if (sentence->id() == nmea::sentence_id::VHW) {

            } else if (sentence->id() == nmea::sentence_id::VPW) {

            } else if (sentence->id() == nmea::sentence_id::VWR) {

            } else if (sentence->id() == nmea::sentence_id::VWT) {

            } else if (sentence->id() == nmea::sentence_id::XTE) {

            } else if (sentence->id() == nmea::sentence_id::GSA) {

            } else if (sentence->id() == nmea::sentence_id::GSV) {

            } else if (sentence->id() == nmea::sentence_id::MTW) {

            } else if (sentence->id() == nmea::sentence_id::HVM) {

            } else if (sentence->id() == nmea::sentence_id::ZTG) {

            }

            updates["updates"]={ update };
            document->update(updates);

        } catch (nmea::unknown_sentence ex) {
            std::cout <<  "nmea0183DataProvider::parse:" << line << " --- unknown!\n";
        }
    }
};

bool NMEA0183DataProvider::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}