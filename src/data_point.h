
#pragma once

#include "types.h"

namespace bot {

struct SDataPoint
{
	static const size_t columnCount = 6;
	uint64_t time   = 0;
	double   open   = 0.0;
	double   high   = 0.0;
	double   low    = 0.0;
	double   close  = 0.0;
	double   volume = 0.0;

	constexpr double& operator[] (size_t i)
	{
		switch (i)
		{
		case 1: return open;
		case 2: return high;
		case 3: return low;
		case 4: return close;
		case 5: return volume;
		default: throw std::out_of_range("column out of range");
		}
	}
};

using TDataPoints = vector<SDataPoint>;

} // namespace bot
