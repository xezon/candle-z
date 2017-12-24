
#pragma once

#include "csv_loader.h"
#include "candle.h"
#include "chance.h"

namespace bot {

class CPatternMatcher
{
public:
	SChance GetChance(const SCandle& candle)
	{
		(void)candle;
	}

	void FeedPatternsFromCsv(const string& dir)
	{
		m_dataSets = CCsvLoader::LoadDataSets(dir);
	}

private:
	CCsvLoader::TDataSets m_dataSets;
};

} // namespace bot
