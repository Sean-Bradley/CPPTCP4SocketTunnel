# CPPTCP4SocketTunnel

A TCP 4 Socket Tunnel, to be used for creating multiple endpoints for a simple server or load balancing.

IF you start a socket server using my CPPMultiThreadedLowLatencyTCPSocketServer on one ip address,

and you want to hide that ip from the rest of the world, or create a load balancing solution,

you can start up a CPPTCP4Socket tunnel on another server, and expose that endpoint to your desired clients.


eg,
                                                                 / Client 1
                                              
                                 ---Tunnel 1 (192.168.1.2:8080) {   Client 2
                                 
                               /                                 \  Client 3
                               
                             /
                             
Server (192.169.1.1:8080)  {
                                     
                            \
                            
                             \                                  /  Client 4
                             
                              ---  Tunnel 2 (192.168.1.3:8080) {   Client 5
                              
                                                                \  Client 6
                                                                
                                                                
Have as many tunnels as you want.

Use what ever ports you want,

Actually, use the tunnel for what ever you want.


To compile on linux

g++ TCP4Tunnel.cpp -pthread -std=c++11 -o SeansTCP4Tunnel

to run

./SeansTCP4Tunnel [local listen port] [destination ip] [destination port]

test using either the supplied tcp client in the other repository, or simply just telnet


