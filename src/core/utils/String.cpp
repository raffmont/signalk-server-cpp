//
// Created by Raffaele Montella on 30/10/2018.
//


#include "String.hpp"

namespace Utils {


    bool String::endsWith(std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }



    std::string String::replace(std::string subject, const std::string& search,
                                                     const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    unsigned char String::random_char() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        return static_cast<unsigned char>(dis(gen));
    }

    std::string String::generate_hex(const unsigned int len) {
        std::stringstream ss;
        for (auto i = 0; i < len; i++) {
            auto rc = random_char();
            std::stringstream hexstream;
            hexstream << std::hex << int(rc);
            auto hex = hexstream.str();
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }
        return ss.str();
    }
}