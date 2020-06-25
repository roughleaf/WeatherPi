// SSD1306 Driver under development
// Not recomended for use as currently is

#include "SSD1306.hpp"
#include <string.h>
#include <cmath>
#include <vector>
#include <algorithm>

SSD1306::SSD1306()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			buffer[y][x] = 0;
		}
	}
}

SSD1306::~SSD1306()
{
	return;
}

int SSD1306::Initialize(const unsigned char i2cAddr)
{
	handle = i2cOpen(1, i2cAddr, 0);

	std::vector<unsigned char> commands = { 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0x7F, 0xA4, 0xA6, 0xD5, 0x80, 0x8D, 0x14, 0xAF };

	for (unsigned char commandValue : commands)
	{
		WriteCommand(commandValue); 
	}

	ClearBuffer();

	return 0;
}

int SSD1306::Initialize(const unsigned char i2cBus, const unsigned char i2cAddr)
{
	handle = i2cOpen(i2cBus, i2cAddr, 0);

	std::vector<unsigned char> commands = { 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0x7F, 0xA4, 0xA6, 0xD5, 0x80, 0x8D, 0x14, 0xAF };

	for (unsigned char commandValue : commands)
	{
		WriteCommand(commandValue);
	}

	ClearBuffer();

	return 0;
}

int SSD1306::Close(void)
{
	i2cClose(handle);
	return 0;
}

int SSD1306::DrawCharacter(const unsigned char character, int x, int y)
{
	if (character == ' ')
	{
		return 0;
	}
	int charIndex = character - font.first;
	int charBitmapByteSize = font.glyph[charIndex + 1].bitmapOffset - font.glyph[charIndex].bitmapOffset;
	char* charData = new char[charBitmapByteSize];
	int bitmapOffsetEnd = font.glyph[charIndex].bitmapOffset + charBitmapByteSize;
	int columnStart = font.glyph[charIndex].xOffset + x;
	int columnEnd = columnStart + font.glyph[charIndex].width;
	int columnWidth = columnEnd - columnStart;
	int bitmapHeight = (charBitmapByteSize * 8) / columnWidth;

	int charBase = y + font.glyph['|' - font.first].height;

	for (int i = font.glyph[charIndex].bitmapOffset; i < bitmapOffsetEnd; i++)
	{
		charData[i - font.glyph[charIndex].bitmapOffset] = font.bitmap[i];
	}

	int bitCount = charBitmapByteSize * 8;
	int bits[bitCount + 1];
	for (int i = 0; i < charBitmapByteSize; i++)
	{
		int k = 7;
		for (int j = 0; j < 8; j++)
		{
			int temp = charData[i] & (int)pow(2, j);
			if (temp != 0)
			{
				bits[(i * 8) + k] = 1;
			}
			else
			{
				bits[(i * 8) + k] = 0;
			}
			k--;
		}

	}

	int bitCounter = 0;

	//int dY = charBase + font.glyph[character].yOffset;
	int dY = charBase - bitmapHeight;

	//for (int i = y; i < (bitmapHeight + y); i++)
	for (int i = dY; i < (bitmapHeight + dY); i++)
	{
		for (int j = x; j < (columnWidth + x); j++)
		{
			DrawPixel(j, i, bits[bitCounter]);
			++bitCounter;
		}
	}

	delete(charData);
	return 0;
}

int SSD1306::WriteCommand(const unsigned char cmd)
{
	return i2cWriteByteData(handle, 0x00, cmd);
}

int SSD1306::WriteData(const unsigned char data)
{
	i2cWriteByteData(handle, 0x40, data);
	return 0;
}

int SSD1306::WriteDataString(char* buf)
{
	if(strlen(buf) <= 32)
	{
		i2cWriteI2CBlockData(handle, 0x40, buf, strlen(buf));
	}
	else
	{
		return -1;
	}

	return 0;
}

int SSD1306::InvertScreen(void)
{
	if (inverted)
	{
		inverted = !inverted;
		return WriteCommand(0xA6);
	}
	else
	{
		inverted = !inverted;
		return WriteCommand(0xA7);
	}
	return 0;
}

int SSD1306::DisplayOff(void)
{
	WriteCommand(0xAE);
	return 0;
}

int SSD1306::DisplayOn(void)
{
	WriteCommand(0xAF);
	return 0;
}

int SSD1306::Display(void)
{
	WriteCommand(0x20);			// Set Memory Addressing Mode
	WriteCommand(0x00);			// Set to Horizontal Scanning
	WriteCommand(0x22);			// Set Page Adress
	WriteCommand(0x00);			// Set to start at Page 0
	WriteCommand(0x07);			// Set end to Page 7
	WriteCommand(0x21);			// Set Column Address
	WriteCommand(0);			// Set Column start Address
	WriteCommand(127);			// Set Column End Adress

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			i2cWriteI2CBlockData(handle, 0x40, &buffer[y][x*32], 32);
		}
	}
	return 0;
}

int SSD1306::ClearScreen(void)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			buffer[y][x] = 0;
		}
	}
	
	Display();

	return 0;
}

int SSD1306::ClearBuffer(void)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			buffer[y][x] = 0;
		}
	}

	return 0;
}

int SSD1306::FillScreen(void)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			buffer[y][x] = 0xff;
		}
	}

	Display();

	return 0;
}

int SSD1306::FlipDisplayVertically(void)
{
	if (flippedVertically)
	{
		flippedVertically = !flippedVertically;
		WriteCommand(0xC0);
	}
	else
	{
		flippedVertically = !flippedVertically;
		WriteCommand(0xC8);
	}
	return 0;
}

int SSD1306::FlipDisplayHorizontally(void)
{
	if (flippedHorizontally)
	{
		flippedHorizontally = !flippedHorizontally;
		WriteCommand(0xA0);
		Display();		
	}
	else
	{
		flippedHorizontally = !flippedHorizontally;
		WriteCommand(0xA1);
		Display();
	}
	return 0;
}

// Test that write single character at the top corner of the OLED
// I should add a arguments to set the X and Y coordinates
int SSD1306::PrintC(const unsigned char character)
{
	int charIndex = character - font.first;
	int charBitmapByteSize = font.glyph[charIndex + 1].bitmapOffset - font.glyph[charIndex].bitmapOffset;
	
	DrawCharacter(character, 0, 0);

	return 0;
}

// BUG: the correct pixel gets set, but not cleared.
int SSD1306::DrawPixel(int x, int y, int colour)
{
	if (colour != 0 && colour != 1)
	{
		return -1;
	}

	int yBit = colour;
	int page = y / 8;
	int shift = y % 8;
	yBit = yBit << shift;
	int mask = yBit ^ 0xff;

	buffer[page][x] &= mask;	// Clear the pixel
	buffer[page][x] |= yBit;	// Set the pixel to value of colour

	return 0;
}

//Bug: xAdvance is not always the expected value.
int SSD1306::PrintS(const char* buf, int x, int y) // TODO: Add protection to prevent buf size from extending out of bounds.
{
	int xAdvance = x;

	for (int i = 0; i < (strlen(buf)); i++)
	{
		DrawCharacter(buf[i], xAdvance, y);
		xAdvance += font.glyph[buf[i+font.first]].xAdvance;
	}
	return 0;
}
