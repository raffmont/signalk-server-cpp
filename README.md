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


