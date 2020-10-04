#pragma once
#include <pigpio.h>

class I2CLCD
{
private:
	int handle;
	int WriteNibble(unsigned char nibble, unsigned char rs);

public:
	int Initialize(const unsigned char i2cAddr);
	int Initialize(const unsigned char i2cBus, const unsigned char i2cAddr);
	int WriteCommand(const unsigned char command);
	int WriteCharacter(const unsigned char data);
	int WriteString(const char* buf);
};

