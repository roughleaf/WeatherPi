#pragma once
#include "STIME.hpp"
#include <string>
#include "ClimateData.hpp"
#include "LightningData.hpp"
namespace icodec
{
	void BuildTimeDateByteString(char* datetime, int channel);
	std::string BuildJsonArray(ClimateData* nodeData, LightningData lightningData);
	// std::string BuildJsonArray(ClimateData* nodeData);
}