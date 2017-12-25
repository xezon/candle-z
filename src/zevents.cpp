
#define ZORRO_DLLMAIN
#define ZORRO_EXPECT_ALL_FUNCTIONS
#define ZORRO_USE_EVENT_CLASS
#include <zorro_impl.h>
#include <common/types.h>
#include "pattern_matcher.h"

bot::CPatternMatcher g_patternMatcher;

void CZorroEvents::main()
{
	g_patternMatcher.LoadDataSetsFromCsv("../data");
	g_patternMatcher.BuildCandlePatterns();

	bot::SDataPoint dataPoint;
	dataPoint.open = 16611.0;
	dataPoint.high = 17153.0;
	dataPoint.low = 13966.0;
	dataPoint.close = 15198.0;

	bot::SPriceMoves priceMoves = g_patternMatcher.GetExpectedPriceMoves(dataPoint);
	double upWinChance = priceMoves.GetUpWinChance();
	double upProfitChance = priceMoves.GetUpProfitChance();
	double upMaxProfitChance = priceMoves.GetUpMaxProfitChance();
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
