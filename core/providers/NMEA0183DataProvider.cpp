//
// Created by Raffaele Montella on 11/09/2018.
//


#include <iomanip>
#include "NMEA0183DataProvider.hpp"

using namespace marnav;
using json = nlohmann::json;

void NMEA0183DataProvider::parse(std::string line) {

    if (document!=NULL && line.empty()==false) {
        replace(line, "\n", "");
        replace(line, "\r", "");
        //std::cout <<  "NMEA0183DataProvider::parse:" << line << "\n";
        try {
            try {
                auto sentence = nmea::make_sentence(line);

                //json update;
                nlohmann::json updates = json::array();
                //bool issue_update=false;

                if (sentence->id() == nmea::sentence_id::ZDA) {
                    auto zda = nmea::sentence_cast<nmea::zda>(sentence.get());
                    utils::optional<nmea::time> utcTime = zda->get_time_utc();
                    std::cout << "UTC:" << nmea::to_string(utcTime.value()) << "\n";

                } else if (sentence->id() == nmea::sentence_id::GLL) {
                    auto gll = nmea::sentence_cast<nmea::gll>(sentence.get());
                    //if (gll->get_data_valid()) {
                    utils::optional<geo::longitude> longitude = gll->get_longitude();
                    utils::optional<geo::latitude> latitude = gll->get_latitude();

                    json update = makeUpdate(nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                    update["values"] = {
                            {
                                    {"path", "navigation.position"},
                                    {"value", {
                                                      {"longitude", longitude.value().get()},
                                                      {"latitude", latitude.value().get()}
                                              }
                                    }
                            }
                    };
                    updates.push_back(update);


                    //std::cout << "Pos:" << nmea::to_string(longitude.value()) << "," << nmea::to_string(latitude.value())
                    //          << "\n";

                    //}
                } else if (sentence->id() == nmea::sentence_id::MWD) {

                } else if (sentence->id() == nmea::sentence_id::VTG) {

                } else if (sentence->id() == nmea::sentence_id::APB) {

                } else if (sentence->id() == nmea::sentence_id::BWC) {

                } else if (sentence->id() == nmea::sentence_id::DBT) {

                } else if (sentence->id() == nmea::sentence_id::GGA) {
                    auto gga = nmea::sentence_cast<nmea::gga>(sentence.get());
                    //if (gga->get_data_valid()) {
                    utils::optional<geo::longitude> longitude = gga->get_longitude();
                    utils::optional<geo::latitude> latitude = gga->get_latitude();
                    utils::optional<double> altitude = gga->get_altitude();

                    json update = makeUpdate(nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                    update["values"] = {
                            {
                                    {"path", "navigation.position"},
                                    {"value", {
                                                      {"longitude", longitude.value().get()},
                                                      {"latitude", latitude.value().get()},
                                                      {"altitude", altitude.value()}
                                              }
                                    }
                            }
                    };
                    updates.push_back(update);

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

                if (updates.size() > 0) {


                    std::string vesselSelf = "vessels." + document->getSelf();
                    json delta = {{"context", vesselSelf},
                                  {"updates", updates}};

                    document->update(delta);

                    //std::cout << delta.dump(4) << "\n";
                }
            } catch (std::invalid_argument ex1) {
                std::cout << "++++++++++++++++++++" << line << "\n" << ex1.what() << "\n";
            }

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

/**
 * Generate a UTC ISO8601-formatted timestamp
 * and return as std::string
 */
std::string NMEA0183DataProvider::currentISO8601TimeUTC() {
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(gmtime(&itt), "%FT%TZ");
    return ss.str();
}

nlohmann::json NMEA0183DataProvider::makeUpdate(std::string talker, std::string id) {
    json update  = {
            {"source",    {
                                  {"sentence", id},
                                  {"talker", talker},
                                  {"type", "NMEA0183"},
                                  {"label", "nmeaFromFile"}
                          }
            },
            {"timestamp", currentISO8601TimeUTC()}
        };
    return update;
}