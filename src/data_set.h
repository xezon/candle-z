
#pragma once

#include "types.h"
#include "data_point.h"

namespace bot {

struct SDataSet
{
	string name;
	fs::path path;
	TDataPoints data;

	bool empty() const
	{
		return data.empty();
	}

	void clear()
	{
		name.clear();
		path.clear();
		data.clear();
	}
};

using TDataSets = vector<SDataSet>;

} // namespace bot
