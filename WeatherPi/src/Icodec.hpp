#pragma once
#include "STIME.hpp"
#include <string>
#include "ClimateData.hpp"
namespace icodec
{
	void BuildTimeDateByteString(char* datetime, int channel);
	std::string BuildJsonArray(ClimateData* nodeData);
}