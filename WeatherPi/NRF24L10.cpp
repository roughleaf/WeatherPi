#include "NRF24L10.hpp"
#include <unistd.h>
#include <string.h>
#include <iostream>

int NRF24L10::Initialize(int channel)
{
	if (handle < 0)		//If already open, ignore and return -1.
	{
		gpioWrite(NRF24_CE, 0);
		handle = spiOpen(channel, 100000, 0);

		WriteRegister(CONFIG_REG, 0x0F);	// RX, TX & Max Retry interrupt enabled, PRX mode, Power up. Use TX interrupt to return device to PRX Mode after transmit.
		//WriteRegister(EN_AA_REG, 0x3F);	// Enable auto acknowledge for all data pipes
		//WriteRegister(EN_RXADDR_REG, 0x3F);	// Enable all data pipe RX addresses

		WriteRegister(EN_AA_REG, 0x3F);	// Enable auto acknowledge for all data pipes
		WriteRegister(EN_RXADDR_REG, 0x3F);	// Enable all data pipe RX addresses

		WriteRegister(SETUP_AW_REG, 0x03);
		WriteRegister(SETUP_RETR_REG,0xF3 );
		WriteRegister(SETUP_AW_REG, 0x03);
		WriteRegister(SETUP_RETR_REG, 0xF3);
		WriteRegister(RF_CH_REG, 0x50);		// Set to 2480Mhz, outer edge of chanel 13 but still within legal limits
		WriteRegister(RF_SETUP_REG, 0x02);	// Set gain to minimum for testing
		WriteRegister(STATUS_REG, 0x70);
		WriteRegister(RX_PW_P0_REG, 32);		// Set receive payload width to 32 bytes
		WriteRegister(RX_PW_P1_REG, 32);		// Set receive payload width to 32 bytes
		WriteRegister(RX_PW_P2_REG, 32);		// Set receive payload width to 32 bytes
		WriteRegister(RX_PW_P3_REG, 32);		// Set receive payload width to 32 bytes
		WriteRegister(RX_PW_P4_REG, 32);		// Set receive payload width to 32 bytes
		WriteRegister(RX_PW_P5_REG, 32);		// Set receive payload width to 32 bytes

		char rxAddrP0[5] = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 };
		//char RxAddr[5] = { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 };

		char ChanP0[5] = { 9, 9, 9, 9, 9 };

		char RxAddr[5] = { 1, 2, 3, 4, 5 };

		RxAddr[4] = 0;
		WriteRegisterBytes(RX_ADDR_P0_REG, ChanP0, 5);
		
		RxAddr[4] = 1;
		WriteRegisterBytes(RX_ADDR_P1_REG, RxAddr, 5);		
		
		WriteRegister(RX_ADDR_P2_REG, 2);
		WriteRegister(RX_ADDR_P3_REG, 3);
		WriteRegister(RX_ADDR_P4_REG, 4);
		WriteRegister(RX_ADDR_P5_REG, 5);
		
		//WriteRegisterBytes(0x10, RxAddr, 5);
		

		FlushRX();
		FlushTX();
		PRXmode();
		return handle;
	}
	else
	{
		return -1;
	}
}

char NRF24L10::ReadRegister(unsigned char reg)
{
	//char txBuff[2] = { reg | 0x40, 0x00 };	// was char txBuff[2] = { reg | 0x40, 0xC2 };
	char txBuff[2] = { reg , 0x00 };
	char rxBuff[2] = { 0xFF, 0xFF };

	spiXfer(handle, txBuff, rxBuff, 2);

	return rxBuff[1];
}

char NRF24L10::ReadRegisterBytes(unsigned char reg, char* buff, int len)
{
	char txBuff[6] = { reg, 0x00 };
	//char rxBuff[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	char status = 0;

	spiXfer(handle, txBuff, buff, len+1);

	status = buff[0];

	for (int x = 1; x < len + 1; x++)
	{
		buff[x-1] = buff[x];	// remove the status bit
	}
	return status;
}

int NRF24L10::WriteRegister(unsigned char reg, unsigned char value)
{
	char txBuff[2] = { reg | 0x20, value };

	spiWrite(handle, txBuff, 2);

	return 0;
}

int NRF24L10::WriteRegisterBytes(unsigned char reg, char* value, int len)
{
	reg |= 0x20;
	char buff[len + 1];
	buff[0] = reg;
	for (int x = 1; x < len + 1; x++)
	{
		buff[x] = value[x - 1];
	}

	spiWrite(handle, buff, len+1);

	return 0;
}

char NRF24L10::ReadPayload(char* rxBuff, int len)		// TODO: Add length argument. length will be given in RX register.
{
	//int length = 32;
	char txBuff[33] = { R_RX_PAYLOAD, 0x00 };
	char status = 0;

	spiXfer(handle, txBuff, rxBuff, len + 1);

	status = rxBuff[0];

	for (int x = 1; x < len + 1; x++)
	{
		rxBuff[x - 1] = rxBuff[x];	// remove the status bit
	}
	return status;
}

int NRF24L10::LoadPayload(const char* txBuff)
{
	char instruction = W_TX_PAYLOAD;
	int len = 32;//strlen(txBuff);
	char buff[len + 1];
	buff[0] = instruction;			// Add instruction byte
	for (int x = 1; x < len + 1; x++)
	{
		buff[x] = txBuff[x - 1];	// Add payload bytes after instruction byte
	}

	//spiWrite(handle, buff, len + 1);
	spiWrite(handle, buff, 33);		// Make sure only 32 bytes gets transmitted for testing

	return 0;
}

int NRF24L10::TransmitToChannel(const char* txBuff, char channel)
{
	char chan[5] = { 1, 2, 3, 4, (channel) };
	/*if (channel == 0)
	{
		for (int x = 0; x < 5; x++)
		{
			chan[x] = 0xE7;
		}
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
		std::cout << "Write to channel 0 =====++++++" << std::endl;
	}
	else if (channel == 1)
	{
		for (int x = 0; x < 5; x++)
		{
			chan[x] = 0xC2;
		}
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
		std::cout << "Write to channel 1 =====++++++" << std::endl;
	}
	else if (channel == 2)
	{
		for (int x = 0; x < 4; x++)
		{
			chan[x] = 0xC2;
		}
		chan[4] = 0xC3;
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
		std::cout << "Write to channel 2 =====++++++" << std::endl;
	}
	else if (channel == 3)
	{
		for (int x = 0; x < 4; x++)
		{
			chan[x] = 0xC2;
		}
		chan[4] = 0xC4;
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
	}
	else if (channel == 4)
	{
		for (int x = 0; x < 4; x++)
		{
			chan[x] = 0xC2;
		}
		chan[4] = 0xC5;
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
	}
	else if (channel == 5)
	{
		for (int x = 0; x < 4; x++)
		{
			chan[x] = 0xC2;
		}
		chan[4] = 0xC6;
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
	}
	else return 0;*/

	PTXmode();					// Set PTX mode
	FlushTX();
	if (channel == 0)
	{
		char chanP0[5] = { 9, 9, 9, 9, 9 };
		WriteRegisterBytes(RX_ADDR_P0_REG, chanP0, 5);
		WriteRegisterBytes(TX_ADDR_REG, chanP0, 5);		// Set channel to write to and automatic acknowledge		
	}
	else
	{
		WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
		WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge
	}

	LoadPayload(txBuff);

	gpioWrite(NRF24_CE, 1);
	usleep(15);
	gpioWrite(NRF24_CE, 0);							// Signal to send

	//chan[4] = 0;
	//WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);	// Reset auto ack to datapipe 0 address

	//PRXmode();

	return 0;
}

char NRF24L10::ReadStatus(void)
{
	char txBuff[1] = { 0xFF };
	char rxBuff[1] = { 0xFF };

	spiXfer(handle, txBuff, rxBuff, 1);

	return rxBuff[0];
}

void NRF24L10::PTXmode(void)
{
	gpioWrite(NRF24_CE, 0);
	FlushTX();
	char conf = ReadRegister(CONFIG_REG);
	conf &= 0xFE;
	WriteRegister(CONFIG_REG, conf);
}

void NRF24L10::PRXmode(void)
{
	FlushRX();
	unsigned char conf = ReadRegister(CONFIG_REG);
	conf |= 0x01;
	WriteRegister(CONFIG_REG, conf);
	gpioWrite(NRF24_CE, 1);
}

void NRF24L10::FlushTX(void)
{
	char buff[1] = { 0xE1 };
	spiWrite(handle, buff, 1);
}

void NRF24L10::FlushRX(void)
{
	char buff[1] = { 0xE2 };
	spiWrite(handle, buff, 1);
}

void NRF24L10::ResetRXAddr(void)
{
	//char chan[5] = { 1, 2, 3, 4, 0 };
	//WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);    // Setup Receive Datapipe to Address 0

	char chanP0[5] = { 9, 9, 9, 9, 9 };
	WriteRegisterBytes(RX_ADDR_P0_REG, chanP0, 5);
}