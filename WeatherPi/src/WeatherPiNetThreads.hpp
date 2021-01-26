#pragma once
#include "WeatherPiSystem.hpp"
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <thread>
#include <ctime>

void* udpNet(void* port);
void* tcpDataTransmitTimer(void* port);