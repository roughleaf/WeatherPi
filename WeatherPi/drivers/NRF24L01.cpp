#include "NRF24L01.hpp"
#include <unistd.h>
#include <string.h>

int NRF24L01::Initialize(int channel)
{
	if (handle < 0)		//If already open, ignore and return -1.
	{
		gpioWrite(NRF24_CE, 0);
		handle = spiOpen(channel, 100000, 0);

		WriteRegister(CONFIG_REG, 0x0D);	// RX, TX & Max Retry interrupt enabled, PRX mode, Power Down, 2 byte CRC
		WriteRegister(EN_RXADDR_REG, 0x01);	// Enable only datapipe 0
		WriteRegister(EN_AA_REG, 0x01);	// Enable auto acknowledge for datapipe 0
		WriteRegister(SETUP_AW_REG, 0x03);
		WriteRegister(SETUP_RETR_REG, 0xF3);
		WriteRegister(RF_CH_REG, 0x50);		// Set to 2480Mhz, outer edge of chanel 13 but still within legal limits
		WriteRegister(RF_SETUP_REG, 0x0F);	// Set gain to minimum for testing
		WriteRegister(STATUS_REG, 0x70);

		Activate();							// Enable R_RX_PL_WID, DYNPD & FEATURE

		WriteRegister(DYNPD_REG, 0x01);		// Enable dynamic data width on datapipes 0
		WriteRegister(FEATURE_REG, 0x04);	// Enable dynamic data width feature

		WriteRegisterBytes(RX_ADDR_P0_REG, channelP0, 5);
		WriteRegisterBytes(TX_ADDR_REG, channelP0, 5);		

		FlushTX();
		FlushRX();

		Powerup();
		gpioWrite(NRF24_CE, 1);

		return handle;
	}
	else
	{
		return -1;
	}
}

char NRF24L01::ReadRegister(unsigned char reg)
{
	//char txBuff[2] = { reg | 0x40, 0x00 };	// was char txBuff[2] = { reg | 0x40, 0xC2 };
	char txBuff[2] = { reg , 0x00 };
	char rxBuff[2] = { 0xFF, 0xFF };

	spiXfer(handle, txBuff, rxBuff, 2);

	return rxBuff[1];
}

char NRF24L01::ReadRegisterBytes(unsigned char reg, char* buff, int len)
{
	char txBuff[6] = { reg, 0x00 };
	char status = 0;

	spiXfer(handle, txBuff, buff, len+1);

	status = buff[0];

	for (int x = 1; x < len + 1; x++)
	{
		buff[x-1] = buff[x];	// remove the status bit
	}
	return status;
}

int NRF24L01::WriteRegister(unsigned char reg, unsigned char value)
{
	char txBuff[2] = { reg | 0x20, value };

	spiWrite(handle, txBuff, 2);

	return 0;
}

int NRF24L01::WriteRegisterBytes(unsigned char reg, char* value, int len)
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

char NRF24L01::ReadPayload(char* rxBuff, int len)		// TODO: Add length argument. length will be given in RX register.
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

int NRF24L01::LoadPayload(const char* txBuff, int length)
{
	char instruction = W_TX_PAYLOAD;					//strlen cannot be used because the send array do not have a '\0'
	char buff[length + 1];
	buff[0] = instruction;			// Add instruction byte
	for (int x = 1; x < length + 1; x++)
	{
		buff[x] = txBuff[x - 1];	// Add payload bytes after instruction byte
	}

	spiWrite(handle, buff, length + 1);

	return 0;
}

int NRF24L01::TransmitData(const char* txBuff, int length)
{
	PTXmode();					// Set PTX mode

	LoadPayload(txBuff, length);

	gpioWrite(NRF24_CE, 1);
	usleep(15);
	gpioWrite(NRF24_CE, 0);							// Signal to send

	return 0;
}

char NRF24L01::ReadStatus(void)
{
	char txBuff[1] = { 0xFF };
	char rxBuff[1] = { 0xFF };

	spiXfer(handle, txBuff, rxBuff, 1);

	return rxBuff[0];
}

void NRF24L01::PTXmode(void)
{
	gpioWrite(NRF24_CE, 0);
	char conf = ReadRegister(CONFIG_REG);
	conf &= 0xFE;
	WriteRegister(CONFIG_REG, conf);
	FlushTX();
}

void NRF24L01::PRXmode(void)
{
	unsigned char conf = ReadRegister(CONFIG_REG);
	conf |= 0x01;
	WriteRegister(CONFIG_REG, conf);
	FlushRX();
	gpioWrite(NRF24_CE, 1);
}

void NRF24L01::FlushTX(void)
{
	char buff[1] = { 0xE1 };
	spiWrite(handle, buff, 1);
}

void NRF24L01::FlushRX(void)
{
	char buff[1] = { 0xE2 };
	spiWrite(handle, buff, 1);
}

void NRF24L01::ResetRXAddr(void)
{
	WriteRegisterBytes(RX_ADDR_P0_REG, channelP0, 5);
}

void NRF24L01::Powerdown(void)
{
	char config = ReadRegister(CONFIG_REG);
	WriteRegister(CONFIG_REG, (config & 0xFD));
	usleep(1500);
}

void NRF24L01::Powerup(void)
{
	char config = ReadRegister(CONFIG_REG);
	WriteRegister(CONFIG_REG, (config | 0x02));
	usleep(1500);								//1.5ms delay while internal oscelator powers up
}

void NRF24L01::Activate(void)
{
	if (ReadRegister(DYNPD_REG) == 0)			// Check if Activate have been run to avoid disabing the features with a program restart without a power cycle
	{
		char spiCommand[2] = { ACTIVATE , 0x73 };
		spiWrite(handle, spiCommand, 2);
	}
}

int NRF24L01::GetRXWidth(void)
{	
	char rxWidth[2] = { 0, 0 };
	char txCommand[2] = { R_RX_PL_WID , 0 };
	spiXfer(handle, txCommand, rxWidth, 2);
	return rxWidth[1];
}

void NRF24L01::Close(void)
{
	spiClose(handle);
}
