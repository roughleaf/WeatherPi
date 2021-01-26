#pragma once
#include <ctime>
#include <string>
#include <chrono>
#include <thread>

class STIME
{
private:
	time_t now;
	tm* SystemTime;

	unsigned char ToBCD(int n);

public:
	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int Second;

	unsigned char BCDyear;
	unsigned char BCD10year;
	unsigned char BCDmonth;
	unsigned char BCD10month;
	unsigned char BCDday;
	unsigned char BCD10day;
	unsigned char BCDhour;
	unsigned char BCD10hour;
	unsigned char BCDminute;
	unsigned char BCD10minute;
	unsigned char BCDsecond;
	unsigned char BCD10second;

	std::string Date;
	std::string Time;
	std::string SystemDateTime;

	std::string GetSystemDateTime(void);
	std::string GetSystemDate(void);
	std::string GetSystemTime(void);

	std::chrono::system_clock::time_point GetNextTenMinute(void);
};

