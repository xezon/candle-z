
#pragma once

#include "csv_loader.h"
#include "price_move.h"

namespace bot {

struct SPatternMatcherSettings
{
	double similarityExp = 1.0;
};

template <size_t PatternN, size_t ResultN>
class CPatternMatcher
{
	static_assert(PatternN > 0, "Pattern size must be at least 1");
	static_assert(ResultN > 0, "Follow up candle size must be at least 1");

	struct SCandle
	{
		SCandle()
		{}

		SCandle(const SCandle& candle)
			: magnitude(candle.magnitude)
		{}

		SCandle(const SDataPoint& dataPoint)
		{
			SPriceMove move;
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
			const double absA = std::abs(magnitudeA);
			const double absB = std::abs(magnitudeB);

			if (absA > DoubleEps && absB > DoubleEps)
			{
				similarity = (absA > absB)
					? magnitudeB / magnitudeA
					: magnitudeA / magnitudeB;
			}
			else if (absA <= DoubleEps && absB <= DoubleEps)
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

		SPriceMove magnitude;
	};

	using TPriceMoves = std::array<SPriceMove, ResultN>;
	using TCandlePatterns = std::array<SCandle, PatternN>;
	using TCandleResults = std::array<SCandle, ResultN>;
public:
	using TDataPointPatterns = std::array<SDataPoint, PatternN>;
private:

	struct SMatchingResult
	{
		double similarity = 0.0;
		TPriceMoves moves;
	};

	struct SCandleGroup
	{
		SCandleGroup()
		{}

		SMatchingResult Match(
			const SPatternMatcherSettings& settings,
			const TCandlePatterns& candles) const
		{
			SMatchingResult matchingResult;

			for (size_t i = 0; i < PatternN; ++i)
			{
				const SCandle& userCandle = candles[i];
				const SCandle& patternCandle = patterns[i];
				matchingResult.similarity += userCandle.GetSimilarity(patternCandle);
			}
			matchingResult.similarity /= PatternN;
			matchingResult.similarity = std::pow(matchingResult.similarity, settings.similarityExp);

			for (size_t i = 0; i < ResultN; ++i)
			{
				const SCandle& candle = results[i];
				SPriceMove& move = matchingResult.moves[i];
				move = candle.magnitude * matchingResult.similarity;

				if (matchingResult.similarity < 0.0)
				{
					// Similarity is inverse: swap high and low values
					std::swap(move.high, move.low);
				}
			}
			return matchingResult;
		}
		
		TCandlePatterns patterns;
		TCandleResults results;
	};

	using TCandleGroups = vector<SCandleGroup>;

	TCandlePatterns ToCandlePatterns(const TDataPointPatterns& dataPoints) const
	{
		TCandlePatterns candles;
		for (size_t i = 0; i < PatternN; ++i)
		{
			candles[i] = dataPoints[i];
		};
		return candles;
	}

public:
	TUpDownPriceMoves<ResultN> GetExpectedPriceMoves(
		const SPatternMatcherSettings& settings,
		const TDataPointPatterns& dataPoints) const
	{
		TUpDownPriceMoves<ResultN> upDownPriceMoves;
		const TCandlePatterns candles = ToCandlePatterns(dataPoints);

		for (const SCandleGroup& group : m_candleGroups)
		{
			const SMatchingResult matchingResult = group.Match(settings, candles);

			for (size_t i = 0; i < ResultN; ++i)
			{
				const SPriceMove& priceMove = matchingResult.moves[i];
				SUpDownPriceMove& upDownPriceMove = upDownPriceMoves[i];

				if (priceMove.Up())
				{
					upDownPriceMove.up += priceMove;
					upDownPriceMove.upCount += std::abs(matchingResult.similarity);
				}
				else if (priceMove.Down())
				{
					upDownPriceMove.down += priceMove;
					upDownPriceMove.downCount += std::abs(matchingResult.similarity);
				}
			}
		}
		return upDownPriceMoves;
	}

	template <class... Args>
	TUpDownPriceMoves<ResultN> GetExpectedPriceMoves(
		const SPatternMatcherSettings& settings,
		const Args&... args) const
	{
		static_assert(sizeof...(args) == PatternN, "Argument size must match pattern size");
		const TDataPointPatterns dataPoints = { args... };
		return GetExpectedPriceMoves(settings, dataPoints);
	}

	void BuildCandlePatterns(const TDataSets& dataSets)
	{
		m_candleGroups.clear();

		for (const SDataSet& dataSet : dataSets)
		{
			const size_t candleBegin = PatternN - 1;
			const size_t candleEnd = dataSet.data.size() - ResultN;
			const size_t candleCount = candleEnd - candleBegin;
			m_candleGroups.reserve(candleCount - 1);

			for (size_t candle = candleBegin; candle < candleCount; ++candle)
			{
				SCandleGroup group;

				for (int i = 0; i < PatternN; ++i)
				{
					const SDataPoint& dataPoint = dataSet.data[candle + i + 1 - PatternN];
					SCandle newCandle(dataPoint);
					group.patterns[i] = newCandle;
				}

				for (int i = 0; i < ResultN; ++i)
				{
					const SDataPoint& dataPoint = dataSet.data[candle + i + 1];
					SCandle newCandle(dataPoint);
					group.results[i] = newCandle;
				}

				m_candleGroups.emplace_back(group);
			}
		}
	}

private:
	TCandleGroups m_candleGroups;
};

} // namespace bot
