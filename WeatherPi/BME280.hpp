#pragma once
#include <pigpio.h>

class BME280
{

public: struct SensorData
	{
		double temperature = 0.0;
		double humididty = 0.0;
		double pressure = 0.0;
	};

private:

	struct SensorRawData
	{
		long temperature = 0.0;
		unsigned long humididty = 0.0;
		unsigned long pressure = 0.0;
	};

	SensorRawData resultRaw;

	int handle = -1;

	bool tempMeasureEnabled = true;
	bool humidityMeasureEnabled = true;
	bool pressureMeasureEnabled = true;

	unsigned short digT1 = 0;
	signed short digT2 = 0;
	signed short digT3 = 0;

	signed long t_fine = 0;

	unsigned short digP1 = 0;
	signed short digP2 = 0;
	signed short digP3 = 0;
	signed short digP4 = 0;
	signed short digP5 = 0;
	signed short digP6 = 0;
	signed short digP7 = 0;
	signed short digP8 = 0;
	signed short digP9 = 0;
	
	unsigned char digH1 = 0;
	signed short digH2 = 0;
	unsigned char digH3 = 0;
	signed short digH4 = 0;
	signed short digH5 = 0;
	signed char digH6 = 0;

	int GetStatus();
	int GetCalibrateData();
	int GetSensorData(void);
	double CompensateTemp(const signed long tempADC);
	double CompensatePressure(const unsigned long pressADC);
	double CompensateHumidity(const unsigned long humADC);

public:

	SensorData result;

	const unsigned char pressureSensorDisable = 0x00;
	const unsigned char pressureOversampleX1 = 0x01;
	const unsigned char pressureOversampleX2 = 0x02;
	const unsigned char pressureOversampleX4 = 0x03;
	const unsigned char pressureOversampleX8 = 0x04;
	const unsigned char pressureOversampleX16 = 0x05;
	const unsigned char temperatureSensorDisable = 0x00;
	const unsigned char temperatureOversamplingX1 = 0x01;
	const unsigned char temperatureOversamplingX2 = 0x02;
	const unsigned char temperatureOversamplingX4 = 0x03;
	const unsigned char temperatureOversamplingX8 = 0x04;
	const unsigned char temperatureOversamplingX16 = 0x05;
	const unsigned char sensorSleepMode = 0x00;
	const unsigned char sensorForcedMode = 0x01;
	const unsigned char sensorNormalMode = 0x03;

	const unsigned char configStandby0_5ms = 0x00;
	const unsigned char configStandby62_5ms = 0x01;
	const unsigned char configStandby125ms = 0x02;
	const unsigned char configStandby250ms = 0x03;
	const unsigned char configStandby500ms = 0x04;
	const unsigned char configStandby1000ms = 0x05;
	const unsigned char configStandby10ms = 0x06;
	const unsigned char configStandby20ms = 0x07;
	const unsigned char configFilterOff = 0x00;
	const unsigned char configFilter2 = 0x01;
	const unsigned char configFilter4 = 0x02;
	const unsigned char configFilter8 = 0x03;
	const unsigned char configFilter16 = 0x04;

	const unsigned char humiditySensorDisable = 0x00;
	const unsigned char humidityOversamplingX1 = 0x01;
	const unsigned char humidityOversamplingX2 = 0x02;
	const unsigned char humidityOversamplingX4 = 0x03;
	const unsigned char humidityOversamplingX8 = 0x04;
	const unsigned char humidityOversamplingX16 = 0x05;

	int Initialize(const unsigned char i2cAddr);
	int Initialize(const unsigned char i2cBus, const unsigned char i2cAddr);
	int Close(void);
	int GetDeviceID(void);
	int SetConfig(const unsigned char config);
	int SetConfig(const unsigned char standBy, const unsigned char filter);
	int SetCtrlMeas(const unsigned char ctrlMeas);
	int SetCtrlMeas(const unsigned char temperatureOversampling, const unsigned char pressureOversampling, const unsigned char mode);
	int SetCtrlHum(const int humididtyOversampling);
	int GetAllResults(void);
	double GetTemperature(void);
	double GetPressure(void);
	double GetHumidity(void);
	bool StatusMeasuringBusy(void);
	bool ImUpdateBusy(void);
	int Reset(void);
};

