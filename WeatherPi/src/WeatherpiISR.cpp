#include "WeatherpiISR.hpp"

void As3935Interrupt(int gpio, int level, uint32_t tick)
{
	std::string count = "";
	int dist = -1;

	systemTime.GetSystemDateTime();
	std::cout << '\n' << systemTime.SystemDateTime << " AS3935 Interrupt Triggered\n" << std::endl;

	usleep(5000);						// Wait 5 ms for AS3935 to finish operations

	int Lint = (int)lightningDetector.ReadRegister(0x03) & 0x0F;

	std::cout << "\t\t - Register 3: " << Lint << std::endl;
	usleep(2000);

	//lightningDetector.SetRegister(0x07, 0);
	switch (Lint)
	{
	case 8:
	{
		count = std::to_string(++lightningDetector.LightningStrikeCount);
		dist = lightningDetector.ReadRegister(0x07);

		lightningData.AddLightningStrike(dist);

		std::cout << "\t\t - Lightning detected" << std::endl;
		std::cout << "\t\t - Lightning Distance estimation: " << dist << "km" << std::endl;
	}
	break;
	case 4:
	{
		count = std::to_string(++lightningDetector.DisturberCount);
		dist = lightningDetector.ReadRegister(0x07);

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
	std::cout << '\n' << systemTime.GetSystemDateTime() << " Entered nRF24L01+ interrupt callback" << std::endl;

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
			char datetimeTest[13] = { 0 };
			icodec::BuildTimeDateByteString(datetimeTest, NodeID);
			// Seems possible to transmit from this thread. Needs more testing.
			// Fails when console output not active (nohup). Then the RTC values on remoteSensorNode stays reset values
			nrf24.TransmitData(datetimeTest, 13);
			std::cout << "\n\t\t - Node ID: " << NodeID << " requested Date and Time" << std::endl;
			std::cout << "\t\t - Date and time transmitted to SensorNode #" << NodeID << std::endl;
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
	usleep(3000);	// Delay needed to fix bug where Date and time is not transmitted when console not active. 
	nrf24.WriteRegister(0x07, (status | 0x70));		// Ensure that all interrupts are clear
	nrf24.PRXmode();	// This device will primarily be an PRX device
}