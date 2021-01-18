#include "I2CLCD.hpp"
#include <unistd.h>
#include <bitset>
#include <string.h>

int I2CLCD::WriteNibble(unsigned char nibble, unsigned char rs)
{
	nibble |= rs;
	nibble |= BacklightOnOff;		// Turn on backlight

	i2cWriteByte(handle, nibble | 0x04);
	i2cWriteByte(handle, nibble & 0xFB);
	usleep(50);

	return 0;
}

int I2CLCD::Initialize(const unsigned char i2cAddr)
{
	return Initialize(1, i2cAddr);
}

int I2CLCD::Initialize(const unsigned char i2cBus, const unsigned char i2cAddr)
{
	handle = i2cOpen(i2cBus, i2cAddr, 0);
	
	if (handle >= 0)
	{

		i2cWriteByte(handle, 0x00);
		usleep(30000);
		WriteCommand(0x03);
		usleep(5000);
		WriteCommand(0x03);
		usleep(5000);
		WriteCommand(0x03);
		usleep(5000);
		WriteCommand(0x02);
		usleep(5000);
		WriteCommand(0x20 | (2 << 2));
		usleep(50000);
		WriteCommand(0x0C);
		usleep(50000);
		WriteCommand(0x01);
		usleep(50000);
		WriteCommand(0x04 | 0x02);
		usleep(50000);
	}
	
	return handle;

}

int I2CLCD::BacklightOn()
{
	BacklightOnOff = 0x08;

	return 0;
}

int I2CLCD::BacklightOff()
{
	BacklightOnOff = 0x00;

	return 0;
}

int I2CLCD::BacklightToggle()
{
	if (BacklightOnOff == 0x00)
	{
		BacklightOnOff = 0x08;
		i2cWriteByte(handle, 0x08);
	}
	else
	{
		BacklightOnOff = 0x00;
		i2cWriteByte(handle, 0x00);
	}
	return 0;

	Home();

}

int I2CLCD::WriteCommand(const unsigned char command)
{
	unsigned char rs = 0;

	WriteNibble(command & 0xF0, rs);
	WriteNibble((command << 4) & 0xF0, rs);
		
	return 0;
}

int I2CLCD::WriteCharacter(const unsigned char data)
{
	unsigned char rs = 1;

	WriteNibble(data & 0xF0, rs);
	WriteNibble((data << 4) & 0xF0, rs);

//	usleep(50);

	return 0;
}

int I2CLCD::WriteString(const char* buf)
{
	for (unsigned int i = 0; i < (strlen(buf)); i++)
	{
		WriteCharacter(buf[i]);
	}

	return 0;
}

int I2CLCD::Clear()
{
	WriteCommand(LCD_CLEAR);
	usleep(1300);

	return 0;
}

int I2CLCD::On()
{
	WriteCommand(LCD_TURN_ON);
	usleep(50);

	return 0;
}

int I2CLCD::Off()
{
	WriteCommand(LCD_TURN_OFF);
	usleep(50);

	return 0;
}

int I2CLCD::Home()
{
	WriteCommand(LCD_HOME);
	usleep(1300);
	return 0;
}

void I2CLCD::Close(void)
{
	i2cClose(handle);
}
