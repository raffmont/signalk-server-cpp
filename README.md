# signalk-server-cpp
A C++ implementation of a SignalK server

This SignalK server (http://signalk.org) has been designed in order to be executed as Linux daemon.

The primary reason for at least two implementations of a signalk server is to ensure we dont create a node application instead of a generic communication standard in signalk. This has already been avoided several times, as the node-server is quite RPi/web-browser oriented, resulting in node/npm/http dependencies creeping in to webapp deployments, IoT device requirements, and node specialised server-side requirements.

In a SignalK world there should be no dependency except on signalk message and API proocols, and all devices should be equal participants. If we dont achieve this we will just create another specialised application framework.

Current status:
The signalk-server-cpp works as nmea to signalk converter and web socket server.
It supports signalk over TcpIp.

Available providers:
1. providers/nmea0183/filestream - Reads a NMEA 0183 log file
2. providers/nmea0183/serial - Reads NMEA 0183 data from a serial port
3. providers/signalk/websocket - Reads SignalK data from another server

Usage:

    signalk-server-cpp settings.json

Where:

    settings.json - The configuration file


When the signalk-server-cpp is running, read the signalk-server-node websocket:

    wscat ws://localhost:3000/signalk/v1/stream?subscribe=all

The web interface is working at http://localhost:3000
The SignalK web API are available on http://localhost:3000/signalk :

    {
      "endpoints": {
        "v1": {
          "signalk-http": "http://localhost:3000/signalk/v1/api/",
          "signalk-ws": "ws://localhost:3000/signalk/v1/stream",
          "version": "v0.1.0"
        }
      },
      "server": {
        "id": "signalk-dynamo-server",
        "version": "v0.1.0"
      }
    }

Example of configuration file:

    {
      "vessel": {
        "name": "Sarima V",
        "brand": "XYachts",
        "type": "362 Sport",
        "uuid": "urn:mrn:signalk:uuid:705f5f1a-efaf-44aa-9cb8-a0fd6305567c",

        "dimensions": {
          "length": 10.72,
          "width": 3.48,
          "mast": 17,
          "depthTransducer": 0.5,
          "keel": 2.05
        }
      },

      "interfaces": {},

      "servers": [
        { "id":"signalk-web", "type":"servers/signalk/web", "enabled":true,
          "options":{
            "bind":"localhost",
            "port":3000,
            "root":"/Users/raffaelemontella/CLionProjects/signalk-server-cpp/www/"
          }
        },
        { "id":"signalk-tcpip", "type":"servers/signalk/tcpip", "enabled":true,
          "options":{
            "bind":"localhost",
            "port":55555
          }
        },
        { "id":"nmea0183-tcpip", "type":"servers/nmea0183/tcpip", "enabled":false,
          "options":{
            "bind":"localhost",
            "port":10110
          }
        }
      ],

      "providers": [
        { "id":"FileNMEA0183", "type":"providers/nmea0183/filestream", "enabled":true,
          "options":{
            "path":"/Users/raffaelemontella/CLionProjects/signalk-server-cpp/samples/SarimaV-20060114.nmea",
            "millis":250
          }
        },
        { "id":"SerialNMEA0183", "type":"providers/nmea0183/serial", "enabled":false,
          "options":{
            "device":"/dev/cu.usbserial",
            "baud":4800,
            "bytesize":8,
            "parity": "none",
            "stopbits": 1
          }
        }
      ]
    }

Important: remember to change the bind ip address in order to match your configuration.
It is, of course, possible to set it as localhost for development purposes, but this affect the service advertisement.


Android Build
-------------
The following was based on several online examples and tutorial, presented below:  
* [How to cross-compile an Android project with Cmake](https://github.com/taka-no-me/android-cmake)
* [How to configure the environment before compiling](https://github.com/libuv/libuv)
  - From here I have downloaded the files `android-configure-*`.
* [How to solve the problem with the OpenSSL library](https://scottyab.com/2017/05/how-to-build-and-integrate-openssl-into-your-android-ndk-project/)
  - About this, check the `CMakeLists.txt` file, since for the moment the path  
  to the downloaded `openssl` is hard-coded.

First, make sure you have downloaded the [Android NDK](https://developer.android.com/ndk/).  
Then, create a folder `build-android` and move into that. Use `source` to create the environment for the Android compiling. Use `cmake` to create the Makefile (for other ABIs check the above links):
```console
foo@bar:~$ export NDK_PATH=<path to the NDK installation>
foo@bar:~$ mkdir build-android
foo@bar:~$ cd build-android
foo@bar:~$ source ../android-configure-x86_64 ${NDK_PATH} 27
foo@bar:~$ ls
android-toolchain-x86_64
foo@bar:~$ cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
-DANDROID_NDK=${NDK_PATH}  \
-DCMAKE_BUILD_TYPE=Release \
-DANDROID_ABI="x86_64" ..
```

The compiling fails with the following errors:
```console
[.... OTHER MESSAGES ....]
CMake Warning at CMakeLists.txt:8 (find_package):
  By not providing "Findcatkin.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "catkin", but
  CMake did not find one.

  Could not find a package configuration file provided by "catkin" with any
  of the following names:

    catkinConfig.cmake
    catkin-config.cmake

  Add the installation prefix of "catkin" to CMAKE_PREFIX_PATH or set
  "catkin_DIR" to a directory containing one of the above files.  If "catkin"
  provides a separate development package or SDK, be sure it has been
  installed.


-- Configuring done
-- Generating done
-- Build files have been written to: /Users/sokol/git/signalk-server-cpp/build-android/serial-prefix/src/serial-build
[  6%] Performing build step for 'serial'
Scanning dependencies of target serial
[ 16%] Building CXX object CMakeFiles/serial.dir/src/serial.cc.o
[ 33%] Building CXX object CMakeFiles/serial.dir/src/impl/unix.cc.o
[ 50%] Building CXX object CMakeFiles/serial.dir/src/impl/list_ports/list_ports_osx.cc.o
[ 66%] Linking CXX static library libserial.a
warning: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib: warning for library: libserial.a the table of contents is empty (no object file members in the library define global symbols)
[ 66%] Built target serial
Scanning dependencies of target serial_example
[ 83%] Building CXX object CMakeFiles/serial_example.dir/examples/serial_example.cc.o
[100%] Linking CXX executable serial_example
/Users/sokol/git/signalk-server-cpp/build-android/android-toolchain-x86_64/bin/../lib/gcc/x86_64-linux-android/4.9.x/../../../../x86_64-linux-android/bin/ld: fatal error: -f/--auxiliary may not be used without -shared
clang70++: error: linker command failed with exit code 1 (use -v to see invocation)
make[5]: *** [serial_example] Error 1
make[4]: *** [CMakeFiles/serial_example.dir/all] Error 2
make[3]: *** [all] Error 2
make[2]: *** [serial-prefix/src/serial-stamp/serial-build] Error 2
make[1]: *** [CMakeFiles/serial.dir/all] Error 2
make: *** [all] Error 2
```
