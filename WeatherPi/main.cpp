#include "BME280.hpp"
#include "SSD1306.hpp"
#include "DS18B20.hpp"
#include "AS3935.hpp"
#include "I2CLCD.hpp"
#include <pigpio.h>
#include <iostream>
#include <iomanip>
#include "Mono18pt7b.hpp"
#include <string.h>
#include <math.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <cstring>
#include <sstream>

#define PORT     8080 
#define MAXLINE 1024 

//gpioISRFunc_t PushedButton;

AS3935 lightningDetector;
I2CLCD lcd;

void As3935Interrupt(int gpio, int level, uint32_t tick);

int main(void)
{
	system("clear");

	BME280 sensor;
	SSD1306 oled;
	DS18B20 tempSensor;
	

	std::string Temperature = "";
	std::string Humidity = "";
	std::string Pressure = "";
	std::string UDPreturn = "";
	std::string q = "";

	std::string UDPrequest = "";

	std::cout << std::fixed;
	std::cout << std::setprecision(3);

	gpioInitialise();			// Initialize PIGPIO Library

	lcd.Initialize(0x27);
	lcd.WriteString("Tsup World!");	// Write "H" to LCD
	//lcd.WriteCharacter('H');
	lcd.WriteCommand(0x08);

	gpioSetMode(27, PI_OUTPUT);
	gpioSetMode(17, PI_INPUT);
	gpioSetPullUpDown(17, PI_PUD_OFF);

	gpioISRFunc_t As3935CallBack = As3935Interrupt;				// Setup Interrupt Callback
	gpioSetISRFunc(17, 1, 0, As3935CallBack);

	sensor.Initialize(0x76,		// Initialize BME280
		sensor.humidityOversamplingX1, 
		sensor.temperatureOversamplingX1, 
		sensor.pressureOversamplingX1, 
		sensor.sensorForcedMode);
	oled.Initialize(0x3C);		// Initialize SSD1306 OLED Display
	oled.DisplayOff();
	tempSensor.Mount();
	tempSensor.Initialize();

	if (lightningDetector.Initialize(0) >= 0)
	{
		std::cout << "AS3935 opened" << std::endl;
	}
	else
	{
		std::cout << "could not open AS3935" << std::endl;
	}

	lightningDetector.SetRegister(0x03, 0x00);

	std::cout << "Lightning detector register 0 data: " << (int)lightningDetector.ReadRegister(0) << std::endl;
	std::cout << "Lightning detector register 1 data: " << (int)lightningDetector.ReadRegister(1) << std::endl;
	std::cout << "Lightning detector register 2 data: " << (int)lightningDetector.ReadRegister(2) << std::endl;
	std::cout << "Lightning detector interrupt register data: " << (int)lightningDetector.ReadRegister(0x03) << std::endl;
	// ==================== UDP Server Code ==========================
	
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

	len = sizeof(cliaddr);  //len is value/resuslt 

	while (q != "0")
	{
		std::cout << "Listening..." << std::endl;

		n = recvfrom(sockfd, (char*)buffer, MAXLINE,
			MSG_WAITALL, (struct sockaddr*)&cliaddr,
			(socklen_t*)&len);

		std::cout << "Received UDP Datagram from " << inet_ntoa(cliaddr.sin_addr) << std::endl;

		buffer[n] = '\0';

		UDPrequest = buffer;
		q = UDPrequest;
		std::cout << "Request Message: " << UDPrequest << std::endl;

		switch (buffer[0])
		{
		case '1':
		{
			UDPreturn = std::to_string(sensor.GetTemperature()) + ',' 
				+ std::to_string(sensor.GetHumidity()) + ',' 
				+ std::to_string(sensor.GetPressure()) + ',' 
				+ std::to_string(tempSensor.GetTemperature());

			// TODO
			// + std::to_string(lightningDetector.Distance);
			// lightningDetector.LightningDetected = false;
			// lightningDetector.Distance = 0;
			// Not implienting it yet because the client programs are not yet updated to accept the new string

			sendto(sockfd, (const char*)UDPreturn.c_str(), UDPreturn.length(),
				MSG_CONFIRM, (const struct sockaddr*)&cliaddr,
				len);

			std::cout << "Return Message: """ << UDPreturn << """ has been sent." << std::endl << std::endl;
		}
		break;

		case '2':	// Section to test some stuff
		{
			std::stringstream stream;
			stream << std::fixed << std::setprecision(1) << sensor.GetTemperature() << "c";
			std::string oledTemperature = stream.str();
			stream.str("");
			stream << std::fixed << std::setprecision(1) << sensor.GetHumidity() << "%";
			std::string oledHumidity = stream.str();
			std::cout << oledHumidity << std::endl;
			std::string oHumidity;
			oled.ClearBuffer();
			oled.PrintS(oledTemperature.c_str(), 0, 0);
			oled.PrintS(oledHumidity.c_str(), 0, 32);
			oled.Display();
			std::cout << "Turning Oled on" << std::endl;		
		}
		break;

		case '3':	// Section to test some stuff
		{
			oled.ClearScreen();
			oled.DisplayOff();
			std::cout << "Turning OLED off" << std::endl;
		}
		break;

		case '4':	// Section to test some stuff
		{
			oled.DisplayOn();
		}
		break;
		}
	}
	oled.DisplayOff();
	std::cout << "Releasing I2C Handles" << std::endl;
	oled.Close();
	sensor.Close();
	std::cout << "Releasing GPIO Library" << std::endl;
	gpioTerminate();
	return 0;
}

void As3935Interrupt(int gpio, int level, uint32_t tick)
{
	std::cout << "AS3935 Interrupt Triggered" << std::endl;
	usleep(5000);						// Wait 5 ms for AS3935 to finish operations
	gpioWrite(27, !gpioRead(27));		// 

	int Lint = (int)lightningDetector.ReadRegister(0x03) & 0x0F;

	switch (Lint)
	{
	case 8:
		lightningDetector.LightningDetected = true;
		lightningDetector.Distance = (int)lightningDetector.ReadRegister(0x07);
		std::cout << "Lightning Distance estimation: " << lightningDetector.Distance << "km" << std::endl;

		break;
	case 4:
		std::cout << "Disturber detected" << std::endl;
		break;
	case 1:
		std::cout << "Noise level to high" << std::endl;
		break;
	case 0:
		std::cout << "Old data purged" << std::endl;
		break;
	default:
		break;
	}

	usleep(5000);

	return;
}
