
#define ZORRO_DLLMAIN
#define ZORRO_EXPECT_ALL_FUNCTIONS
#define ZORRO_USE_EVENT_CLASS
#include <zorro_impl.h>
#include <common/types.h>
#include "csv_loader.h"
#include "pattern_matcher.h"

bot::CPatternMatcher<1,1> g_patternMatcher11;
bot::CPatternMatcher<2,2> g_patternMatcher22;
bot::CPatternMatcher<2,1> g_patternMatcher21;

#pragma warning(push)
#pragma warning(disable: 4189)

void CZorroEvents::main()
{
	const bot::TDataSets dataSets = bot::CCsvLoader::LoadDataSets("../data");
	bot::SPatternMatcherSettings settings;
	settings.matchingExp = 1.0;
	settings.matchingThreshold = 0.0;
	settings.useOppositeMatches = true;

	g_patternMatcher11.BuildCandlePatterns(dataSets);
	g_patternMatcher22.BuildCandlePatterns(dataSets);
	g_patternMatcher21.BuildCandlePatterns(dataSets);

	bot::SDataPoint dataPoint0;
	dataPoint0.open = 13812;
	dataPoint0.high = 16639;
	dataPoint0.low = 13333;
	dataPoint0.close = 16611;

	bot::SDataPoint dataPoint1;
	dataPoint1.open = 16611;
	dataPoint1.high = 17153;
	dataPoint1.low = 13966;
	dataPoint1.close = 15198;

	auto priceMoves11 = g_patternMatcher11.GetExpectedPriceMoves(settings, dataPoint1);
	auto priceMoves22 = g_patternMatcher22.GetExpectedPriceMoves(settings, dataPoint0, dataPoint1);
	auto priceMoves21 = g_patternMatcher21.GetExpectedPriceMoves(settings, dataPoint0, dataPoint1);

	double upWinChance11 = GetUpWinChance(priceMoves11);
	double upProfitChance11 = GetUpProfitChance(priceMoves11);
	double upMaxProfitChance11 = GetUpMaxProfitChance(priceMoves11);
	bot::SPriceMove upAvgMagnitude = GetUpAvgMagnitude(priceMoves11);
	bot::SPriceMove downAvgMagnitude = GetDownAvgMagnitude(priceMoves11);

	double downWinChance11 = GetDownWinChance(priceMoves11);
	double downProfitChance11 = GetDownProfitChance(priceMoves11);
	double downMaxProfitChance11 = GetDownMaxProfitChance(priceMoves11);

	double upWinChance22 = GetUpWinChance(priceMoves22);
	double upProfitChance22 = GetUpProfitChance(priceMoves22);
	double upMaxProfitChance22 = GetUpMaxProfitChance(priceMoves22);

	double upWinChance21 = GetUpWinChance(priceMoves21);
	double upProfitChance21 = GetUpProfitChance(priceMoves21);
	double upMaxProfitChance21 = GetUpMaxProfitChance(priceMoves21);
}

#pragma warning(pop)

void CZorroEvents::run()
{

}

void CZorroEvents::tick()
{

}

void CZorroEvents::tock()
{

}

void CZorroEvents::click(int row, int col)
{
	(void)row;
	(void)col;
}

void CZorroEvents::evaluate(const PERFORMANCE* pPerformance)
{
	(void)pPerformance;
}

var CZorroEvents::objective()
{
	return objectivePRR();
}

EOrderResult CZorroEvents::order(EOrderAction type)
{
	(void)type;

	return EOrderResult::OK;
}

var CZorroEvents::neural(ENeuralMode mode, int model, int numSignals, const void* pData)
{
	(void)mode;
	(void)model;
	(void)numSignals;
	(void)pData;

	return 0.0;
}

EBarAction CZorroEvents::bar(cvars open, cvars high, cvars low, cvars close, cvars price, DATE start, DATE time)
{
	(void)open;
	(void)high;
	(void)low;
	(void)close;
	(void)price;
	(void)start;
	(void)time;

	return EBarAction::CLOSE_NORMALLY;
}
