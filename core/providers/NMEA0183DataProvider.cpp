//
// Created by Raffaele Montella on 11/09/2018.
//

#include "NMEA0183DataProvider.hpp"

using namespace marnav;

void NMEA0183DataProvider::parse(std::string line) {

    if (document!=NULL && line.empty()==false) {
        replace(line, "\n", "");
        replace(line, "\r", "");
        //std::cout <<  "NMEA0183DataProvider::parse:" << line << "\n";
        try {
            auto sentence = nmea::make_sentence(line);
            if (sentence->id() == nmea::sentence_id::ZDA) {
                auto zda = nmea::sentence_cast<nmea::zda>(sentence.get());
                utils::optional<nmea::time> utcTime = zda->get_time_utc();
                std::cout << "UTC:" << nmea::to_string(utcTime.value()) << "\n";
            } else if (sentence->id() == nmea::sentence_id::GLL) {
                auto gll = nmea::sentence_cast<nmea::gll>(sentence.get());
                //if (gll->get_data_valid()) {
                utils::optional<geo::longitude> longitude = gll->get_longitude();
                utils::optional<geo::latitude> latitude = gll->get_latitude();
                std::cout << "Pos:" << nmea::to_string(longitude.value()) << "," << nmea::to_string(latitude.value())
                          << "\n";
                //}
            }
        } catch (nmea::unknown_sentence ex) {
            std::cout <<  "NMEA0183DataProvider::parse:" << line << " --- unknown!\n";
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