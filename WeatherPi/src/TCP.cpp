#include "TCP.hpp"

std::string TCP::SendOverTCP(int port, std::string ip, std::string payload)
{
    std::string returnString = "";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        returnString = "Error creating socket";
        return returnString;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form 
    //if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)      
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        returnString = "Invalid address/ Address not supported";
        return returnString;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        returnString = "Connection Failed";
        return returnString;
    }

    send(sock, payload.c_str(), payload.length(), 0);
    valread = read(sock, buffer, 1024);

    returnString = buffer;

    close(sock);

    return returnString;
}