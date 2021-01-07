#include "DS18B20.hpp"

int DS18B20::Mount()
{
    system("sudo modprobe w1-gpio");
    system("sudo modprobe w1-therm");

    return 0;
}

int DS18B20::Initialize()
{
    if ((dirp = opendir(path)) == NULL)
    {
        return 0;
    }
    // TODO: List all connected DS18B20 Devices.
    while ((direntp = readdir(dirp)) != NULL)
    {  
        if (strstr(direntp->d_name, "28-"))
        {
            strcpy(serialNumber, direntp->d_name);
        }
    }
    closedir(dirp);

    strcat(path, serialNumber);
    strcat(path, "/w1_slave");

    return 0;
}

float DS18B20::GetTemperature()
{

    int fd = -1;
    char* temp;
    float value;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        return 999.99f;
    }
    // Read the file
    if (read(fd, buf, sizeof(buf)) < 0)
    {
        return 999.99f;
    }
    // Returns the first index of 't'.
    temp = strchr(buf, 't');
    // Read the string following "t=".
    sscanf(temp, "t=%s", temp);
    // atof: changes string to float.
    value = (float)atof(temp) / 1000;

    return value;
}
