#include "WeatherPiNetThreads.hpp"
#include <sstream>
#include <iomanip>

void* udpNet(void* port)
{
	int Port;
	Port = (int)port;

	std::string UDPreturn = "";
	std::string q = "";
	std::string UDPrequest = "";

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
	if (bind(sockfd, (const struct sockaddr*)&servaddr,
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


		std::cout << systemTime.GetSystemDateTime() << " Received UDP Datagram from " << inet_ntoa(cliaddr.sin_addr) << std::endl;

		buffer[n] = '\0';

		UDPrequest = buffer;
		q = UDPrequest;
		std::cout << "\t\t- Request Message: " << UDPrequest << std::endl;

		switch (buffer[0])
		{
		case '1':
		{
			nodeData[9].PopulateFromLocal(sensorBME280.GetTemperature(), sensorBME280.GetPressure(), sensorBME280.GetHumidity(), tempSensorDS18B20.GetTemperature());

			UDPreturn = nodeData[9].UdpReturnString();

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
			// Add some lightnigh strikes to test JSON array
			lightningData.AddLightningStrike(8);
			lightningData.AddLightningStrike(6);
			lightningData.AddLightningStrike(1);
			lightningData.AddLightningStrike(5);
			lightningData.AddLightningStrike(8);
			

			// Populate nodeData object with data from sensors on the Raspberry pi.
			// Then I test if the JSON from a single node is valid
			// After that I test if all the nodes in a JSON array functions correctly.
			// To test I copy the data from the console output and paste in in a online JSON validation tool and viewer.
			nodeData[9].PopulateFromLocal(sensorBME280.GetTemperature(), sensorBME280.GetPressure(), sensorBME280.GetHumidity(), tempSensorDS18B20.GetTemperature());
			std::cout << "\n\n=========================================================================================================" << std::endl;
			std::cout << "Resulting JSON String for node[9]:\n" << nodeData[9].BuildJsonString() << std::endl;
			std::cout << "=========================================================================================================" << std::endl;

			std::cout << "\n\n=========================================================================================================" << std::endl;
			std::cout << "Resulting JSON String for everything:\n" << icodec::BuildJsonArray(nodeData, lightningData) << std::endl;
			std::cout << "=========================================================================================================" << std::endl;

			/*int NodeID = 1;
			std::cout << "===========================================================================================" << std::endl;
			std::cout << "\n\t\t - Node ID: " << nodeData[NodeID].NodeID << std::endl;
			std::cout << "\t\t - Timestamp from node: " << nodeData[NodeID].Time << std::endl;
			std::cout << "\t\t - Datestamp from node: " << nodeData[NodeID].Date << std::endl;
			std::cout << "\t\t - RX Node BME Temperature: " << nodeData[NodeID].BME280Temperature << std::endl;
			std::cout << "\t\t - RX Node BME Pressure: " << nodeData[NodeID].BME280Pressure << std::endl;
			std::cout << "\t\t - RX Node BME Humidity: " << nodeData[NodeID].BME280Humididty << std::endl;
			std::cout << "\t\t - RX Node DS18B20 Temperature: " << nodeData[NodeID].DS18B20Temperature << std::endl;
			std::cout << "\t\t - RX Node Rain Count: " << nodeData[NodeID].RainCount << std::endl;
			std::cout << "===========================================================================================" << std::endl;*/
		}
		break;
		}
	}
	pthread_exit(NULL);
}