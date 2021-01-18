#pragma once
#include "STIME.hpp"
#include <string>
class ClimateData
{
private:
	STIME systemTime;

	// LightningCount and LightningDistance must be private and only be updatable through class method.
	// Reason being the LightningCount will be used to keep track of the LightningDisatnce element in the array
	int LightningCount = 0;
	// Will initially be able to store 100 strikes in a 10 minute period.
	// I'll allow Lightning count to run past 100 in lightning season to determine a reasonable array size.
	int LightningDistance[100] = { 0 };		
	
public:	
	float BME280Temperature = 0;
	float BME280Pressure = 0;
	float DS18B20Temperature = 0;
	int BME280Humididty = 0;
	int RainCount = 0;
	int NodeID = 0;
	std::string Date = "";
	std::string Time = "";

	void PopulateFromSensorNode(char* sensorNodeData);
	void PopulateFromLocal(float bmeTemp, float bmePressure, int bmeHumidity, float ds18b20Temp);
	void AddLightningStrike(int distance);
	std::string UdpReturnString(void);
	std::string BuildJsonString(void);
};

