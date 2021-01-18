#pragma once
#include <pigpio.h>

#define LCD_FIRST_ROW			0x80
#define LCD_SECOND_ROW			0xC0
#define LCD_THIRD_ROW			0x94
#define LCD_FOURTH_ROW			0xD4
#define LCD_CLEAR				0x01
#define LCD_HOME				0x02
#define LCD_ENTRY_MODE_SET		0x04
#define LCD_CURSOR_OFF			0x0C
#define LCD_UNDERLINE_ON		0x0E
#define LCD_BLINK_CURSOR_ON		0x0F
#define LCD_MOVE_CURSOR_LEFT	0x10
#define LCD_MOVE_CURSOR_RIGHT	0x14
#define LCD_TURN_ON				0x0C
#define LCD_TURN_OFF			0x08
#define LCD_SHIFT_LEFT			0x18
#define LCD_SHIFT_RIGHT			0x1E

class I2CLCD
{
private:
	int handle;
	int WriteNibble(unsigned char nibble, unsigned char rs);
	unsigned char BacklightOnOff = 0x08;

public:
	int Initialize(const unsigned char i2cAddr);
	int Initialize(const unsigned char i2cBus, const unsigned char i2cAddr);
	int BacklightOn();
	int BacklightOff();
	int BacklightToggle();
	int WriteCommand(const unsigned char command);
	int WriteCharacter(const unsigned char data);
	int WriteString(const char* buf);
	int Clear();
	int On();
	int Off();
	int Home();
	void Close(void);
};

