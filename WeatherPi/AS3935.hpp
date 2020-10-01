#pragma once
#include <pigpio.h>

class AS3935
{
private:
	int handle = -1;

public:
	int Initialize(int channel);
	unsigned char ReadRegister(unsigned char reg);
	int SetRegister(unsigned char reg, unsigned char value);
};

