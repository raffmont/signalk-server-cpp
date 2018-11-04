//
// Created by Raffaele Montella on 11/09/2018.
//


#include <iomanip>
#include "NMEA0183DataProvider.hpp"

using namespace marnav;
using json = nlohmann::json;

void NMEA0183DataProvider::parse(std::string label,std::string line) {
    //spdlog::get("console")->info("NMEA0183DataProvider::parse {0} {1}",label,line);
    if (pSignalKModel!= nullptr && !line.empty()) {
        replace(line, "\n", "");
        replace(line, "\r", "");
        try {
            try {

                try {
                    auto sentence = nmea::make_sentence(line);

                    //json update;
                    nlohmann::json updates = json::array();
                    //bool issue_update=false;

                    if (sentence->id() == nmea::sentence_id::ZDA) {
                        auto zda = nmea::sentence_cast<nmea::zda>(sentence.get());
                        utils::optional<nmea::time> utcTime = zda->get_time_utc();
                        //std::cout << "UTC:" << nmea::to_string(utcTime.value()) << "\n";

                    } else if (sentence->id() == nmea::sentence_id::GLL) {
                        auto gll = nmea::sentence_cast<nmea::gll>(sentence.get());
                        utils::optional<nmea::status> status=gll->get_data_valid();
                        if (status.available()) {
                            utils::optional<geo::longitude> longitude = gll->get_longitude();
                            utils::optional<geo::latitude> latitude = gll->get_latitude();

                            json updatePosition = makeUpdate(label,nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                            updatePosition["values"] = {
                                    {
                                            {"path", "navigation.position"},
                                            {"value", {
                                                              {"longitude", longitude.value().get()},
                                                              {"latitude", latitude.value().get()}
                                                      }
                                            }
                                    }
                            };
                            updates.push_back(updatePosition);
                        }
                    } else if (sentence->id() == nmea::sentence_id::RMC) {
                        auto rmc = nmea::sentence_cast<nmea::rmc>(sentence.get());
                        utils::optional<char> status=rmc->get_status();
                        if (status.available()) {
                            utils::optional<geo::longitude> longitude = rmc->get_longitude();
                            utils::optional<geo::latitude> latitude = rmc->get_latitude();

                            utils::optional<double> heading = rmc->get_heading();
                            utils::optional<double> sog = rmc->get_sog();
                            utils::optional<double> mag = rmc->get_mag();

                            json updatePosition = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                             nmea::to_string(sentence->id()));
                            updatePosition["values"] = {
                                    {
                                            {"path", "navigation.position"},
                                            {"value", {
                                                              {"longitude", longitude.value().get()},
                                                              {"latitude", latitude.value().get()}

                                                      }
                                            }
                                    }
                            };
                            updates.push_back(updatePosition);

                            json updateMagneticVariation = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                      nmea::to_string(sentence->id()));
                            updateMagneticVariation["values"] = {
                                    {
                                            {"path", "navigation.magneticVariation"},
                                            {"value", deg2rad(mag.value())
                                            }
                                    }
                            };
                            updates.push_back(updateMagneticVariation);

                            json updateCourseOverGroundTrue = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                         nmea::to_string(sentence->id()));
                            updateCourseOverGroundTrue["values"] = {
                                    {
                                            {"path", "navigation.courseOverGroundTrue"},
                                            {"value", deg2rad(heading.value())
                                            }
                                    }
                            };
                            updates.push_back(updateCourseOverGroundTrue);

                            json courseOverGroundMagnetic = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                       nmea::to_string(sentence->id()));
                            updateCourseOverGroundTrue["values"] = {
                                    {
                                            {"path", "navigation.courseOverGroundMagnetic"},
                                            {"value", deg2rad(heading.value() + mag.value())
                                            }
                                    }
                            };
                            updates.push_back(updateCourseOverGroundTrue);

                            json updateSpeedOverGround = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                    nmea::to_string(sentence->id()));
                            updateSpeedOverGround["values"] = {
                                    {
                                            {"path", "navigation.speedOverGround"},
                                            {"value", knots2ms(sog.value())
                                            }
                                    }
                            };
                            updates.push_back(updateSpeedOverGround);
                        }

                    } else if (sentence->id() == nmea::sentence_id::MWD) {
                        auto mwd = nmea::sentence_cast<nmea::mwd>(sentence.get());

                        utils::optional<double> directionMag=mwd->get_direction_mag();
                        utils::optional<double> directionTrue=mwd->get_direction_true();
                        utils::optional<double> speedKn=mwd->get_speed_kn();

                        json updateDirectionMagnetic = makeUpdate(label,nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                        updateDirectionMagnetic["values"] = {
                            {
                                {"path", "environment.wind.directionMagnetic"},
                                {"value", deg2rad(directionMag.value())}
                            }
                        };
                        updates.push_back(updateDirectionMagnetic);

                        json updateDirectionTrue = makeUpdate(label,nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                        updateDirectionTrue["values"] = {
                                {
                                        {"path", "environment.wind.directionTrue"},
                                        {"value", deg2rad(directionTrue.value())}
                                }
                        };
                        updates.push_back(updateDirectionTrue);

                        json updateSpeedTrue = makeUpdate(label,nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                        updateSpeedTrue["values"] = {
                                {
                                        {"path", "environment.wind.speedTrue"},
                                        {"value", knots2ms(speedKn.value())}
                                }
                        };
                        updates.push_back(updateSpeedTrue);

                    } else if (sentence->id() == nmea::sentence_id::VTG) {
                        auto vtg = nmea::sentence_cast<nmea::vtg>(sentence.get());

                        utils::optional<double> speedKn=vtg->get_speed_kn();
                        utils::optional<double> trackMagn=vtg->get_track_magn();
                        utils::optional<double> trackTrue=vtg->get_track_true();

                        json updateCourseOverGroundTrue = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                     nmea::to_string(sentence->id()));
                        updateCourseOverGroundTrue["values"] = {
                                {
                                        {"path", "navigation.courseOverGroundTrue"},
                                        {"value", deg2rad(trackTrue.value())
                                        }
                                }
                        };
                        updates.push_back(updateCourseOverGroundTrue);

                        json courseOverGroundMagnetic = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                   nmea::to_string(sentence->id()));
                        updateCourseOverGroundTrue["values"] = {
                                {
                                        {"path", "navigation.courseOverGroundMagnetic"},
                                        {"value", deg2rad(trackMagn.value())
                                        }
                                }
                        };
                        updates.push_back(updateCourseOverGroundTrue);

                        json updateSpeedOverGround = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                nmea::to_string(sentence->id()));
                        updateSpeedOverGround["values"] = {
                                {
                                        {"path", "navigation.speedOverGround"},
                                        {"value", knots2ms(speedKn.value())
                                        }
                                }
                        };

                        updates.push_back(updateSpeedOverGround);

                    } else if (sentence->id() == nmea::sentence_id::APB) {

                    } else if (sentence->id() == nmea::sentence_id::BWC) {

                    } else if (sentence->id() == nmea::sentence_id::DBT) {
                        auto dbt = nmea::sentence_cast<nmea::dbt>(sentence.get());

                        utils::optional<double > depthM = dbt->get_depth_meter();

                        json updateBelowTransducer = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                nmea::to_string(sentence->id()));
                        updateBelowTransducer["values"] = {
                                {
                                        {"path", "environment.depth.belowTransducer"},
                                        {"value", depthM.value()
                                        }
                                }
                        };

                        updates.push_back(updateBelowTransducer);

                    } else if (sentence->id() == nmea::sentence_id::GGA) {
                        auto gga = nmea::sentence_cast<nmea::gga>(sentence.get());
                        //if (gga->get_data_valid()) {
                        utils::optional<geo::longitude> longitude = gga->get_longitude();
                        utils::optional<geo::latitude> latitude = gga->get_latitude();
                        utils::optional<double> altitude = gga->get_altitude();

                        json updatePosition = makeUpdate(label,nmea::to_string(sentence->get_talker()), nmea::to_string(sentence->id()));
                        updatePosition["values"] = {
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
                        updates.push_back(updatePosition);

                    } else if (sentence->id() == nmea::sentence_id::HDG) {
                        auto hdg = nmea::sentence_cast<nmea::hdg>(sentence.get());

                        utils::optional<double> magnVar=hdg->get_magn_var();
                        utils::optional<nmea::direction> magnVarHem=hdg->get_magn_var_hem();

                        if (magnVarHem.value()==nmea::direction::east) {
                            magnVar=magnVar.value()*-1;
                        }

                        json updateMagneticVariation = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                nmea::to_string(sentence->id()));
                        updateMagneticVariation["values"] = {
                                {
                                        {"path", "navigation.magneticVariation"},
                                        {"value", deg2rad(magnVar.value()) }
                                }
                        };

                        updates.push_back(updateMagneticVariation);

                    } else if (sentence->id() == nmea::sentence_id::HDM) {
                        auto hdm = nmea::sentence_cast<nmea::hdm>(sentence.get());

                        utils::optional<double> headingMagn=hdm->get_heading();

                        json updateHeadingMagnetic = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                nmea::to_string(sentence->id()));
                        updateHeadingMagnetic["values"] = {
                            {
                                {"path", "navigation.headingMagnetic"},
                                {"value", deg2rad(headingMagn.value()) }
                            }
                        };

                        updates.push_back(updateHeadingMagnetic);

                    } else if (sentence->id() == nmea::sentence_id::HDT) {
                        auto hdt = nmea::sentence_cast<nmea::hdt>(sentence.get());

                        utils::optional<double> headingTrue=hdt->get_heading();

                        json updateHeadingTrue = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                nmea::to_string(sentence->id()));
                        updateHeadingTrue["values"] = {
                                {
                                        {"path", "navigation.headingTrue"},
                                        {"value", deg2rad(headingTrue.value()) }
                                }
                        };

                        updates.push_back(updateHeadingTrue);

                    } else if (sentence->id() == nmea::sentence_id::RSA) {
                        auto rsa = nmea::sentence_cast<nmea::rsa>(sentence.get());

                        utils::optional<nmea::status> status1=rsa->get_rudder1_valid();
                        if (status1.available()) {

                            utils::optional<double> rudder1=rsa->get_rudder1();

                            json updateRudderAngle1 = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                               nmea::to_string(sentence->id()));
                            updateRudderAngle1["values"] = {
                                    {
                                            {"path", "steering.rudderAngle"},
                                            {"value", deg2rad(rudder1.value()) }
                                    }
                            };

                            updates.push_back(updateRudderAngle1);
                        }


                    } else if (sentence->id() == nmea::sentence_id::MWV) {
                        auto mwv = nmea::sentence_cast<nmea::mwv>(sentence.get());

                        utils::optional<double> angle = mwv->get_angle();
                        utils::optional<nmea::reference> angleRef = mwv->get_angle_ref();
                        utils::optional<double> speed = mwv->get_speed();
                        utils::optional<nmea::unit::velocity> speedUnit = mwv->get_speed_unit();

                        // Convert 0-360 in -180,0,180
                        if (angle.value()>180) {
                            angle=180-angle.value();
                        }

                        // Convert speed in meters per seconds
                        if (speedUnit.value()==nmea::unit::velocity::knot) {
                            speed=knots2ms(speed.value());
                        } else if (speedUnit.value()==nmea::unit::velocity::kmh) {
                            speed=kmh2ms(speed.value());
                        }

                        if (angleRef.value() == nmea::reference::RELATIVE) {

                            json updateAngleApparent = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                  nmea::to_string(sentence->id()));

                            updateAngleApparent["values"] = {
                                    {
                                            {"path", "environment.wind.angleApparent"},
                                            {"value", deg2rad(angle.value())}
                                    }
                            };

                            updates.push_back(updateAngleApparent);

                            json updateSpeedApparent = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                  nmea::to_string(sentence->id()));

                            updateSpeedApparent["values"] = {
                                    {
                                            {"path", "environment.wind.speedApparent"},
                                            {"value", knots2ms(speed.value())}
                                    }
                            };

                            updates.push_back(updateSpeedApparent);
                        } else if (angleRef.value() == nmea::reference::TRUE) {

                            json updateAngleTrueWater = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                  nmea::to_string(sentence->id()));

                            updateAngleTrueWater["values"] = {
                                    {
                                            {"path", "environment.wind.angleTrueWater"},
                                            {"value", deg2rad(angle.value())}
                                    }
                            };

                            updates.push_back(updateAngleTrueWater);

                            json updateSpeedTrue = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                  nmea::to_string(sentence->id()));

                            updateSpeedTrue["values"] = {
                                    {
                                            {"path", "environment.wind.speedApparent"},
                                            {"value", knots2ms(speed.value())}
                                    }
                            };

                            updates.push_back(updateSpeedTrue);
                        }



                    } else if (sentence->id() == nmea::sentence_id::VHW) {
                        auto vhw = nmea::sentence_cast<nmea::vhw>(sentence.get());

                        utils::optional<double> heading = vhw->get_heading();
                        utils::optional<nmea::reference > degreesMag=vhw->get_degrees_mag();
                        utils::optional<nmea::reference > degreesTrue=vhw->get_degrees_true();
                        utils::optional<double> speedKn = vhw->get_speed_knots();

                        /*
                        json updateHeadingTrue= makeUpdate(nmea::to_string(sentence->get_talker()),
                                                                nmea::to_string(sentence->id()));

                        updateHeadingTrue["values"] = {
                                {
                                        {"path", "navigation.headingTrue"},
                                        {"value", deg2rad(heading.value())}
                                }
                        };

                        updates.push_back(updateHeadingTrue);
                         */

                        json updateHeadingMagnetic = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                                  nmea::to_string(sentence->id()));

                        updateHeadingMagnetic["values"] = {
                                {
                                        {"path", "navigation.headingMagnetic"},
                                        {"value", deg2rad(heading.value())}
                                }
                        };

                        updates.push_back(updateHeadingMagnetic);


                        json updateSpeedThroughWater = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                          nmea::to_string(sentence->id()));

                        updateSpeedThroughWater["values"] = {
                                {
                                        {"path", "navigation.speedThroughWater"},
                                        {"value", knots2ms(speedKn.value())}
                                }
                        };

                        updates.push_back(updateSpeedThroughWater);

                    } else if (sentence->id() == nmea::sentence_id::VPW) {
                        auto vpw = nmea::sentence_cast<nmea::vpw>(sentence.get());

                        utils::optional<double> speedKn=vpw->get_speed_knots();

                    } else if (sentence->id() == nmea::sentence_id::VWR) {
                        auto vwr = nmea::sentence_cast<nmea::vwr>(sentence.get());

                        utils::optional<double> speedKn=vwr->get_speed_knots();
                        utils::optional<double> angle=vwr->get_angle();
                        utils::optional<nmea::side > angleSide=vwr->get_angle_side();

                        if (angleSide.value()==nmea::side::left) {
                            angle=-1*angle.value();
                        }

                        json updateAngleApparent = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                              nmea::to_string(sentence->id()));

                        updateAngleApparent["values"] = {
                                {
                                        {"path", "environment.wind.angleApparent"},
                                        {"value", deg2rad(angle.value())}
                                }
                        };

                        updates.push_back(updateAngleApparent);

                        json updateSpeedApparent = makeUpdate(label,nmea::to_string(sentence->get_talker()),
                                                              nmea::to_string(sentence->id()));

                        updateSpeedApparent["values"] = {
                                {
                                        {"path", "environment.wind.speedApparent"},
                                        {"value", knots2ms(speedKn.value())}
                                }
                        };

                        updates.push_back(updateSpeedApparent);


                    } /*else if (sentence->id() == nmea::sentence_id::VWT) {
                        auto vwt = nmea::sentence_cast<nmea::vwt>(sentence.get());

                    }*/ else if (sentence->id() == nmea::sentence_id::XTE) {

                    } else if (sentence->id() == nmea::sentence_id::GSA) {

                    } else if (sentence->id() == nmea::sentence_id::GSV) {

                    } else if (sentence->id() == nmea::sentence_id::MTW) {

                    } else if (sentence->id() == nmea::sentence_id::HVM) {

                    } else if (sentence->id() == nmea::sentence_id::ZTG) {

                    }

                    if (updates.size() > 0) {


                        std::string vesselSelf = "vessels." + pSignalKModel->getSelf();
                        json delta = {{"context", vesselSelf},
                                      {"updates", updates}};

                        pSignalKModel->update(delta);

                        //std::cout << delta.dump(4) << "\n";
                    }
                    // marnav::nmea::checksum_error
                } catch (std::exception ex3) {
                    //spdlog::get("console")->error("NMEA0183DataProvider: {0} {1}",ex3.what(),line);
                }
            } catch (std::invalid_argument ex1) {
                spdlog::get("console")->error("NMEA0183DataProvider: {0} {1}",ex1.what(),line);
            }

        } catch (nmea::unknown_sentence ex2) {
            spdlog::get("console")->error("NMEA0183DataProvider: {0} {1}",ex2.what(),line);
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



nlohmann::json NMEA0183DataProvider::makeUpdate(std::string label, std::string talker, std::string id) {
    json update  = {
            {"source",    {
                                  {"sentence", id},
                                  {"talker", talker},
                                  {"type", "NMEA0183"},
                                  {"label", label}
                          }
            },
            {"timestamp", pSignalKModel->currentISO8601TimeUTC()}
        };
    return update;
}