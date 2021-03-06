#include "STIME.hpp"
#include <iostream>	// For testing and debugging purposes

unsigned char STIME::ToBCD(int n)
{
	return ((n / 10) << 4) | (n % 10);
}

std::string STIME::GetSystemDateTime(void)
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

	// Bug: zero values is discarded. The use of the BCD variables siims to aleviate this. I need to adjust below code acordingly
	Date = "20" + std::to_string(BCD10year) + std::to_string(BCDyear) + "/" + std::to_string(BCD10month) + std::to_string(BCDmonth) + "/" + std::to_string(BCD10day) + std::to_string(BCDday);
	Time = std::to_string(BCD10hour) + std::to_string(BCDhour) + ":" + std::to_string(BCD10minute) + std::to_string(BCDminute) + ":" + std::to_string(BCD10second) + std::to_string(BCDsecond);
	//if (std::to_string(Second).length() == 1)
	//{
	//	Time += "0";
	//}

	SystemDateTime = Date + " " + Time;

	return SystemDateTime;
}

std::string STIME::GetSystemDate(void)
{
	GetSystemDateTime();
	return Date;
}

std::string STIME::GetSystemTime(void)
{
	GetSystemDateTime();
	return Time;
}

std::chrono::system_clock::time_point STIME::GetNextTenMinute(void)
{
	now = time(0);

	tm *NextTenMinute = localtime(&now);

	int tenMinute = ToBCD(NextTenMinute->tm_min) >> 4;
	tenMinute += 1;
	tenMinute *= 10;

	NextTenMinute->tm_min = tenMinute;

	NextTenMinute->tm_sec = 10;

	time_t tenMinuteTime = mktime(NextTenMinute);

	std::chrono::system_clock::time_point tenMinuteReturn =
		std::chrono::system_clock::from_time_t(tenMinuteTime);

	return tenMinuteReturn;
}

