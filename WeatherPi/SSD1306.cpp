#include "SSD1306.hpp"
#include <iostream>
#include <string.h>
#include <cmath>

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

	//std::cout << "|a===========================================================================|" << std::endl;
	//std::cout << "Index being written: ";

	for (int i = font.glyph[charIndex].bitmapOffset; i < bitmapOffsetEnd; i++)
	{
		charData[i - font.glyph[charIndex].bitmapOffset] = font.bitmap[i];
	//	std::cout << i << " ";
	}
	//std::cout << "|b===========================================================================|" << std::endl;
	
	/*
	WriteCommand(0x20);		// Set Memory Addressing Mode
	WriteCommand(0x00);		// Set to Horizontal Scanning

	//================ TODO, set this up properly =====================
	WriteCommand(0x22);			// Set Page Adress
	WriteCommand(0x00);			// Set to start at Page 0
	WriteCommand(0x07);			// Set end to Page 7

	WriteCommand(0x21);			// Set Column Address
	WriteCommand(0);	// Set Column start Address
	WriteCommand(24);	// Set Column End Adress
	std::cout << "============================================================================" << std::endl;
	std::cout << "Column Start = " << (int)columnStart << std::endl;
	std::cout << "Column End = " << (int)columnEnd << std::endl;
	std::cout << "============================================================================" << std::endl;
	*/
	

	//std::cout << "|c==========================================================================|" << std::endl;
	//std::cout << "Writing to OLED Display: ";

	int bitCount = charBitmapByteSize * 8;
	int bits[bitCount+1];
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

	//for (int i = 0; i < bitCount; i++)
	//{
	//	std::cout << bits[i] << ", ";
	//}

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
	//std::cout << "bitCounter : " << bitCounter << std::endl;
	//std::cout << "bitCount : " << bitCount << std::endl;
	//std::cout << "bitmapHeight : " << bitmapHeight << std::endl;
	//std::cout << "columnWidth : " << columnWidth << std::endl;
	//std::cout << "charBase : " << charBase << std::endl;
	//std::cout << "dY : " << dY << std::endl;
	//std::cout << "font.glyph['|'].height : " << (int)font.glyph['|' - font.first].height << std::endl;
	
	
//	for (int i = 0; i < charBitmapByteSize; i++)
//	{
//		for (int j = 0; j < 8; j++)
//		{
//			DrawPixel(x, y, (charData[i] & (int)pow(j, 2)));
//		}
//		for (int j = x; j < columnWidth + x; j++)
//		{
//			
//		}
//	}

	//std::cout << std::endl;
	//std::cout << "============================================================================" << std::endl;

	delete(charData);
	return 0;
}

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

	WriteCommand(0xA8);
	WriteCommand(0x3F);

	WriteCommand(0xD3);
	WriteCommand(0x00);

	WriteCommand(0x40);
	
	//WriteCommand(0xA0); // Choose one
	WriteCommand(0xA1); // Choose one

	//WriteCommand(0xC0);
	WriteCommand(0xC8);

	WriteCommand(0xDA);
	WriteCommand(0x12);

	WriteCommand(0x81);
	WriteCommand(0x7F);

	WriteCommand(0xA4);

	WriteCommand(0xA6);

	WriteCommand(0xD5);
	WriteCommand(0x80);

	WriteCommand(0x8D);
	WriteCommand(0x14);

	WriteCommand(0xAF);	

	return 0;
}
int SSD1306::Initialize(const unsigned char i2cBus, const unsigned char i2cAddr)
{
	handle = i2cOpen(i2cBus, i2cAddr, 0);

	WriteCommand(0xA8);	// Set MUX Ratio Page 31
	WriteCommand(0x3F);

	WriteCommand(0xD3);	// Set Display Offset Page 31
	WriteCommand(0x00);

	WriteCommand(0x40);	// Set Display Start Line Page 31

	//WriteCommand(0xA0); // Set Seg re-map Page 31
	WriteCommand(0xA1); 

	WriteCommand(0xC0); // Set COM Output Scan Direction Page 31
	//WriteCommand(0xC8);

	WriteCommand(0xDA); // Set COM Pins hardware configuration Page 31
	WriteCommand(0x12);

	WriteCommand(0x81); // Set Contrast Control Page 28
	WriteCommand(0x7F);

	WriteCommand(0xA4); // Set Entire Display On Page 28

	WriteCommand(0xA6); // Set Normal Display Page 28

	WriteCommand(0xD5); // Set Osc Frequency Page 32
	WriteCommand(0x80);

	WriteCommand(0x8D); // Enable Charge Pump Regulator Page 62
	WriteCommand(0x14);

	WriteCommand(0xAF); // Display On Page 28

	return 0;
}
int SSD1306::Close(void)
{
	i2cClose(handle);
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
	//std::cout << "StrLen = " << strlen(buf) << std::endl;
	//for (unsigned int i = 0; i < strlen(buf); i++)
	//{
	//	WriteData(buf[i]);
	//}

	//int size = strlen(buf);
	//int num = size / 32;
	//int res = size - (num * 32);
	//int itteration = 1;

	//std::cout << "=============================================" << std::endl;
	//std::cout << "size = " << size << std::endl;
	//std::cout << "num  = " << num << std::endl;
	//std::cout << "res  = " << res << std::endl;
	//std::cout << "=============================================" << std::endl;

	//i2cWriteBlockData(handle, 0x40, buf, 32);

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

//int SSD1306::WriteDataString(char* buf)
//{
//	if (strlen(buf) <= 32)
//	{
//		i2cWriteI2CBlockData(handle, 0x40, buf, strlen(buf));
//		WriteData((char)0x00);
//	}
//	else
//	{
//		return -1;
//	}
//
//	return 0;
//}

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
	/*
	//WriteCommand(0x20);								// Memory Addressing Mode Page 34
	//WriteCommand(0x02);								// Page Adressing

	//WriteCommand(0x21);			// Set Column Address
	//WriteCommand(0x00);			// Set Column start Address
	//WriteCommand(0x0F);			// Set Column End Adress

	WriteCommand(0x20);		// Set Memory Addressing Mode
	WriteCommand(0x00);		// Set to Horizontal Scanning

	WriteCommand(0x22);			// Set Page Adress
	WriteCommand(0x00);			// Set to start at Page 0
	WriteCommand(0x07);			// Set end to Page 7

	WriteCommand(0x21);			// Set Column Address
	WriteCommand(0);			// Set Column start Address
	WriteCommand(127);			// Set Column End Adress

	char* buf = new char[32]; // { (char)0xFF };
	memset(buf, 0x00, 32);

	for (int y = 0; y < 8; y++)						// 8 Pages 
	{
		//int page = y | 0xb0;
		//WriteCommand(static_cast<unsigned char>(page));
		//WriteCommand(0x00);							// Set start column to 0
		//WriteCommand(0x10);			
		for (int i = 0; i < 4; i++)
		//for (int i = 0; i < 127; i++)
		{
			i2cWriteI2CBlockData(handle, 0x40, buf, 32);
			//WriteData(0x00);
		}
	}*/
	Display();

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

	/*
	//WriteCommand(0x20);								// Memory Addressing Mode Page 34
	//WriteCommand(0x02);		// Page Adressing

	//WriteCommand(0x21);			// Set Column Address
	//WriteCommand(0x00);			// Set Column start Address
	//WriteCommand(0x0F);			// Set Column End Adress

	WriteCommand(0x20);		// Set Memory Addressing Mode
	WriteCommand(0x00);		// Set to Horizontal Scanning

	//================ TODO, set this up properly =====================
	WriteCommand(0x22);			// Set Page Adress
	WriteCommand(0x00);			// Set to start at Page 0
	WriteCommand(0x07);			// Set end to Page 7

	WriteCommand(0x21);			// Set Column Address
	WriteCommand(0);			// Set Column start Address
	WriteCommand(127);			// Set Column End Adress

	char* buf = new char[32]; // { (char)0xFF };
	memset(buf, 0xff, 32);

	for (int y = 0; y < 8; y++)						// 8 Pages 
	{
		//int page = y | 0xb0;
		//WriteCommand(static_cast<unsigned char>(page));
		//WriteCommand(0x00);							// Set start column to 0
		//WriteCommand(0x10);
		for (int i = 0; i < 4; i++)
		{
			WriteDataString(buf);
		}
	}

	delete(buf);
	*/
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
		std::cout << "================================================" << std::endl;
		std::cout << "Command 0xA0" << std::endl;
		std::cout << "================================================" << std::endl;
	}
	else
	{
		flippedHorizontally = !flippedHorizontally;
		WriteCommand(0xA1);
		Display();
		std::cout << "================================================" << std::endl;
		std::cout << "Command 0xA1" << std::endl;
		std::cout << "================================================" << std::endl;
	}
	return 0;
}

int SSD1306::PrintC(const unsigned char character) // This is a test implimentation to see if I can get the correct data from the font table
{
	int charIndex = character - font.first;
	int charBitmapByteSize = font.glyph[charIndex + 1].bitmapOffset - font.glyph[charIndex].bitmapOffset;

	system("clear");
	std::cout << "==================================================================================" << std::endl;
	std::cout << "============= Glyph Data for character """ << character << """ is ================" << std::endl;
	std::cout << (int)font.glyph[character - font.first].bitmapOffset << " "
		<< (int)font.glyph[character - font.first].width << " "
		<< (int)font.glyph[character - font.first].height << " "
		<< (int)font.glyph[character - font.first].xAdvance << " "
		<< (int)font.glyph[character - font.first].xOffset << " "
		<< (int)font.glyph[character - font.first].yOffset << " " << std::endl;
	std::cout << "Glyph = " << charBitmapByteSize << " Bytes" << std::endl;
	std::cout << "==================================================================================" << std::endl;

	DrawCharacter(character, 0, 0);

	return 0;
}

void SSD1306::FontTest(void)
{
	char t = 0;

	while (t != 'x')
	{
		std::cout << "(1) Horizontal Mapping Test" << std::endl;
		std::cout << "(2) Verticle Mapping Test" << std::endl;
		std::cout << "(3) Page Mapping Test" << std::endl;
		std::cout << "(4) Clear Display" << std::endl;
		std::cout << "(5) Fill Display" << std::endl;
		std::cout << "(6) Write char to display" << std::endl;
		std::cout << "(7) Send string to OLED" << std::endl;
		std::cout << "(x) Return to Main Menu" << std::endl;

		std::cin >> t;

		switch (t)
		{
		case '1':
		{
			system("clear");
			std::cout << "=============================================================" << std::endl;
			std::cout << "Write 11000011 11000011 to Display" << std::endl;
			std::cout << "=============================================================" << std::endl;
			WriteCommand(0x20);		// Set Memory Addressing Mode
			WriteCommand(0x00);		// Set to Horizontal Scanning

			//================ TODO, set this up properly =====================
			WriteCommand(0x22);			// Set Page Adress
			WriteCommand(0x03);			// Set to start at Page 0
			WriteCommand(0x07);			// Set end to Page 7

			WriteCommand(0x21);			// Set Column Address
			WriteCommand(6);			// Set Column start Address
			WriteCommand(127);			// Set Column End Adress

			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			
		}
		break;

		case '2':
		{
			system("clear");
			std::cout << "=============================================================" << std::endl;
			std::cout << "Write 11000011 11000011 to Display" << std::endl;
			std::cout << "=============================================================" << std::endl;
			WriteCommand(0x20);		// Set Memory Addressing Mode
			WriteCommand(0x01);		// Set to Verticle Scanning

			//================ TODO, set this up properly =====================
			WriteCommand(0x22);			// Set Page Adress
			WriteCommand(0x01);			// Set to start at Page 0
			WriteCommand(0x02);			// Set end to Page 7

			WriteCommand(0x21);			// Set Column Address
			WriteCommand(0x10);			// Set Column start Address
			WriteCommand(127);			// Set Column End Adress

			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
		}
		break;

		case '3':
		{
			system("clear");
			std::cout << "=============================================================" << std::endl;
			std::cout << "Write 11000011 11000011 to Display" << std::endl;
			std::cout << "=============================================================" << std::endl;
			WriteCommand(0x20);		// Set Memory Addressing Mode
			WriteCommand(0x02);		// Set to Page Scanning

			WriteCommand(0xB6);		//Page 2

		//	WriteCommand(0x00);		//Column lower nibble
		//	WriteCommand(0x10);		//column upper nibble

		//	WriteCommand(0x21);			// Set Column Address
		//	WriteCommand(0x00);			// Set Column start Address
		//	WriteCommand(0x01);			// Set Column End Adress

			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
			WriteData(0b11000011);
		}
		break;

		case '4':
		{
			ClearScreen();
			system("clear");
		}
		break;

		case '5':
		{
			FillScreen();
			system("clear");
		}
		break;

		case '6':
		{
			DrawCharacter('H', 0, 0);
			DrawCharacter('e', 21, 0);
			DrawCharacter('l', 42, 0);
			DrawCharacter('l', 63, 0);
			DrawCharacter('o', 84, 0);

			DrawCharacter('W', 0, 35);
			DrawCharacter('o', 21, 35);
			DrawCharacter('r', 42, 35);
			DrawCharacter('l', 63, 35);
			DrawCharacter('d', 84, 35);
			Display();
			//CharArrayTest();
		}
		break;

		case '7':
		{
			PrintS("Test !", 0, 0);
			Display();
		}
		break;

		case 'x':
		{
		}
		break;

		default :
			break;
		}
	}
}

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

	buffer[page][x] &= mask;
	buffer[page][x] |= yBit;
//	std::cout << "========================================================================" << std::endl;
//	std::cout << "yBit = " << yBit << std::endl;
//	std::cout << "page = " << page << std::endl;
//	std::cout << "shift = " << shift << std::endl;
//	std::cout << "mask = " << mask << std::endl;
//	std::cout << "buffer[page][x] = " << (int)buffer[page][x] << std::endl;
//	std::cout << "========================================================================" << std::endl;

	return 0;
}

int SSD1306::CharArrayTest(void)
{
	char q = '0';
	while (q != 'x')
	{
		std::cout << "Character to write on OLED: ";
		std::cin >> q;
		ClearScreen();
		DrawCharacter(q, 0, 0);
	}
	return 0;
}

int SSD1306::PrintS(const char* buf, int x, int y)
{
	int xAdvance = x;
	std::cout << "strlen(buf) = " << strlen(buf) << std::endl;
	for (int i = 0; i < (strlen(buf)); i++)
	{
		DrawCharacter(buf[i], xAdvance, y);
		//std::cout << "xAdvance = " << xAdvance << std::endl;
		xAdvance += font.glyph[buf[i+font.first]].xAdvance;
	}
	return 0;
}
