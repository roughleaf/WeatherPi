#pragma once
#include "WeatherPiSystem.hpp"
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

void* udpNet(void* port);