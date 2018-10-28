cmake_minimum_required(VERSION 3.5)
project(signalk_server_cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: https://developer.android.com/ndk/guides/cmake



# Include the support to external projects
include(ExternalProject)

# Set the external install location
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

# Add to the includes
include_directories(SYSTEM ${EXTERNAL_INSTALL_LOCATION}/include)

# Add to the libraries
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)




# TODO: Add support for STATIC also.
#find_package(OpenSSL REQUIRED)
set(OPENSSL_USE_STATIC_LIBS TRUE)
include(FindOpenSSL)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

find_package( ZLIB REQUIRED )




#### serial

ExternalProject_Add(serial
        GIT_REPOSITORY https://github.com/raffmont/serial.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        TIMEOUT 360
        )

#### spdlog

ExternalProject_Add(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        TIMEOUT 360
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )



#### marnav

ExternalProject_Add(marnav
        GIT_REPOSITORY https://github.com/raffmont/marnav.git
        CMAKE_ARGS -DENABLE_STATIC=OFF -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC=ON -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        TIMEOUT 360
        )



#### json

ExternalProject_Add(json
        GIT_TAG v3.2.0
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        TIMEOUT 360
        )

#### variant

ExternalProject_Add(variant
        GIT_REPOSITORY https://github.com/mpark/variant.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        TIMEOUT 360
        )

#### libuv

ExternalProject_Add(libuv
        GIT_REPOSITORY https://github.com/libuv/libuv.git
        GIT_TAG v1.x
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        TIMEOUT 360
        )


####

ExternalProject_Add(uvw
        GIT_REPOSITORY https://github.com/skypjack/uvw.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        TIMEOUT 360
        )

#### uWebSockets

ExternalProject_Add(uWebSockets
        GIT_REPOSITORY https://github.com/raffmont/uWebSockets.git
        TIMEOUT 360
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND echo Noghing
        BUILD_COMMAND make PREFIX=${EXTERNAL_INSTALL_LOCATION} CPPFLAGS=-I${EXTERNAL_INSTALL_LOCATION}/include/ CFLAGS=-I${OPENSSL_INCLUDE_DIR}
        INSTALL_COMMAND make PREFIX=${EXTERNAL_INSTALL_LOCATION} install
        )


# Set the library name
set(LIBSERIAL ${EXTERNAL_INSTALL_LOCATION}/lib/libserial.a)
#if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#    set(LIBUV ${EXTERNAL_INSTALL_LOCATION}/lib/libuv.dylib)
#endif()

# Set the library name
#set(LIBUV ${EXTERNAL_INSTALL_LOCATION}/lib/libuv)
#if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#    set(LIBUV ${EXTERNAL_INSTALL_LOCATION}/lib/libuv.dylib)
#endif()
set(LIBUV ${EXTERNAL_INSTALL_LOCATION}/lib/libuv_a.a)

# Set the library name
#set(LIBUWS ${EXTERNAL_INSTALL_LOCATION}/lib/libuWS)
#if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#    set(LIBUWS ${EXTERNAL_INSTALL_LOCATION}/lib/libuWS.dylib)
#endif()
set(LIBUWS ${EXTERNAL_INSTALL_LOCATION}/lib/libuWS.a)

# Set the library name
#set(LIBMARNAV ${EXTERNAL_INSTALL_LOCATION}/lib/libmarnav)
#if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#    set(LIBMARNAV ${EXTERNAL_INSTALL_LOCATION}/lib/libmarnav.dylib)
#endif()
set(LIBMARNAV ${EXTERNAL_INSTALL_LOCATION}/lib/libmarnav.a)


# Add the executables to the project
add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/core/model/SignalKModel.h
        src/core/model/SignalKModel.cpp
        src/core/model/ReaderHandler.h
        src/core/providers/DataProvider.cpp
        src/core/providers/DataProvider.hpp
        src/core/servers/WebDataServer.cpp
        src/core/servers/WebDataServer.hpp
        src/core/providers/WebSocketClientDataProvider.cpp
        src/core/providers/WebSocketClientDataProvider.hpp
        src/core/providers/NMEA0183DataProvider.cpp
        src/core/providers/NMEA0183DataProvider.hpp
        src/core/providers/SignalKDataProvider.cpp
        src/core/providers/SignalKDataProvider.hpp
        src/core/providers/FileNMEA0183DataProvider.cpp
        src/core/providers/FileNMEA0183DataProvider.hpp
        src/core/providers/DataProviders.cpp
        src/core/providers/DataProviders.hpp
        src/core/servers/DataServer.cpp
        src/core/servers/DataServer.hpp
        src/core/servers/SignalKDataServer.cpp
        src/core/servers/SignalKDataServer.hpp
        src/core/model/UpdateBus.cpp
        src/core/model/UpdateBus.hpp
        src/core/providers/SerialNMEA0183DataProvider.cpp
        src/core/providers/SerialNMEA0183DataProvider.hpp src/core/model/Node.cpp src/core/model/Node.hpp src/core/servers/TcpIpSignalKDataServer.cpp src/core/servers/TcpIpSignalKDataServer.hpp src/SignalKServer.cpp src/SignalKServer.hpp src/core/DataThread.cpp src/core/DataThread.hpp src/core/servers/DataServers.cpp src/core/servers/DataServers.hpp src/core/servers/TcpIpNMEA0183DataServer.cpp src/core/servers/TcpIpNMEA0183DataServer.hpp src/core/servers/NMEA0183DataServer.cpp src/core/servers/NMEA0183DataServer.hpp src/core/DataThreads.cpp src/core/DataThreads.hpp src/core/model/Subscription.cpp src/core/model/Subscription.hpp src/core/model/Subscriptions.cpp src/core/model/Subscriptions.hpp)


# Expicit the dependencies
add_dependencies(${PROJECT_NAME} serial)
add_dependencies(${PROJECT_NAME} spdlog)
add_dependencies(${PROJECT_NAME} marnav)
add_dependencies(${PROJECT_NAME} json)
add_dependencies(${PROJECT_NAME} variant)
add_dependencies(${PROJECT_NAME} libuv)
add_dependencies(uWebSockets libuv)
add_dependencies(${PROJECT_NAME} uWebSockets)
add_dependencies(uvw libuv)
add_dependencies(${PROJECT_NAME} uvw)



# Add the libraries to the linker
target_link_libraries(${PROJECT_NAME} ${LIBSERIAL} ${LIBMARNAV} ${LIBUV} ${LIBUWS} ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES})

