#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <cstring>
#include <string>

class DS18B20
{
private:
    char path[50] = "/sys/bus/w1/devices/";
    char serialNumber[20];
    char buf[100];
    DIR* dirp;
    struct dirent* direntp;

public:
    int Mount();
    int Initialize();
    float GetTemperature();
};

