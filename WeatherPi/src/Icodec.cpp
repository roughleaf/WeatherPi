#include "Icodec.hpp"
namespace icodec
{
	void BuildTimeDateByteString(char* datetime, int channel)
	{
		STIME stime;
		stime.GetSystemDateTime();

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

	std::string BuildJsonArray(ClimateData* nodeData, LightningData lightningData)
	// std::string BuildJsonArray(ClimateData* nodeData)
	{
		std::string JsonReturn = "{\"NodeData\":[";

		for (int i = 0; i < 9; i++)
		{
			JsonReturn = JsonReturn + nodeData[i].BuildJsonString() + ',';
		}

		JsonReturn = JsonReturn + nodeData[9].BuildJsonString() + ']' + ',';

		JsonReturn = JsonReturn + "\"LightningData\":" + lightningData.BuildJsonString();

		JsonReturn = JsonReturn + '}';

		return JsonReturn;
	}
}