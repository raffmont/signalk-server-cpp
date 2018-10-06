cmake_minimum_required(VERSION 3.5)
project(signalk_server_cpp)

set(CMAKE_CXX_STANDARD 11)

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
        main.cpp
        core/model/SignalKModel.h
        core/model/SignalKModel.cpp
        core/model/ReaderHandler.h
        core/providers/DataProvider.cpp
        core/providers/DataProvider.hpp
        core/servers/WebSocketDataServer.cpp
        core/servers/WebSocketDataServer.hpp
        core/providers/WebSocketClientDataProvider.cpp
        core/providers/WebSocketClientDataProvider.hpp
        core/providers/NMEA0183DataProvider.cpp
        core/providers/NMEA0183DataProvider.hpp
        core/providers/SignalKDataProvider.cpp
        core/providers/SignalKDataProvider.hpp
        core/providers/FileNMEA0183DataProvider.cpp
        core/providers/FileNMEA0183DataProvider.hpp
        core/providers/DataProviders.cpp
        core/providers/DataProviders.hpp
        core/servers/DataServer.cpp
        core/servers/DataServer.hpp
        core/servers/SignalKDataServer.cpp
        core/servers/SignalKDataServer.hpp
        core/model/UpdateBus.cpp
        core/model/UpdateBus.hpp
        core/providers/SerialNMEA0183DataProvider.cpp
        core/providers/SerialNMEA0183DataProvider.hpp core/model/Node.cpp core/model/Node.hpp)


# Expicit the dependencies
add_dependencies(${PROJECT_NAME} serial)
add_dependencies(${PROJECT_NAME} spdlog)
add_dependencies(${PROJECT_NAME} marnav)
add_dependencies(${PROJECT_NAME} json)
add_dependencies(${PROJECT_NAME} variant)
add_dependencies(${PROJECT_NAME} libuv)
add_dependencies(uWebSockets libuv)
add_dependencies(${PROJECT_NAME} uWebSockets)


# Add the libraries to the linker
target_link_libraries(${PROJECT_NAME} ${LIBSERIAL} ${LIBMARNAV} ${LIBUV} ${LIBUWS} ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES})

