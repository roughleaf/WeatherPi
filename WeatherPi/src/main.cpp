// The mail function will onl;y initialize en create the needed threads.
// There will be 2 or 3 network related threads
// the UPD network protocall will have 2 functions:
// 1. To identify this device's IP adress when requested with a UDP broadcast (If this work, not implimnented to test yet)
// 2. Remote shut down of this application. The UDP thread will be joined to the main thread, blocking it until the UDP thread exits.
// When the main function exits, all threads will be released.
// TODO, investigate if I cal clode threads from the main function.
// TCP will be used to transmit the sensor data to the upstream database. I decided on TCP for several reasons
// 1. It's reliable, dropped packets should not be a major concern
// 2. Security. I need to impliment SSL encryption. If I transmit data upstream tio the remote database I need to encrypt the DB password being transmitted.
//    TCP with SSL will also be used to transmit settings to this device. This is needed if passwords is updated.

// TODO Impliment a config file and encrypt the config file.

#include "WeatherPiSystem.hpp"
#include "WeatherPiNetThreads.hpp"
#include <iostream>
#include <iomanip>
#include <string.h>
#include <math.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <cstdlib>
#include <pthread.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime> 

int main(void)
{	
	WeatherPiSystemInitialize();

	// This thread will transmit measured data to the server over TCP even 10 minutes.
	// This TCP thread will not accept any incomming connections.
	// All the emasurements will be innitiated within this thread.
	//pthread_t tcpTimerThread;										// Disable the thread while testing to preserve the SD card.
	//int tcpPort = 8080;
	//pthread_create(&tcpTimerThread, NULL, tcpDataTransmitTimer, (void*)tcpPort);

	// This thread accept UDP datagrams and will be used for basic controls
	// This thread will block the main function and in this way control when the application exits.
	// The exit cammand can only be sent over UPD to this thread.
	pthread_attr_t attr;
	void* status;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_t udpThread;
	int udpPort = 8080;
	pthread_create(&udpThread, &attr, udpNet, (void*)udpPort);		// Created a joinable thread, That is what &attr is for
	pthread_join(udpThread, &status);								// Join the Thread. Now the main function will block and wait for the udpNet function to finish.	

	oledDisplay.DisplayOff();
	oledDisplay.Close();
	lightningDetector.Close();
	lcd.Close();
	nrf24.Close();
	sensorBME280.Close();

	std::cout << "Releasing GPIO Library" << std::endl;
	gpioTerminate();
	return 0;
}
