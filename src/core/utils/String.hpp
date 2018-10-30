//
// Created by Raffaele Montella on 30/10/2018.
//

#ifndef SIGNALK_SERVER_CPP_STRING_HPP
#define SIGNALK_SERVER_CPP_STRING_HPP


#include <string>
#include <random>
#include <sstream>

namespace Utils {

    class String {
    public:
        static bool endsWith(std::string const &fullString, std::string const &ending);

        static std::string replace(std::string subject, const std::string& search,
                                    const std::string& replace);

        static std::string generate_hex(const unsigned int len);

    private:
        static unsigned char random_char();
    };

}

#endif //SIGNALK_SERVER_CPP_STRING_HPP
