#include "BME280.hpp"
#include "SSD1306.hpp"
#include "DS18B20.hpp"
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

#define	LED	17

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
	char t = 0;
	char* device;

	//char c = 0;

	std::cout << std::fixed;
	std::cout << std::setprecision(3);

	gpioInitialise();

	sensor.Initialize(0x76);	// Initialize BME280
	oled.Initialize(0x3C);		// Initialize SSD1306 OLED Display
	oled.DisplayOff();

	// ==================== UDP Server Code ==========================
	
	int sockfd;
	char buffer[MAXLINE], c;
	char* hello = "Hello from server";
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

	//================================ Test code to calibrate DS18B20 ======================================
	//This code will update the OLED with the temperatures read from the BME280 and DS18B20 every 15 seconds
	//std::stringstream stream;
	std::string BME280Temperature = "0";
	std::string DS18B20Tempterature = "0";

	//oled.DisplayOn();

	tempSensor.Mount();
	tempSensor.Initialize();
	
	/*while (1)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << sensor.GetTemperature() << "c";
		std::string oledTemperature = stream.str();
		stream.str("");
		stream << std::fixed << std::setprecision(0) << sensor.GetHumidity() << "%";
		std::string oledHumidity = stream.str();
		std::cout << oledHumidity << std::endl;
		//std::string oTemperature = std::to_string(sensor.GetTemperature());
		std::string oHumidity;
		oled.ClearScreen();
		oled.DisplayOn();
		oled.PrintS(oledTemperature.c_str(), 0, 0);
		oled.PrintS(oledHumidity.c_str(), 0, 32);
		oled.Display();
		std::cout << "Displaying Temps" << std::endl;

		sleep(2);
	}*/

	//======================================================================================================


	

	while (q != "0")
	{
		std::cout << "Listening..." << std::endl;

		n = recvfrom(sockfd, (char*)buffer, MAXLINE,
			MSG_WAITALL, (struct sockaddr*)&cliaddr,
			(socklen_t*)&len);


		//std::cout << "Received UDP Datagram from " << cliaddr.sin_addr.s_addr << std::endl;
		std::cout << "Received UDP Datagram from " << inet_ntoa(cliaddr.sin_addr) << std::endl;

		buffer[n] = '\0';

		UDPrequest = buffer;
		q = UDPrequest;
		std::cout << "Request Message: " << UDPrequest << std::endl;

		switch (buffer[0])
		{
		case '1':
		{
			UDPreturn = std::to_string(sensor.GetTemperature()) + ',' + std::to_string(sensor.GetHumidity()) + ',' + std::to_string(sensor.GetPressure()) + ',' + std::to_string(tempSensor.GetTemperature());
			sendto(sockfd, (const char*)UDPreturn.c_str(), UDPreturn.length(),
				MSG_CONFIRM, (const struct sockaddr*)&cliaddr,
				len);
			std::cout << "Return Message: """ << UDPreturn << """ has been sent." << std::endl << std::endl;
		}
		break;

		case '2':
		{
			std::stringstream stream;
			stream << std::fixed << std::setprecision(1) << sensor.GetTemperature() << "c";
			std::string oledTemperature = stream.str();
			stream.str("");
			stream << std::fixed << std::setprecision(1) << sensor.GetHumidity() << "%";
			std::string oledHumidity = stream.str();
			std::cout << oledHumidity << std::endl;
			//std::string oTemperature = std::to_string(sensor.GetTemperature());
			std::string oHumidity;
			oled.ClearScreen();
			oled.DisplayOn();
			oled.PrintS(oledTemperature.c_str(), 0, 0);
			oled.PrintS(oledHumidity.c_str(), 0, 32);
			oled.Display();
			std::cout << "Turning Oled on" << std::endl;		
		}
		break;

		case '3':
		{
			oled.ClearScreen();
			oled.DisplayOff();
			std::cout << "Turning OLED off" << std::endl;
		}
		break;

		case '4':
		{
			tempSensor.Mount();
			tempSensor.Initialize();
			std::cout << tempSensor.GetTemperature() << std::endl;
		}
		break;
		}


	}
	oled.DisplayOff();
	std::cout << "Releasing I2C Handle" << std::endl;
	oled.Close();
	sensor.Close();
	std::cout << "Releasing GPIO Library" << std::endl;
	gpioTerminate();
	return 0;
}






	//std::cin >> c;

	//================================================================

	//displayHandle = i2cOpen(1, 0x3D, 0);
	/*
	std::cout << "Device ID = " << sensor.GetDeviceID() << std::endl;

	while (t != 'x')
	{
		std::cout << "(1) Read Temperature" << std::endl;
		std::cout << "(2) Read Humidity" << std::endl;
		std::cout << "(3) Read Pressure" << std::endl;
		std::cout << "(4) Show all sensor data" << std::endl;
		std::cout << "(5) Fill Screen" << std::endl;
		std::cout << "(6) Invert OLED" << std::endl;
		std::cout << "(7) OLED ON" << std::endl;
		std::cout << "(8) OLED OFF" << std::endl;
		std::cout << "(9) Clear Screen" << std::endl;
		std::cout << "(0) Update Oled" << std::endl;
		std::cout << "(v) Flip Vertically" << std::endl;
		std::cout << "(h) Flip Horizontally" << std::endl;
		std::cout << "(f) Some Font Stuff" << std::endl;
		std::cout << "(x) Exit" << std::endl;
		std::cout << "Just a random string" << std::endl;
		std::cin >> t;

		switch (t)
		{
		case '1':
		{
			system("clear");
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Temperature : " << sensor.GetTemperature() << " Degrees Celcius" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '2':
		{
			system("clear");
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Humidity : " << sensor.GetHumidity() << "%" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '3':
		{
			system("clear");
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Pressure : " << sensor.GetPressure() << " Pa" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '4':
		{
			system("clear");
			sensor.GetAllResults();
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Temperature is: " << sensor.result.temperature << " Degrees Celcius" << std::endl;
			std::cout << "Humidity is   : " << sensor.result.humididty << "%" << std::endl;
			std::cout << "Pressure is   : " << sensor.result.pressure << " Pa" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '5':
		{
			system("clear");
			oled.FillScreen();
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Fill OLED Screen" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '6':
		{
			system("clear");
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Inverting OLED"<< std::endl;
			std::cout << "Expect 0, got: " << oled.InvertScreen() << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '7':
		{
			system("clear");
			oled.DisplayOn();
			std::cout << "================================================================================================" << std::endl;
			std::cout << "OLED ON" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '8':
		{
			system("clear");
			oled.DisplayOff();
			std::cout << "================================================================================================" << std::endl;
			std::cout << "OLED OFF" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '9':
		{
			system("clear");
			std::cout << "================================================================================================" << std::endl;
			std::cout << "For Loop Results" << std::endl;
			std::cout << "================================================================================================" << std::endl;
			oled.ClearScreen();
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case '0':
		{
			system("clear");
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Update OLED with data" << std::endl;
			std::cout << "================================================================================================" << std::endl;
			char* tmp = new char[10];
			//sprintf(tmp, "%+0.02d", sensor.GetTemperature());
			snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%c%0.02f", '+', fabs(sensor.GetTemperature()));
			oled.PrintS(tmp, 0, 0);
			oled.Display();
			std::cout << "tmp = " << tmp << std::endl;
			delete(tmp);
		}
		break;

		case 'v':
		{
			system("clear");
			oled.FlipDisplayVertically();
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Flip Display Vertically" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case 'h':
		{
			system("clear");
			oled.FlipDisplayHorizontally();
			std::cout << "================================================================================================" << std::endl;
			std::cout << "Flip Display Horizontally" << std::endl;
			std::cout << "================================================================================================" << std::endl;
		}
		break;

		case 'f':
		{
			system("clear");
			//std::cout << "================================================================================================" << std::endl;
			//std::cout << "Check some font stuff" << std::endl;
			//std::cout << "Type in a character to get the Glyph Data :";
			//std::cin >> c;
			//oled.PrintC(c);
			oled.FontTest();
			system("clear");
		}
		break;

		case 'x':
			break;

		default:
		{
			std::cout << "Invalid Selection" << std::endl;
		}
		break;
		}
	}
	*/