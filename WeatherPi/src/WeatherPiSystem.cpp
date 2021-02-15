#include "WeatherPiSystem.hpp"

STIME systemTime;
ClimateData nodeData[10];
AS3935 lightningDetector;
I2CLCD lcd;
NRF24L01 nrf24;
BME280 sensorBME280;
SSD1306 oledDisplay;
DS18B20 tempSensorDS18B20;
LightningData lightningData;
TCP tcp;
std::string ServerIP;
std::string MacAddress;

void WeatherPiSystemInitialize(void)
{
	system("clear");			// Clear console window

	usleep(300000);				// Give the system command time to finish

	{
		int fd = -1;
		char buff[18] = { "" };
		fd = open("/sys/class/net/wlan0/address", O_RDONLY);
		read(fd, buff, sizeof(buff));
		buff[17] = '\0';
		MacAddress = buff;
		std::cout << systemTime.GetSystemDateTime() << " Device MAC address is: " << MacAddress << std::endl;
	}

	std::ifstream ServerIpFile("ServerIP.txt");
	getline(ServerIpFile, ServerIP);
	ServerIpFile.close();

	for (int i = 0; i < 10; i++)
	{
		nodeData[i].NodeID = i;	// Initialize values into NodeID for JSON identification on transmitted end.
	}

	gpioInitialise();			// Initialize PIGPIO Library

	gpioSetMode(27, PI_OUTPUT);
	gpioSetMode(26, PI_INPUT);
	gpioSetMode(17, PI_INPUT);
	gpioSetMode(6, PI_INPUT);
	gpioSetMode(19, PI_OUTPUT);
	gpioSetPullUpDown(17, PI_PUD_OFF);

	std::cout << systemTime.GetSystemDateTime() << " Server IP read from file: " << ServerIP << std::endl;
	std::cout << systemTime.GetSystemDateTime() << " PIGPIO Initialized" << std::endl;

	if (lcd.Initialize(0x27) >= 0)
	{
		std::cout << systemTime.GetSystemDateTime() << " 16x2 LCD Initialized" << std::endl;
	}
	else
	{
		std::cout << systemTime.GetSystemDateTime() << " 16x2 LCD could not be Initialized" << std::endl;
	}

	if (sensorBME280.Initialize(0x76,		// Initialize BME280
		sensorBME280.humidityOversamplingX1,
		sensorBME280.temperatureOversamplingX1,
		sensorBME280.pressureOversamplingX1,
		sensorBME280.sensorForcedMode) >= 0)
	{		
		std::cout << systemTime.GetSystemDateTime() << " BME280 Initialized" << std::endl;
	}
	else
	{
		systemTime.GetSystemDateTime();
		std::cout << systemTime.SystemDateTime << " BME280 could not be Initialized" << std::endl;
	}

	if (oledDisplay.Initialize(0x3C) >= 0)
	{
		std::cout << systemTime.GetSystemDateTime() << " SSD1306 OLED Display Initialized" << std::endl;
		oledDisplay.DisplayOff();
	}
	else
	{
		std::cout << systemTime.GetSystemDateTime() << " SSD1306 OLED Display could not be Initialized" << std::endl;
	}


	tempSensorDS18B20.Mount();
	if (tempSensorDS18B20.Initialize() >= 0)
	{
		std::cout << systemTime.GetSystemDateTime() << " DS18B20 Initialized" << std::endl;
	}
	else
	{
		std::cout << systemTime.GetSystemDateTime() << " DS18B20 could not be Initialized" << std::endl;
	}

	if (nrf24.Initialize(1) >= 0)
	{
		std::cout << systemTime.GetSystemDateTime() << " NRF24L10 Radio Opened" << std::endl;
	}
	else
	{
		std::cout << systemTime.GetSystemDateTime() << " NRF24L10 could not be opened" << std::endl;
	}

	if (lightningDetector.Initialize(0) >= 0)
	{
		std::cout << systemTime.GetSystemDateTime() << " AS3935 opened" << std::endl;
	}
	else
	{
		std::cout << systemTime.GetSystemDateTime() << " could not open AS3935" << std::endl;
	}

	gpioSetISRFunc(17, RISING_EDGE, 0, As3935Interrupt);	// Setup AS3935 Interrupt Callback
	gpioSetISRFunc(22, FALLING_EDGE, 0, NrfInterrupt);		// Setup NRF24L01+ Interrupt Callback

	std::cout << systemTime.GetSystemDateTime() << " All devices started..." << std::endl;
}