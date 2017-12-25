
#pragma once

#include "csv_loader.h"
#include "data_point.h"
#include "price_move.h"

namespace bot {

class CPatternMatcher
{
private:
	struct SCandle
	{
		SCandle(const SCandle& candle)
			: move(candle.move)
			, magnitude(candle.magnitude)
		{}

		SCandle(const SDataPoint& dataPoint)
		{
			move.high  = dataPoint.high  - dataPoint.open;
			move.low   = dataPoint.low   - dataPoint.open;
			move.close = dataPoint.close - dataPoint.open;

			const double scale = 1.0 / dataPoint.open;

			magnitude.high  = scale * dataPoint.high  * scale * move.high;
			magnitude.low   = scale * dataPoint.low   * scale * move.low;
			magnitude.close = scale * dataPoint.close * scale * move.close;
		}


		double GetSimilarity(const double magnitudeA, const double magnitudeB) const
		{
			double similarity = 0.0;
			const double absA = abs(magnitudeA);
			const double absB = abs(magnitudeB);
			constexpr double eps = std::numeric_limits<double>::epsilon();

			if (absA > eps && absB > eps)
			{
				similarity = (absA > absB)
					? magnitudeB / magnitudeA
					: magnitudeA / magnitudeB;
			}
			else if (absA == 0.0 && absB == 0.0)
			{
				similarity = 1.0;
			}
			return similarity;
		}

		double GetSimilarity(const SCandle& other) const
		{
			double closeSimilarity = GetSimilarity(magnitude.close, other.magnitude.close);
			double highSimilarity;
			double lowSimilarity;
			double direction;

			if (closeSimilarity >= 0.0)
			{
				highSimilarity = GetSimilarity(magnitude.high, other.magnitude.high);
				lowSimilarity  = GetSimilarity(magnitude.low , other.magnitude.low);
				direction      = 1.0;
			}
			else
			{
				// Candles move opposite of each other
				// Swap high and low for pattern matching
				closeSimilarity = -closeSimilarity;
				highSimilarity  = -GetSimilarity(magnitude.high, other.magnitude.low);
				lowSimilarity   = -GetSimilarity(magnitude.low , other.magnitude.high);
				direction       = -1.0;
			}

			const double similarity = (highSimilarity + lowSimilarity + closeSimilarity) / 3.0 * direction;
			return similarity;
		}

		SPriceMove move;
		SPriceMove magnitude;
	};

	struct SCandlePattern1
	{
		SCandlePattern1(const SCandle& candle0, const SCandle& candle1)
			: candle0(candle0)
			, candle1(candle1)
		{}

		SPriceMove GetExpectedPriceMove(const SCandle& newCandle) const
		{
			const double similarity = candle0.GetSimilarity(newCandle);
			const SPriceMove expectedMove = candle1.magnitude * similarity;
			return expectedMove;
		}

		SCandle candle0;
		SCandle candle1;
	};

	using TCandlePatterns1 = vector<SCandlePattern1>;

public:
	SPriceMoves GetExpectedPriceMoves(const SDataPoint& newDataPoint) const
	{
		SPriceMoves priceMoves;
		const SCandle newCandle(newDataPoint);

		for (const SCandlePattern1& pattern : m_candlePatterns1)
		{
			const SPriceMove newMove = pattern.GetExpectedPriceMove(newCandle);

			if (newMove.Up())
			{
				priceMoves.up += newMove;
				priceMoves.upCount++;
			}
			else if (newMove.Down())
			{
				priceMoves.down += newMove;
				priceMoves.downCount++;
			}
		}
		return priceMoves;
	}

	void BuildCandlePatterns()
	{
		m_candlePatterns1.clear();

		for (const CCsvLoader::SDataSet& dataSet : m_dataSets)
		{
			const size_t usedCandleNum = dataSet.data.size() - 1;
			m_candlePatterns1.reserve(usedCandleNum);

			for (size_t candle = 0; candle < usedCandleNum; ++candle)
			{
				const SDataPoint& point0 = dataSet.data[candle];
				const SDataPoint& point1 = dataSet.data[candle+1];
				const SCandle candle0(point0);
				const SCandle candle1(point1);
				m_candlePatterns1.emplace_back(point0, point1);
			}
		}
	}

	void LoadDataSetsFromCsv(const string& dir)
	{
		m_dataSets = CCsvLoader::LoadDataSets(dir);
	}

private:
	CCsvLoader::TDataSets m_dataSets;
	TCandlePatterns1 m_candlePatterns1;
};

} // namespace bot
