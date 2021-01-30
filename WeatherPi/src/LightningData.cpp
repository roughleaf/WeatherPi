#include "LightningData.hpp"

void LightningData::AddLightningStrike(int distance)
{
	if (LightningCount < 200)
	{
		StrikeDistance[LightningCount].LightningDistance = distance;
		StrikeDistance[LightningCount].LightningDate = systemTime.GetSystemDate();
		StrikeDistance[LightningCount].LightningTime = systemTime.GetSystemTime();
	}
	LightningCount++;	// I want LightningCount to keep counting even after LightningDistance array is full.
}

std::string LightningData::BuildJsonString(void)
{
	std::string JsonReturn = "[";

	for (int i = 0; i < LightningCount; i++)
	{
		JsonReturn = JsonReturn + '{';
		JsonReturn = JsonReturn + "\"LightningDistance\"" + ':' + std::to_string(StrikeDistance[i].LightningDistance) + ',';
		JsonReturn = JsonReturn + "\"DateStamp\"" + ':' + '"' + StrikeDistance[i].LightningDate + '"' + ',';
		JsonReturn = JsonReturn + "\"TimeStamp\"" + ':' + '"' + StrikeDistance[i].LightningTime + '"';
		JsonReturn = JsonReturn + '}';

		if (i < LightningCount - 1)
		{
			JsonReturn = JsonReturn + ',';
		}

	}

	JsonReturn = JsonReturn + ']';

	return JsonReturn;
}

void LightningData::clear(void)
{
	LightningCount = 0;
	StrikeDistance->LightningDate = { "" };
	StrikeDistance->LightningTime = { "" };
	StrikeDistance->LightningDistance = { 0 };
}
