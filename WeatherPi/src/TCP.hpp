#pragma once
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <iostream>

class TCP
{
private:
    int sock = 0;
    ssize_t valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };

public:
    std::string SendOverTCP(int port, std::string ip, std::string payload);
};