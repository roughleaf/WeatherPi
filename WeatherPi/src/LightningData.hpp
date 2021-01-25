#pragma once
#include "STIME.hpp"
#include <string>
class LightningData
{
private:

	struct StrikeData
	{
		int LightningDistance = 0;
		std::string LightningDate = "";
		std::string LightningTime = "";
	};

	// LightningCount and LightningDistance must be private and only be updatable through class method.
	// Reason being the LightningCount will be used to keep track of the LightningDisatnce element in the array

	// Will be able to store 200 strikes in a 10 minute period.
	// If I ever reach 200 strikes in a tem munute period in lighning season then I'll increase the array size.
	int LightningCount = 0;
	StrikeData StrikeDistance[200];

	STIME systemTime;

public:
	void AddLightningStrike(int distance);
	std::string BuildJsonString(void);
	void clear(void);
};
		

