#include "AS3935.hpp"
#include <unistd.h>

int AS3935::Initialize(int channel)
{
	if (handle < 0)		//If already open, ignore and return -1.
	{
		handle = spiOpen(channel, 100000, 1);

		SetRegister(0x3C, 0x96);	// Set all registers to default
		usleep(2000);
		SetRegister(0x3D, 0x96);	// Calibrates automatically the internal RC Oscillators
		usleep(2000);
		SetRegister(0x00, 0x24);	// Default
		usleep(2000);
		SetRegister(0x01, 0x22);	// Default
		usleep(2000);
		SetRegister(0x02, 0xC2);	// Default
		usleep(2000);
		SetRegister(0x03, 0x00);	// Default		usleep(2000);
		usleep(2000);

		return handle;
	}
	else
	{
		return -1;
	}
}

unsigned char AS3935::ReadRegister(unsigned char reg)
{
	char txBuff[2] = { reg | 0x40, 0x00 };	// was char txBuff[2] = { reg | 0x40, 0xC2 };
	char rxBuff[2] = { 0xFF, 0xFF };

	spiXfer(handle, txBuff, rxBuff, 2);

	return rxBuff[1];
}

int AS3935::SetRegister(unsigned char reg, unsigned char value)
{
	char txBuff[2] = { reg, value };

	spiWrite(handle, txBuff, 2);

	return 0;
}
