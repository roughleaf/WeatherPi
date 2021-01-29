#pragma once
#include <pigpio.h>
#include <fstream>
#include "BME280.hpp"
#include "SSD1306.hpp"
#include "DS18B20.hpp"
#include "AS3935.hpp"
#include "I2CLCD.hpp"
#include "Mono18pt7b.hpp"
#include "NRF24L01.hpp"
#include "STIME.hpp"
#include "Icodec.hpp"
#include "ClimateData.hpp"
#include "WeatherpiISR.hpp"
#include "TCP.hpp"

#define PORT 8080 
#define MAXLINE 1024

// All the global variables that will be used
extern STIME systemTime;
extern ClimateData nodeData[10];
extern AS3935 lightningDetector;
extern I2CLCD lcd;
extern NRF24L01 nrf24;
extern BME280 sensorBME280;
extern SSD1306 oledDisplay;
extern DS18B20 tempSensorDS18B20;
extern LightningData lightningData;
extern TCP tcp;
extern std::string ServerIP;
extern std::string MacAddress;

void WeatherPiSystemInitialize(void);
