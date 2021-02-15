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
		typedef union
		{
			short shortNumber;
			uint8_t bytes[2];
		} SHORTUNION_t;
		
		SHORTUNION_t shortToByte;

		std::string JsonReturn = "{";
		
		JsonReturn = JsonReturn + "\"MacAddress\":" + '"' + MacAddress + '"' + ',';

		JsonReturn = JsonReturn + "\"NodeData\":[";

		for (int i = 0; i < 9; i++)
		{
			JsonReturn = JsonReturn + nodeData[i].BuildJsonString() + ',';
		}

		JsonReturn = JsonReturn + nodeData[9].BuildJsonString() + ']' + ',';

		JsonReturn = JsonReturn + "\"LightningData\":" + lightningData.BuildJsonString();

		JsonReturn = JsonReturn + '}';

		// Following code clock sets the string length as a header.
		shortToByte.shortNumber = JsonReturn.length();
		char LengthHeaderTempChar[2] = { shortToByte.bytes[0], shortToByte.bytes[1] };
		std::string lengthHeader = LengthHeaderTempChar;

		JsonReturn = lengthHeader + JsonReturn;

		return JsonReturn;
	}
}