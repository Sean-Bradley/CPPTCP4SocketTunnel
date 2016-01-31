/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016 Sean Bradley
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

 * Author: Sean Bradley
 * Created on 31 January 2016, 08:43
 */

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <thread>
#include <chrono>
#include <list>

using namespace std;

int ClientCount = 0;
std::list<int> clientList;
char* DestinationIP;
char* LocalListenPort;
char* DestinationPort;

void *destinationResponseHandler(void *client_desc, void *destination_desc) {
    int ClientFD = *(int*) client_desc;
    int DestinationFD = *(int*) destination_desc;
    int numbytes;
    char buf[256];
    memset(buf, 0, 256);
    //send(DestinationFD, "get\0", 5, 0);
    while (0 < (numbytes = recv(DestinationFD, buf, sizeof buf, 0))) {
        buf[numbytes] = '\0';
        cout << "<- : " << numbytes << " " << buf << endl;
        send(ClientFD, buf, strlen(buf), 0);
    }
    memset(buf, 0, 256);
}

void *clientConnectionHandler(void *socket_desc) {
    int ClientFD = *(int*) socket_desc;
    int DestinationFD;
    struct sockaddr_in Destination;
    DestinationFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    Destination.sin_family = AF_INET;
    Destination.sin_port = htons(atoi(DestinationPort));
    Destination.sin_addr.s_addr = inet_addr(DestinationIP);
    connect(DestinationFD, (struct sockaddr *) &Destination, sizeof Destination);
    std::thread destinationResponseThread(destinationResponseHandler, (void*) &ClientFD, (void*) &DestinationFD);
    destinationResponseThread.detach();

    int numbytes;
    char buf[256];
    memset(buf, 0, 256);
    while (0 < (numbytes = recv(ClientFD, buf, sizeof buf, 0))) {
        buf[numbytes] = '\0';
        cout << "-> :" << numbytes << " " << buf << endl;
        send(DestinationFD, buf, sizeof buf, 0);
        memset(buf, 0, 256);
    }
    close(DestinationFD);
    close(ClientFD);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " LocalListeningPort DestinationIPAddress DestinationPort" << std::endl;
        return 1;
    }
    LocalListenPort = argv[1];
    DestinationIP = argv[2];
    DestinationPort = argv[3];
    char buf[256];

    //create the server socket
    int ListenSocket;
    struct sockaddr_in saServer;

    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == ListenSocket) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    saServer.sin_family = AF_INET;
    saServer.sin_addr.s_addr = INADDR_ANY;
    saServer.sin_port = htons(atoi(LocalListenPort));

    if (-1 == bind(ListenSocket, (struct sockaddr *) &saServer, sizeof (saServer))) {
        perror("Error : Bind Failed");
        close(ListenSocket);
        exit(EXIT_FAILURE);
    }
    if (-1 == listen(ListenSocket, 10)) {
        perror("Error : Listen Failed");
        close(ListenSocket);
        exit(EXIT_FAILURE);
    }
    cout << "Server listening on port " << ntohs(saServer.sin_port) << endl;

    while (1) {
        int NewClient = accept(ListenSocket, NULL, NULL);
        std::thread clientThread(clientConnectionHandler, (void*) &NewClient);
        clientThread.detach();
    }
    return 0;
}
