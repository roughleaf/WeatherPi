#include "BME280.hpp"
#include "SSD1306.hpp"
#include "DS18B20.hpp"
#include "AS3935.hpp"
#include "I2CLCD.hpp"
#include <pigpio.h>
#include <iostream>
#include <iomanip>
#include "Mono18pt7b.hpp"
#include "NRF24L01.hpp"
#include <string.h>
#include <math.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "STIME.hpp"
#include "Icodec.hpp"
#include "ClimateData.hpp"

#define PORT 8080 
#define MAXLINE 1024 

// Need to make these global to be able to use them in the interrupt callback functions
AS3935 lightningDetector;
I2CLCD lcd;
NRF24L01 nrf24;
STIME systemTime;

ClimateData nodeData[10];
bool nrfDateTimeTransmitFlag = false;

void As3935Interrupt(int gpio, int level, uint32_t tick);
void NrfInterrupt(int gpio, int level, uint32_t tick);

int main(void)
{	
	system("clear");			// Clear console window

	gpioInitialise();			// Initialize PIGPIO Library
	systemTime.GetSystemTime();
	std::cout << systemTime.SystemDateTime << " PIGPIO Initialized" << std::endl;

	BME280 sensorBME280;
	SSD1306 oledDisplay;
	DS18B20 tempSensorDS18B20;

	std::string Temperature = "";
	std::string Humidity = "";
	std::string Pressure = "";
	std::string UDPreturn = "";
	std::string q = "";

	std::string UDPrequest = "";

	std::cout << std::fixed;
	std::cout << std::setprecision(3);

	if (lcd.Initialize(0x27) >= 0)
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " 16x2 LCD Initialized" << std::endl;
	}
	else
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " 16x2 LCD could not be Initialized" << std::endl;
	}

	lcd.On();

	lcd.Home();
	lcd.WriteCommand(LCD_FIRST_ROW);
	lcd.WriteString("Lightning: ");
	lcd.WriteCommand(LCD_SECOND_ROW);
	lcd.WriteString("Strikes: 0");

	gpioSetMode(27, PI_OUTPUT);
	gpioSetMode(26, PI_INPUT);
	gpioSetMode(17, PI_INPUT);
	gpioSetMode(6, PI_INPUT);
	gpioSetMode(19, PI_OUTPUT);
	gpioSetPullUpDown(17, PI_PUD_OFF);


	if (sensorBME280.Initialize(0x76,		// Initialize BME280
		sensorBME280.humidityOversamplingX1,
		sensorBME280.temperatureOversamplingX1,
		sensorBME280.pressureOversamplingX1,
		sensorBME280.sensorForcedMode) >= 0)
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " BME280 Initialized" << std::endl;
	}
	else
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " BME280 could not be Initialized" << std::endl;
	}

	oledDisplay.Initialize(0x3C);		// Initialize SSD1306 OLED Display
	oledDisplay.DisplayOff();

	tempSensorDS18B20.Mount();
	if (tempSensorDS18B20.Initialize() >= 0)
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " DS18B20 Initialized" << std::endl;
	}
	else
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " DS18B20 could not be Initialized" << std::endl;
	}

	if (nrf24.Initialize(1) >= 0)
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " NRF24L10 Radio Opened" << std::endl;
	}
	else
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " NRF24L10 could not be opened" << std::endl;
	}

	if (lightningDetector.Initialize(0) >= 0)
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " AS3935 opened" << std::endl;
	}
	else
	{
		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " could not open AS3935" << std::endl;
	}
	
	//sleep(2);	// Give time for all intialized devices to stabelise

	systemTime.GetSystemTime();
	std::cout << systemTime.SystemDateTime << " All devices started..." << std::endl;

	//gpioISRFunc_t As3935CallBack = As3935Interrupt;				// Setup Interrupt Callback
	gpioSetISRFunc(17, RISING_EDGE, 0, As3935Interrupt);			// Setup AS3935 Interrupt Callback

	//gpioISRFunc_t NrfCallBack = NrfInterrupt;				// Setup Interrupt Callback
	gpioSetISRFunc(22, FALLING_EDGE, 0, NrfInterrupt);		// Setup NRF24L01+ Interrupt Callback
				

	// ==================== UDP Server Code ==========================
	
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address 
	if (bind(sockfd, (const struct sockaddr*) & servaddr,
		sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	int len, n;

	len = sizeof(cliaddr);  //len is value/result 

	while (q != "0")
	{
		std::cout << "\nListening..." << std::endl << std::endl;

		n = recvfrom(sockfd, (char*)buffer, MAXLINE,
			MSG_WAITALL, (struct sockaddr*)&cliaddr,
			(socklen_t*)&len);

		systemTime.GetSystemTime();
		std::cout << systemTime.SystemDateTime << " Received UDP Datagram from " << inet_ntoa(cliaddr.sin_addr) << std::endl;

		buffer[n] = '\0';

		UDPrequest = buffer;
		q = UDPrequest;
		std::cout << "\t\t- Request Message: " << UDPrequest << std::endl;

		switch (buffer[0])
		{
		case '1':
		{
			UDPreturn = std::to_string(sensorBME280.GetTemperature()) + ',' 
				+ std::to_string(sensorBME280.GetHumidity()) + ',' 
				+ std::to_string(sensorBME280.GetPressure()) + ',' 
				+ std::to_string(tempSensorDS18B20.GetTemperature());

			// TODO
			// + std::to_string(lightningDetector.Distance);
			// lightningDetector.LightningDetected = false;
			// lightningDetector.Distance = 0;
			// Not implienting it yet because the client programs are not yet updated to accept the new string
			// Not going to impliment above. Instead going to impliment a json message serializer.

			sendto(sockfd, (const char*)UDPreturn.c_str(), UDPreturn.length(),
				MSG_CONFIRM, (const struct sockaddr*)&cliaddr,
				len);

			std::cout << "\t\t- Return Message: """ << UDPreturn << """ has been sent." << std::endl << std::endl;
		}
		break;

		case '2':	// Section to test some stuff
		{
			std::stringstream stream;
			stream << std::fixed << std::setprecision(1) << sensorBME280.GetTemperature() << "c";
			std::string oledTemperature = stream.str();
			stream.str("");
			stream << std::fixed << std::setprecision(1) << sensorBME280.GetHumidity() << "%";
			std::string oledHumidity = stream.str();
			std::cout << oledHumidity << std::endl;
			std::string oHumidity;
			oledDisplay.ClearBuffer();
			oledDisplay.PrintS(oledTemperature.c_str(), 0, 0);
			oledDisplay.PrintS(oledHumidity.c_str(), 0, 32);
			oledDisplay.Display();
			std::cout << "Turning Oled on" << std::endl;		
		}
		break;

		case '3':	// Section to test some stuff
		{
			oledDisplay.ClearScreen();
			oledDisplay.DisplayOff();
			std::cout << "Turning OLED off" << std::endl;
		}
		break;

		case '4':	// Section to test some stuff
		{
			oledDisplay.DisplayOn();
		}
		break;

		case '5':	// Section to test some stuff
		{
			lightningDetector.DisturberCount = 0;
			lightningDetector.LightningStrikeCount = 0;
			lcd.Clear();
			lcd.Home();
			lcd.WriteCommand(LCD_FIRST_ROW);
			lcd.WriteString("Lightning: 0");
			lcd.WriteCommand(LCD_SECOND_ROW);
			lcd.WriteString("Disturber: 0");
		}
		break;

		case '6':	// Section to test some stuff
		{
			lcd.On();
		}
		case '7':	// Section to test some stuff
		{
			lcd.Off();
		}
		break;

		case '8':	// Section to test some stuff
		{
			char datetimeTest[13] = { 0 };
			icodec::BuildTimeDateByteString(datetimeTest, 0);
			//lcd.BacklightToggle();
			nrf24.TransmitData(datetimeTest, 13);		
			/*std::cout << "=================================================================================== " << std::endl;
			std::cout << "Register EN_AA: " << (int)nrf24.ReadRegister(EN_AA_REG) << std::endl;
			std::cout << "Register EN_RXADDR: " << (int)nrf24.ReadRegister(EN_RXADDR_REG) << std::endl;
			std::cout << "Register SETUP_AW: " << (int)nrf24.ReadRegister(SETUP_AW_REG) << std::endl;
			std::cout << "Register Config: " << (int)nrf24.ReadRegister(CONFIG_REG) << std::endl;
			std::cout << "Register Status: " << (int)nrf24.ReadRegister(STATUS_REG) << std::endl;
			std::cout << "Register DYNPD: " << (int)nrf24.ReadRegister(0x1C) << std::endl;
			std::cout << "Register Feature: " << (int)nrf24.ReadRegister(0x1D) << std::endl;
			std::cout << "=================================================================================== " << std::endl;*/
			systemTime.GetSystemTime();
			std::cout << systemTime.SystemDateTime << " Date and time transmitted to SensorNode #" << "[Placeholder]" << std::endl;
		}
		break;
		}
	}
	oledDisplay.DisplayOff();
	std::cout << "Releasing I2C Handles" << std::endl;
	oledDisplay.Close();
	sensorBME280.Close();
	std::cout << "Releasing GPIO Library" << std::endl;
	gpioTerminate();
	return 0;
}

void As3935Interrupt(int gpio, int level, uint32_t tick)
{
	std::string count = "";
	std::string dist = "";

	systemTime.GetSystemTime();
	std::cout << '\n' << systemTime.SystemDateTime << " AS3935 Interrupt Triggered\n" << std::endl;

	usleep(5000);						// Wait 5 ms for AS3935 to finish operations

	int Lint = (int)lightningDetector.ReadRegister(0x03) & 0x0F;

	std::cout << "\t\t - Register 3: " << Lint <<std::endl;
	usleep(2000);

	//lightningDetector.SetRegister(0x07, 0);
	switch (Lint)
	{
	case 8:
	{
		count = std::to_string(++lightningDetector.LightningStrikeCount);
		dist = std::to_string((int)lightningDetector.ReadRegister(0x07));

		std::cout << "\t\t - Lightning detected" << std::endl;
		std::cout << "\t\t - Lightning Distance estimation: " << dist << "km" << std::endl;

		lcd.Clear();
		lcd.Home();
		lcd.WriteCommand(LCD_FIRST_ROW);
		lcd.WriteString("Lightning: ");
		lcd.WriteString(dist.c_str());
		lcd.WriteString("km");
		lcd.WriteCommand(LCD_SECOND_ROW);
		lcd.WriteString("Strikes: ");
		lcd.WriteString(count.c_str());
	}
		break;
	case 4:
	{
		count = std::to_string(++lightningDetector.DisturberCount);
		dist = std::to_string((int)lightningDetector.ReadRegister(0x07));

		std::cout << "\t\t - Disturber detected" << std::endl;
		std::cout << "\t\t - Distance Register: " << dist << std::endl;
	}
		break;
	case 1:
	{
		std::cout << "\t\t - Noise level to high" << std::endl;
	}
		break;
	case 0:
		std::cout << "\t\t - Old data purged" << std::endl;
		break;
	default:
		break;
	}

	usleep(5000);

	return;
}

void NrfInterrupt(int gpio, int level, uint32_t tick)
{
	systemTime.GetSystemTime();
	std::cout << '\n' << systemTime.SystemDateTime << " Entered nRF24L01+ interrupt callback" << std::endl;

	gpioWrite(19, !gpioRead(19));	// LED toggle with each nrf24 interrupt

	char status = 0;

	status = nrf24.ReadStatus();

	// TODO
	// Decide how to return retreived data to main application

	if (status & 0x40)		// RX Ready Interrupt
	{
		std::cout << "\t\t - RX Ready interrupt triggered\n" << std::endl;

		int rxWidth = nrf24.GetRXWidth();		// How many bytes is in the received packet
		char rxBuff[32] = { 0 };

		nrf24.ReadPayload(rxBuff, rxWidth);

		int Command = (int)rxBuff[0];
		int NodeID = (int)rxBuff[1];

		if (Command == 1)
		{
			if ((NodeID >= 0) && (NodeID <= 8))	// Raspberry pi will always be NodeID[9]
			{
				nodeData[NodeID].PopulateFromSensorNode(rxBuff);

				std::cout << "\n\t\t - Node ID: " << nodeData[NodeID].NodeID << std::endl;
				std::cout << "\t\t - Timestamp from node: " << nodeData[NodeID].Time << std::endl;
				std::cout << "\t\t - Datestamp from node: " << nodeData[NodeID].Date << std::endl;
				std::cout << "\t\t - RX Node BME Temperature: " << nodeData[NodeID].BME280Temperature << std::endl;
				std::cout << "\t\t - RX Node BME Pressure: " << nodeData[NodeID].BME280Pressure << std::endl;
				std::cout << "\t\t - RX Node BME Humidity: " << nodeData[NodeID].BME280Humididty << std::endl;
				std::cout << "\t\t - RX Node DS18B20 Temperature: " << nodeData[NodeID].DS18B20Temperature << std::endl;
				std::cout << "\t\t - RX Node Rain Count: " << nodeData[NodeID].RainCount << std::endl;
			}
			else
			{
				std::cout << "\n\t\t *** Received node ID: " << NodeID << " is out of range" << std::endl;
				std::cout << "\t\t *** Confirm SW1 dipswitch configuration and restart SensorNode\n" << std::endl;
			}
		}
		else
		{
			nrfDateTimeTransmitFlag = true;
			char datetimeTest[13] = { 0 };
			icodec::BuildTimeDateByteString(datetimeTest, NodeID);
			nrf24.TransmitData(datetimeTest, 13);					// Seems possible to transmit from this thread. Needs more testing.
			systemTime.GetSystemTime();
			std::cout << "\n\t\t - Node ID: " << nodeData[NodeID].NodeID << " requested Date and Time" << std::endl;
			std::cout << '\n\t\t' << systemTime.SystemDateTime << " Date and time transmitted to SensorNode #" << NodeID << std::endl;
		}

		nrf24.WriteRegister(0x07, (status | 0x40));		// Clear RX interrupt flag
		std::cout << "\n\t\t - RX received interrupt flag cleared" << std::endl;
	}

	if (status & 0x20)		// TX data sent interrupt flag
	{
		std::cout << "\t\t - Data was successfully transmitted to a SensorNode" << std::endl;
		nrf24.WriteRegister(0x07, (status | 0x20));		// Clear TX data sent interrupt flag
		std::cout << "\t\t - TX data sent interrupt flag cleared" << std::endl;
	}

	if (status & 0x10)		// Resend interrupt flag
	{
		char buftemp[5] = { 0 };
		std::cout << "\t\t - *** Failed to transmit data to a SensorNode" << std::endl;
		nrf24.WriteRegister(0x07, (status | 0x10));		// Clear resend interrupt flag
		std::cout << "\t\t - Resent retry interrupt flag cleared" << std::endl;
	}
	nrf24.WriteRegister(0x07, (status | 0x70));		// Ensure that all interrupts are clear
	nrf24.PRXmode();	// This device will primarily be an PRX device
}
