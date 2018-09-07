# signalk-server-cpp
A C++ implementation of a SignalK server

This SignalK server (http://signalk.org) has been designed in order to be executed as Linux daemon.

Current status:
The signalk-server-cpp for now works as websocket repeater.
It read from a signalk-server-node and repeat on its own websocket server.

1. Install and execute signalk-server-node
2. Run signalk-server-node with ./bin/nmea-from-file
3. Test it:

wscat ws://localhost:3000/signalk/v1/stream?subscribe=all

4. Compile and run signalk-server-coo
5. Read the signalk-server-node websocket

wscat ws://localhost:2000/signalk/v1/stream?subscribe=all

