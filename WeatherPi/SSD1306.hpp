#pragma once
#include <pigpio.h>
#include "Mono18pt7b.hpp"
#include "GFXFonts.hpp"

class SSD1306
{
	public: struct Command
	{
		const unsigned char FUNDAMENTAL_SET_CONTRAST_CONTROL = 0x81;
		const unsigned char FUNDAMENTAL_ENTIRE_DISPLAY_ON_FOLLOW_RAM = 0xA4;
		const unsigned char FUNDAMENTAL_ENTIRE_DISPLAY_ON_IGNORE_RAM = 0xA5;
		const unsigned char FUNDAMENTAL_DISPLAY_INVERSE_OFF = 0xA6;
		const unsigned char FUNDAMENTAL_DISPLAY_INVERSE_ON = 0xA7;
		const unsigned char FUNDAMENTAL_DISPLAY_SLEEP = 0xAE;
		const unsigned char FUNDAMENTAL_DISPLAY_ON_NORMAL = 0xAF;
		const unsigned char HORIZONTAL_ADDRESSING_MODE = 0x00;
		const unsigned char VERTICLE_ADDRESSING_MODE = 0x01;
		const unsigned char PAGE_ADDRESSING_MODE = 0x02;

	};	
	Command command;

private:
	int handle = -1;
	char buffer[8][128];
	bool inverted = false;
	bool flippedVertically = false;
	bool flippedHorizontally = false;
	const GFXfont font = {
  (uint8_t*)FreeMono18pt7bBitmaps,
  (GFXglyph*)FreeMono18pt7bGlyphs,
  0x20, 0x7E, 35 };

	int DrawCharacter(const unsigned char character, int x, int y);

public:
	SSD1306();
	~SSD1306();
	int Initialize(const unsigned char i2cAddr);
	int Initialize(const unsigned char i2cBus, const unsigned char i2cAddr);
	int Close(void);
	int WriteCommand(const unsigned char cmd);
	int WriteData(const unsigned char data);
	int WriteDataString(char* buf);
	int InvertScreen(void);
	int DisplayOff(void);
	int DisplayOn(void);
	int Display(void);
	int ClearScreen(void);
	int FillScreen(void);
	int FlipDisplayVertically(void);
	int FlipDisplayHorizontally(void);
	int PrintC(const unsigned char character);
	void FontTest(void);
	int DrawPixel(int x, int y, int colour);
	int CharArrayTest(void);
	int PrintS(const char* buf, int x, int y);
};

