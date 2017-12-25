
#define ZORRO_DLLMAIN
#define ZORRO_EXPECT_ALL_FUNCTIONS
#define ZORRO_USE_EVENT_CLASS
#include <zorro_impl.h>
#include <common/types.h>
#include "csv_loader.h"
#include "pattern_matcher.h"

bot::CPatternMatcher<1,1> g_patternMatcher11;
bot::CPatternMatcher<2,2> g_patternMatcher22;

void CZorroEvents::main()
{
	const bot::TDataSets dataSets = bot::CCsvLoader::LoadDataSets("../data");

	g_patternMatcher11.BuildCandlePatterns(dataSets);
	g_patternMatcher22.BuildCandlePatterns(dataSets);

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

	auto priceMoves = g_patternMatcher22.GetExpectedPriceMoves(dataPoint0, dataPoint1);

	double upWinChance = GetUpWinChance(priceMoves);
	double upProfitChance = GetUpProfitChance(priceMoves);
	double upMaxProfitChance = GetUpMaxProfitChance(priceMoves);
}

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
