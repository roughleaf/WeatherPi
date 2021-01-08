#include "STIME.hpp"

unsigned char STIME::ToBCD(int n)
{
	return ((n / 10) << 4) | (n % 10);
}

void STIME::GetSystemTime(void)
{
	now = time(0);
	SystemTime = localtime(&now);

	Year = 1900 + SystemTime->tm_year;
	Month = 1 + SystemTime->tm_mon;
	Day = SystemTime->tm_mday;
	Hour = SystemTime->tm_hour;
	Minute = SystemTime->tm_min;
	Second = SystemTime->tm_sec;

	BCDyear = ToBCD(Year - 2000) & 0x0F;
	BCD10year = ToBCD(Year - 2000) >> 4;

	BCDmonth = ToBCD(Month) & 0x0F;
	BCD10month = ToBCD(Month) >> 4;

	BCDday = ToBCD(Day) & 0x0F;
	BCD10day = ToBCD(Day) >> 4;

	BCDhour = ToBCD(Hour) & 0x0F;
	BCD10hour = ToBCD(Hour) >> 4;

	BCDminute = ToBCD(Minute) & 0x0F;
	BCD10minute = ToBCD(Minute) >> 4;

	BCDsecond = ToBCD(Second) & 0x0F;
	BCD10second = ToBCD(Second) >> 4;

	Date = std::to_string(Year) + "/" + std::to_string(Month) + "/" + std::to_string(Day);
	Time = std::to_string(Hour) + ":" + std::to_string(Minute) + ":" + std::to_string(Second);
	if (std::to_string(Second).length() == 1)
	{
		Time += "0";
	}

	SystemDateTime = Date + " " + Time;
}
