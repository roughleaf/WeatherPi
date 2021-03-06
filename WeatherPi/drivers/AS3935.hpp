#pragma once
#include <pigpio.h>

class AS3935
{
private:
	int handle = -1;

public:
	int LightningStrikeCount = 0;
	int DisturberCount = 0;

	int Initialize(int channel);
	unsigned char ReadRegister(char reg);
	int SetRegister(unsigned char reg, unsigned char value);
	void Close(void);
};

