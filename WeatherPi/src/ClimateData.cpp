#include "ClimateData.hpp"

void ClimateData::PopulateFromSensorNode(char* sensorNodeData)
{
	typedef union
	{
		float floatNumber;
		int intNumber;
		uint8_t bytes[4];
	} FLOATINTUNION_t;

	FLOATINTUNION_t fromByte;

	char date[9] = { sensorNodeData[2]+48, sensorNodeData[3] + 48, '/', sensorNodeData[4] + 48, sensorNodeData[5] + 48, '/', sensorNodeData[6] + 48, sensorNodeData[7] + 48, '\0' };
	Date = date;

	char time[9] = { sensorNodeData[8] + 48, sensorNodeData[9] + 48, ':', sensorNodeData[10] + 48, sensorNodeData[11] + 48, ':', sensorNodeData[12] + 48, sensorNodeData[13] + 48, '\0' };
	Time = time;

	fromByte.bytes[0] = sensorNodeData[14];
	fromByte.bytes[1] = sensorNodeData[15];
	fromByte.bytes[2] = sensorNodeData[16];
	fromByte.bytes[3] = sensorNodeData[17];
	BME280Temperature = fromByte.floatNumber;

	fromByte.bytes[0] = sensorNodeData[19];
	fromByte.bytes[1] = sensorNodeData[20];
	fromByte.bytes[2] = sensorNodeData[21];
	fromByte.bytes[3] = sensorNodeData[22];
	BME280Pressure = fromByte.floatNumber;

	fromByte.bytes[0] = sensorNodeData[23];
	fromByte.bytes[1] = sensorNodeData[24];
	fromByte.bytes[2] = sensorNodeData[25];
	fromByte.bytes[3] = sensorNodeData[26];
	DS18B20Temperature = fromByte.floatNumber;

	fromByte.bytes[0] = sensorNodeData[27];
	fromByte.bytes[1] = sensorNodeData[28];
	fromByte.bytes[2] = sensorNodeData[29];
	fromByte.bytes[3] = sensorNodeData[30];
	RainCount = fromByte.intNumber;

	NodeID = (int)sensorNodeData[1];
	BME280Humididty = (int)sensorNodeData[18];
}

void ClimateData::PopulateFromLocal(float bmeTemp, float bmePressure, int bmeHumidity, float ds18b20Temp)
{
	Date = systemTime.GetSystemDate();
	Time = systemTime.GetSystemTime();

	BME280Temperature = bmeTemp;
	BME280Pressure = bmePressure;
	BME280Humididty = bmeHumidity;
	DS18B20Temperature = ds18b20Temp;
}

std::string ClimateData::UdpReturnString(void)
{
	return std::to_string(BME280Temperature) + ',' 
		+ std::to_string(BME280Humididty) + ',' 
		+ std::to_string(BME280Pressure) + ',' 
		+ std::to_string(DS18B20Temperature);
}
