#include "Icodec.hpp"
namespace icodec
{
	void BuildTimeDateByteString(char* datetime, int channel)
	{
		STIME stime;
		stime.GetSystemTime();

		datetime[0] = (char)channel;
		datetime[1] = stime.BCD10year;
		datetime[2] = stime.BCDyear;
		datetime[3] = stime.BCD10month;
		datetime[4] = stime.BCDmonth;
		datetime[5] = stime.BCD10day;
		datetime[6] = stime.BCDday;

		datetime[7] = stime.BCD10hour;
		datetime[8] = stime.BCDhour;
		datetime[9] = stime.BCD10minute;
		datetime[10] = stime.BCDminute;
		datetime[11] = stime.BCD10second;
		datetime[12] = stime.BCDsecond;
	}
}