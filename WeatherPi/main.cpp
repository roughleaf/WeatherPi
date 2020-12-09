#include "BME280.hpp"
#include "SSD1306.hpp"
#include "DS18B20.hpp"
#include "AS3935.hpp"
#include "I2CLCD.hpp"
#include <pigpio.h>
#include <iostream>
#include <iomanip>
#include "Mono18pt7b.hpp"
#include "NRF24L10.hpp"
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

#define PORT     8080 
#define MAXLINE 1024 

//gpioISRFunc_t PushedButton;


// Need to make these global to be able tom use them in the iterrupt callback functions
AS3935 lightningDetector;
I2CLCD lcd;
NRF24L10 nrf24;

void As3935Interrupt(int gpio, int level, uint32_t tick);
void NrfInterrupt(int gpio, int level, uint32_t tick);

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

	char nrfTest[5] = { 'a', 'b', 'c', 'd', 'e' };

	std::string UDPrequest = "";

	std::cout << std::fixed;
	std::cout << std::setprecision(3);

	gpioInitialise();			// Initialize PIGPIO Library

	lcd.Initialize(0x27);
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


	sensor.Initialize(0x76,		// Initialize BME280
		sensor.humidityOversamplingX1, 
		sensor.temperatureOversamplingX1, 
		sensor.pressureOversamplingX1, 
		sensor.sensorForcedMode);
	oled.Initialize(0x3C);		// Initialize SSD1306 OLED Display
	oled.DisplayOff();
	tempSensor.Mount();
	tempSensor.Initialize();

	if (nrf24.Initialize(1) >= 0)
	{
		char nrTest[6] = { 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2 };
		nrf24.WriteRegisterBytes(0x0A, nrTest, 5);
		nrf24.WriteRegister(0x0D, 0x7A);
		nrf24.ReadRegisterBytes(0x0A, nrf24.rxAddr, 5);
		std::cout << "NRF24L10 Radio Opened" << std::endl;
		std::cout << "========================================================" << std::endl;
		std::cout << "NRF24L10 Radio Read: " << std::hex << (int)nrf24.ReadRegister(0x0D) << std::endl;
		std::cout << "NRF24L10 Radio Multi Read: " << std::hex << (int)nrf24.rxAddr[1] << std::endl;
		std::cout << "========================================================" << std::endl;
	}
	else
	{
		std::cout << "NRF24L10 could not be opened" << std::endl;
	}

	if (lightningDetector.Initialize(0) >= 0)
	{
		std::cout << "AS3935 opened" << std::endl;
	}
	else
	{
		std::cout << "could not open AS3935" << std::endl;
	}
	
	//gpioISRFunc_t As3935CallBack = As3935Interrupt;				// Setup Interrupt Callback
	//gpioSetISRFunc(17, RISING_EDGE, 0, As3935CallBack);

	gpioISRFunc_t NrfCallBack = NrfInterrupt;				// Setup Interrupt Callback
	gpioSetISRFunc(22, FALLING_EDGE, 0, NrfInterrupt);

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
			std::cout << "============================== nrf Write stuff =======================" << std::endl;
			std::cout << "Status Register: " << std::hex << (int)nrf24.ReadStatus() << std::endl;
			std::string toSend = "Test String";
			//lcd.BacklightToggle();
			nrf24.TransmitToChannel(toSend.c_str(), 1);
			std::cout << "Status Register: " << std::hex << (int)nrf24.ReadStatus() << std::endl;
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
	std::string count = "";
	std::string dist = "";

	std::cout << std::endl;
	std::cout << "==============================================" << std::endl;
	std::cout << "AS3935 Interrupt Triggered" << std::endl;
	std::cout << "==============================================" << std::endl;
	usleep(5000);						// Wait 5 ms for AS3935 to finish operations
	gpioWrite(27, !gpioRead(27));		// 

	int Lint = (int)lightningDetector.ReadRegister(0x03) & 0x0F;

	std::cout << "Register 3: " << Lint <<std::endl;
	usleep(2000);
	std::cout << "==============================================" << std::endl;

	//lightningDetector.SetRegister(0x07, 0);
	switch (Lint)
	{
	case 8:
	{
		count = std::to_string(++lightningDetector.LightningStrikeCount);
		dist = std::to_string((int)lightningDetector.ReadRegister(0x07));

		std::cout << "Lightning detected" << std::endl;
		std::cout << "Lightning Distance estimation: " << dist << "km" << std::endl;

		lcd.Clear();
		lcd.Home();
		lcd.WriteCommand(LCD_FIRST_ROW);
		lcd.WriteString("Lightning: ");
		lcd.WriteString(dist.c_str());
		lcd.WriteString("km");
		lcd.WriteCommand(LCD_SECOND_ROW);
		lcd.WriteString("Strikes: ");
		lcd.WriteString(count.c_str());
		//lcd.WriteString((std::to_string(lightningDetector.DisturberCount)).c_str());
	}
		break;
	case 4:
	{
		count = std::to_string(++lightningDetector.DisturberCount);
		dist = std::to_string((int)lightningDetector.ReadRegister(0x07));

		std::cout << "Disturber detected" << std::endl;
		std::cout << "Distance Register: " << dist << std::endl;

		/*lcd.Home();
		lcd.WriteCommand(LCD_FIRST_ROW);
		lcd.WriteString("Lightning: ");
		lcd.WriteString(dist.c_str());
		lcd.WriteString("km");
		lcd.WriteCommand(LCD_SECOND_ROW);
		lcd.WriteString("Strikes: ");
		lcd.WriteString(count.c_str());*/
	}
		break;
	case 1:
	{
		std::cout << "Noise level to high" << std::endl;
	}
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

void NrfInterrupt(int gpio, int level, uint32_t tick)
{
	std::cout << "==================================== nrf24L10 ===========================================" << std::endl;
	std::cout << "Entered nrf interrupt callback" << std::endl;

	gpioWrite(19, !gpioRead(19));

	char status = 0;

	status = nrf24.ReadStatus();

	if (status & 0x40)		// RX Ready Interrupt
	{
		std::cout << "==================================== nrf24L10 ===========================================" << std::endl;
		std::cout << "RX Ready interrupt triggered" << std::endl;
		nrf24.WriteRegister(0x07, (status | 0x40));		// Clear RX interrupt flag
		std::cout << "RX Ready interrupt flag cleared" << std::endl;
		// TODO
		// Check what channel sent the data
		// Check how many bytes was sent
		// Decide how to return retreived data to main application
	}

	if (status & 0x20)		// TX data sent interrupt flag
	{
		std::cout << "==================================== nrf24L10 ===========================================" << std::endl;
		std::cout << "TX data sent interrupt triggered" << std::endl;
		nrf24.PRXmode();
		std::cout << "nrf24L10 set to RX mode" << std::endl;
		nrf24.WriteRegister(0x07, (status | 0x20));		// Clear TX data sent interrupt flag
		std::cout << "TX data sent interrupt flag cleared" << std::endl;
	}

	if (status & 0x10)		// Resend interrupt flag
	{
		std::cout << "==================================== nrf24L10 ===========================================" << std::endl;
		std::cout << "Resent retry interrupt triggered" << std::endl;
		nrf24.WriteRegister(0x07, (status | 0x10));		// Clear resend interrupt flag
		std::cout << "Resent retry interrupt flag cleared" << std::endl;
	}
	// TODO
	// Set default RX address
}
