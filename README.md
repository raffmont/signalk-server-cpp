# signalk-server-cpp
A C++ implementation of a SignalK server

This SignalK server (http://signalk.org) has been designed in order to be executed as Linux daemon.

Current status:
The signalk-server-cpp works as nmea to signalk converter and web socket server.

Usage:

    signalk-server-cpp uuid nmeafile

Where:

    uuid - The vessel uuid
    
    nmeafile - A NMEA0183 log file
    
When the signalk-server-cpp is running, read the signalk-server-node websocket:

wscat ws://localhost:3000/signalk/v1/stream?subscribe=all

