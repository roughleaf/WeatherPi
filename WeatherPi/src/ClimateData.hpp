#pragma once
#include "STIME.hpp"
#include <string>
class ClimateData
{
public:
	float BME280Temperature = 0;
	float BME280Pressure = 0;
	float DS18B20Temperature = 0;
	int BME280Humididty = 0;
	int RainCount = 0;
	int LightningCount = 0;
	int LightningDistance = 0;
	int NodeID = 0;
	std::string Date = "";
	std::string Time = "";

	void PopulateFromSensorNode(char* sensorNodeData);
	void PopulateFromLocal(char* localData);
	//void BuildJsonString(char* jsonString);
};

