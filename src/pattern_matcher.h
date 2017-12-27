
#pragma once

#include "csv_loader.h"
#include "price_move.h"

namespace bot {

struct SPatternMatcherSettings
{
	// Exponential matching of candle.
	// A less matching candle weighs exponentially less than
	// a better matching candle with a higher exponent.
	// Default of 1.0 is a linear weight.
	double matchingExp = 1.0;

	// Min threshold value for candle matching. Range: 0 to 1.
	// Higher threshold ignores less matching candle patterns.
	double matchingThreshold = 0.0;

	// Pattern matcher ignores trend bias
	// when opposite candle matches are permitted
	bool useOppositeMatches = true;
};

template <size_t PatternN, size_t ResultN>
class CPatternMatcher
{
	using TPriceMoves = std::array<SPriceMove, ResultN>;

	static_assert(PatternN > 0, "Pattern size must be at least 1");
	static_assert(ResultN > 0, "Follow up candle size must be at least 1");

	struct SCandleSimilarity
	{
		SCandleSimilarity& operator += (const SCandleSimilarity& other)
		{
			high    += other.high;
			low     += other.low;
			close   += other.close;
			overall += other.overall;
			return *this;
		}

		SCandleSimilarity& operator *= (double value)
		{
			high    *= value;
			low     *= value;
			close   *= value;
			overall *= value;
			return *this;
		}

		double high    = 0.0;
		double low     = 0.0;
		double close   = 0.0;
		double overall = 0.0;
	};

	struct SMatchingResult
	{
		SCandleSimilarity similarity;
		TPriceMoves moves;
	};

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

		SCandleSimilarity GetSimilarity(const SPatternMatcherSettings& settings, const SCandle& other) const
		{
			SCandleSimilarity similarity;
			similarity.close = GetSimilarity(magnitude.close, other.magnitude.close);
			double direction;

			if (similarity.close >= 0.0)
			{
				similarity.high = GetSimilarity(magnitude.high, other.magnitude.high);
				similarity.low  = GetSimilarity(magnitude.low , other.magnitude.low);
				direction = 1.0;
			}
			else
			{
				// Candles move opposite of each other.
				// Swap high and low for pattern matching.
				similarity.high  = -GetSimilarity(magnitude.high, other.magnitude.low);
				similarity.low   = -GetSimilarity(magnitude.low , other.magnitude.high);
				similarity.close = -similarity.close;
				direction = -1.0;
			}

			similarity.high    = std::pow(similarity.high , settings.matchingExp) * direction;
			similarity.low     = std::pow(similarity.low  , settings.matchingExp) * direction;
			similarity.close   = std::pow(similarity.close, settings.matchingExp) * direction;
			similarity.overall = (similarity.high + similarity.low + similarity.close) / 3.0;
			return similarity;
		}

		SPriceMove magnitude;
	};

	using TCandlePatterns = std::array<SCandle, PatternN>;
	using TCandleResults = std::array<SCandle, ResultN>;

public:
	using TDataPointPatterns = std::array<SDataPoint, PatternN>;

private:
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
				const SCandleSimilarity similarity = userCandle.GetSimilarity(settings, patternCandle);

				// Adding similarities for multi candle patterns is ok.
				// Converging candle similarities within a pattern will cancel each other out.
				matchingResult.similarity += similarity;
			}
			matchingResult.similarity *= 1.0 / PatternN;

			for (size_t i = 0; i < ResultN; ++i)
			{
				SPriceMove& move = matchingResult.moves[i];
				const SCandle& candle = results[i];
				move = candle.magnitude;

				move.high  *= matchingResult.similarity.high;
				move.low   *= matchingResult.similarity.low;
				move.close *= matchingResult.similarity.close;

				if (matchingResult.similarity.close < 0.0)
				{
					// Similarity is inverse: swap high and low values.
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
		const TCandlePatterns candles = ToCandlePatterns(dataPoints);
		TUpDownPriceMoves<ResultN> upDownPriceMoves;
		size_t totalUps[ResultN] = {0};
		size_t totalDowns[ResultN] = {0};

		for (const SCandleGroup& group : m_candleGroups)
		{
			const SMatchingResult matchingResult = group.Match(settings, candles);
			const double overallSimilarity = matchingResult.similarity.overall;
			const double absOverallSimilarity = std::abs(overallSimilarity);

			if (absOverallSimilarity < settings.matchingThreshold)
				continue;

			if (!settings.useOppositeMatches && overallSimilarity < 0.0)
				continue;

			for (size_t i = 0; i < ResultN; ++i)
			{
				const SPriceMove& priceMove = matchingResult.moves[i];
				SUpDownPriceMove& upDownPriceMove = upDownPriceMoves[i];

				if (priceMove.Up())
				{
					totalUps[i]++;
					upDownPriceMove.upTotalMagnitude += priceMove;
					upDownPriceMove.upCount += absOverallSimilarity;
				}
				else if (priceMove.Down())
				{
					totalDowns[i]++;
					upDownPriceMove.downTotalMagnitude += priceMove;
					upDownPriceMove.downCount += absOverallSimilarity;
				}
			}
		}

		for (size_t i = 0; i < ResultN; ++i)
		{
			SUpDownPriceMove& upDownPriceMove = upDownPriceMoves[i];
			upDownPriceMove.upAvgMagnitude = upDownPriceMove.upTotalMagnitude / totalUps[i];
			upDownPriceMove.downAvgMagnitude = upDownPriceMove.downTotalMagnitude / totalDowns[i];
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
