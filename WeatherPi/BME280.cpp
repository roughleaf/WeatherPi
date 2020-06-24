#include "BME280.hpp"

int BME280::GetStatus()
{
	return i2cReadByteData(handle, 0xF3);
}

int BME280::GetCalibrateData()
{
	//============================== Temperature Calibration Data ===========================
	digT1 = static_cast<unsigned short>(i2cReadWordData(handle, 0x88));
	digT2 = static_cast<signed short>(i2cReadWordData(handle, 0x8A));
	digT3 = static_cast<signed short>(i2cReadWordData(handle, 0x8C));
	//=======================================================================================
	//============================== Pressure Calibration Data ==============================
	digP1 = static_cast<unsigned short>(i2cReadWordData(handle, 0x8E));
	digP2 = static_cast<signed short>(i2cReadWordData(handle, 0x90));
	digP3 = static_cast<signed short>(i2cReadWordData(handle, 0x92));
	digP4 = static_cast<signed short>(i2cReadWordData(handle, 0x94));
	digP5 = static_cast<signed short>(i2cReadWordData(handle, 0x96));
	digP6 = static_cast<signed short>(i2cReadWordData(handle, 0x98));
	digP7 = static_cast<signed short>(i2cReadWordData(handle, 0x9A));
	digP8 = static_cast<signed short>(i2cReadWordData(handle, 0x9C));
	digP9 = static_cast<signed short>(i2cReadWordData(handle, 0x9E));
	//=======================================================================================
	//============================== Humidity Calibration Data ==============================
	digH1 = static_cast<unsigned char>(i2cReadByteData(handle, 0xA1));
	digH2 = static_cast<signed short>(i2cReadWordData(handle, 0xE1));
	digH3 = static_cast<unsigned char>(i2cReadByteData(handle, 0xE3));
	char digH4Msb = static_cast<char>(i2cReadByteData(handle, 0xE4));
	char digH4H5Shared = static_cast<char>(i2cReadByteData(handle, 0xE5));			//this register hold parts of the values of dig_H4 and dig_h5
	char digH5Msb = static_cast<char>(i2cReadByteData(handle, 0xE6));
	digH6 = static_cast<signed char>(i2cReadByteData(handle, 0xE7));

	digH4 = static_cast<signed short>(digH4Msb << 4 | (digH4H5Shared & 0x0F));			//split and shift the bits appropriately.
	digH5 = static_cast<signed short>(digH5Msb << 4 | ((digH4H5Shared & 0xF0) >> 4));	//split and shift the bits appropriately.
	//=======================================================================================

	return 0;
}

double BME280::CompensateTemp(const signed long tempADC)
{
	double var1;
	double var2;
	double temperature;
	double temperature_min = -40;
	double temperature_max = 85;

	var1 = ((double)tempADC) / 16384.0 - ((double)digT1) / 1024.0;
	var1 = var1 * ((double)digT2);
	var2 = (((double)tempADC) / 131072.0 - ((double)digT1) / 8192.0);
	var2 = (var2 * var2) * ((double)digT3);
	t_fine = (int32_t)(var1 + var2);
	temperature = (var1 + var2) / 5120.0;
	if (temperature < temperature_min)
	{
		temperature = temperature_min;
	}
	else if (temperature > temperature_max)
	{
		temperature = temperature_max;
	}

	return temperature;
}

double BME280::CompensatePressure(const unsigned long pressADC)
{
	double var1;
	double var2;
	double var3;
	double pressure;
	double pressure_min = 30000.0;
	double pressure_max = 110000.0;

	var1 = ((double)t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double)digP6) / 32768.0;
	var2 = var2 + var1 * ((double)digP5) * 2.0;
	var2 = (var2 / 4.0) + (((double)digP4) * 65536.0);
	var3 = ((double)digP3) * var1 * var1 / 524288.0;
	var1 = (var3 + ((double)digP2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double)digP1);

	/* avoid exception caused by division by zero */
	if (var1)
	{
		pressure = 1048576.0 - (double)pressADC;
		pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
		var1 = ((double)digP9) * pressure * pressure / 2147483648.0;
		var2 = pressure * ((double)digP8) / 32768.0;
		pressure = pressure + (var1 + var2 + ((double)digP7)) / 16.0;
		if (pressure < pressure_min)
		{
			pressure = pressure_min;
		}
		else if (pressure > pressure_max)
		{
			pressure = pressure_max;
		}
	}
	else /* Invalid case */
	{
		pressure = pressure_min;
	}

	return pressure;
}

double BME280::CompensateHumidity(const unsigned long humADC)
{
	double humidity;
	double humidity_min = 0.0;
	double humidity_max = 100.0;
	double var1;
	double var2;
	double var3;
	double var4;
	double var5;
	double var6;

	var1 = ((double)t_fine) - 76800.0;
	var2 = (((double)digH4) * 64.0 + (((double)digH5) / 16384.0) * var1);
	var3 = humADC - var2;
	var4 = ((double)digH2) / 65536.0;
	var5 = (1.0 + (((double)digH3) / 67108864.0) * var1);
	var6 = 1.0 + (((double)digH6) / 67108864.0) * var1 * var5;
	var6 = var3 * var4 * (var5 * var6);
	humidity = var6 * (1.0 - ((double)digH1) * var6 / 524288.0);
	if (humidity > humidity_max)
	{
		humidity = humidity_max;
	}
	else if (humidity < humidity_min)
	{
		humidity = humidity_min;
	}

	return humidity;
}

int BME280::Initialize(const unsigned char i2cAddr)
{
	if (handle < 0)		//If already open, ignore and return -1.
	{
		handle = i2cOpen(1, i2cAddr, 0);
		if (handle >= 0)
		{
			Reset();
			SetCtrlHum(humidityOversamplingX4);
			SetCtrlMeas(temperatureOversamplingX8, pressureOversampleX8, sensorNormalMode);
			SetConfig(configStandby1000ms, configFilterOff);
			GetCalibrateData();
		
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int BME280::Initialize(const unsigned char i2cBus, const unsigned char i2cAddr)
{
	if (handle < 0)		//If already open, ignore and return -1.
	{
		handle = i2cOpen(i2cBus, i2cAddr, 0);
		if (handle >= 0)
		{
			Reset();
			SetCtrlHum(humidityOversamplingX4);
			SetCtrlMeas(temperatureOversamplingX8, pressureOversampleX8, sensorNormalMode);
			SetConfig(configStandby1000ms, configFilterOff);
			GetCalibrateData();
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int BME280::Close(void)
{
	i2cClose(handle);
	return 0;
}

int BME280::GetDeviceID()
{
	int deviceID = i2cReadByteData(handle, 0xD0);

	if (deviceID >= 0)
	{
		return deviceID;
	}
	else
	{
		return -1;
	}
}

int BME280::SetConfig(const unsigned char config)
{
	if (i2cWriteByteData(handle, 0xF5, config) >= 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int BME280::SetConfig(const unsigned char standBy, const unsigned char filter)
{
	int config = 0;
	config = (standBy << 5) | (filter << 2);	//Build the configuration bits

	if (i2cWriteByteData(handle, 0xF5, config) >= 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int BME280::SetCtrlMeas(const unsigned char ctrlMeas)
{
	if (i2cWriteByteData(handle, 0xF4, ctrlMeas) >= 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int BME280::SetCtrlMeas(const unsigned char temperatureOversampling, const unsigned char pressureOversampling, const unsigned char mode)
{
	int ctrlMeas = 0;
	ctrlMeas = (temperatureOversampling << 5) | (pressureOversampling << 2) | mode;	//Build the configuration bits

	if (i2cWriteByteData(handle, 0xF4, ctrlMeas) >= 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int BME280::SetCtrlHum(const int humididtyOversampling)
{

	if (i2cWriteByteData(handle, 0xF2, humididtyOversampling) >= 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int BME280::GetAllResults(void)
{
	GetSensorData();
	result.temperature = CompensateTemp(resultRaw.temperature);
	result.pressure = CompensatePressure(resultRaw.pressure);
	result.humididty = CompensateHumidity(resultRaw.humididty);
	return 0;
}

int BME280::GetSensorData()
{
	char* buff = new char[9];

	i2cReadI2CBlockData(handle, 0xF7, buff, 8);

	unsigned char pressMsb = buff[0];
	unsigned char pressLsb = buff[1];
	unsigned char pressXlsb = buff[2];
	unsigned char tempMsb = buff[3];
	unsigned char tempLsb = buff[4];
	unsigned char tempXlsb = buff[5];
	unsigned char humMsb = buff[6];
	unsigned char humLsb = buff[7];

	resultRaw.temperature = tempMsb << 12 | tempLsb << 4 | tempXlsb >> 4;
	resultRaw.pressure = pressMsb << 12 | pressLsb << 4 | pressXlsb >> 4;
	resultRaw.humididty = humMsb << 8 | humLsb;

	delete[] buff;
	return 0;
}

double BME280::GetTemperature(void)
{
	GetSensorData();
	result.temperature = CompensateTemp(resultRaw.temperature);
	return result.temperature;
}

double BME280::GetPressure(void)
{
	GetSensorData();
	result.pressure = CompensatePressure(resultRaw.pressure);
	return result.pressure;
}

double BME280::GetHumidity(void)
{
	GetSensorData();
	result.humididty = CompensateHumidity(resultRaw.humididty);
	return result.humididty;
}

bool BME280::StatusMeasuringBusy(void)
{
	int var = GetStatus() & 8;

	if (var == 8)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BME280::ImUpdateBusy(void)
{
	int var = GetStatus() & 1;

	if (var == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int BME280::Reset(void)
{
	i2cWriteByteData(handle, 0xE0, 0xB6);
	return 0;
}
