#include "NRF24L10.hpp"
#include <unistd.h>
#include <string.h>

int NRF24L10::Initialize(int channel)
{
	if (handle < 0)		//If already open, ignore and return -1.
	{
		gpioWrite(NRF24_CE, 0);
		handle = spiOpen(channel, 100000, 0);

		WriteRegister(CONFIG_REG, 0x1B);	// RX & TX interrupt enabled, PRX mode, Power up. Use TX interrupt to return device to PRX Mode after transmit.
		WriteRegister(EN_RXADDR_REG, 0x3F);	// Enable all data pipe RX addresses
		WriteRegister(RF_CH_REG, 0x50);		// Set to 2480Mhz, outer edge of chanel 13 but still within legal limits
		WriteRegister(RF_SETUP_REG, 0x01);	// Set gain to minimum for testing

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

char NRF24L10::ReadPayload(char* rxBuff)		// TODO: Add length argument. length will be given in RX register.
{
	int len = 32;
	char txBuff[34] = { R_RX_PAYLOAD, 0x00 };
	char status = 0;

	spiXfer(handle, txBuff, rxBuff, len + 1);

	status = rxBuff[0];

	for (int x = 1; x < len + 1; x++)
	{
		rxBuff[x - 1] = rxBuff[x];	// remove the status bit
	}
	return status;
}

int NRF24L10::LoadPayload(char* txBuff)
{
	char instruction = W_TX_PAYLOAD;
	int len = strlen(txBuff);
	char buff[len + 1];
	buff[0] = instruction;			// Add instruction byte
	for (int x = 1; x < len + 1; x++)
	{
		buff[x] = txBuff[x - 1];	// Add payload bytes after instruction byte
	}

	spiWrite(handle, buff, len + 1);

	return 0;
}

int NRF24L10::TransmitToChannel(char* txBuff, char channel)
{
	char chan[5] = { 0x1F, 0x2F, 0x3F, 0x4F, channel };

	char conf = ReadRegister(CONFIG_REG);
	conf &= 0xFE;
	WriteRegister(CONFIG_REG, conf);					// Set PTX mode

	WriteRegisterBytes(RX_ADDR_P0_REG, chan, 5);
	WriteRegisterBytes(TX_ADDR_REG, chan, 5);		// Set channel to write to and automatic acknowledge

	LoadPayload(txBuff);

	gpioWrite(NRF24_CE, 1);
	usleep(15);
	gpioWrite(NRF24_CE, 0);							// Signal to send

	return 0;
}

