cmake_minimum_required(VERSION 3.5)
project(signalk_server_cpp)

set(CMAKE_CXX_STANDARD 11)


# Include the support to external projects
include(ExternalProject)

# Set the external install location
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

# Add to the includes
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)

# Add to the libraries
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)


# TODO: Add support for STATIC also.
find_package(OpenSSL REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories(${OPENSSL_INCLUDE_DIR})
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

#### json

ExternalProject_Add(json
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
        GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
        TIMEOUT 360
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND export PREFIX=${EXTERNAL_INSTALL_LOCATION}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        )







# Set the library name
set(LIBUV ${CMAKE_CURRENT_BINARY_DIR}/extras/libuv/libuv)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(LIBUV ${CMAKE_CURRENT_BINARY_DIR}/extras/libuv/libuv.dylib)
endif()


# Add the executables to the project
add_executable(
        ${PROJECT_NAME}
        main.cpp
        core/DataBase.h
        core/DataBase.cpp
        core/ReaderHandler.h
)


# Expicit the dependencies
add_dependencies(${PROJECT_NAME} json)
add_dependencies(${PROJECT_NAME} variant)
add_dependencies(uWebSockets libuv)
add_dependencies(${PROJECT_NAME} uWebSockets)

# Add the libraries to the linker
target_link_libraries(${PROJECT_NAME} ${LIBUV})

