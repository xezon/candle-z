
#pragma once

namespace bot {

struct SPriceMove
{
	SPriceMove& operator+= (const SPriceMove& a)
	{
		high += a.high;
		low += a.low;
		close += a.close;
		return *this;
	}

	SPriceMove& operator-= (const SPriceMove& a)
	{
		high -= a.high;
		low -= a.low;
		close -= a.close;
		return *this;
	}

	SPriceMove& operator*= (double a)
	{
		high *= a;
		low *= a;
		close *= a;
		return *this;
	}

	SPriceMove& operator/= (double a)
	{
		high /= a;
		low /= a;
		close /= a;
		return *this;
	}

	bool Up() const
	{
		return close > DoubleEps;
	}

	bool Down() const
	{
		return close < -DoubleEps;
	}
	
	double high = 0.0;
	double low = 0.0;
	double close = 0.0;
};

inline SPriceMove operator+ (SPriceMove a, const SPriceMove& b)
{
	a += b;
	return a;
}

inline SPriceMove operator- (SPriceMove a, const SPriceMove& b)
{
	a -= b;
	return a;
}

inline SPriceMove operator* (SPriceMove a, const double b)
{
	a *= b;
	return a;
}

inline SPriceMove operator/ (SPriceMove a, const double b)
{
	a /= b;
	return a;
}

struct SUpDownPriceMove
{
	static double GetChance(double a, double b)
	{
		if (a <= DoubleEps && b <= DoubleEps)
		{
			// Both values are zero
			// it is impossible to make a chance prediction
			return 0.0;
		}
		
		if (b <= DoubleEps)
		{
			// b is zero, 100% chance to hit a
			return 1.0;
		}
		
		double sum = a + b;
		return a / sum;
	}

	double GetUpWinChance() const
	{
		return GetChance(upSimilarity, downSimilarity);
	}

	double GetDownWinChance() const
	{
		return GetChance(downSimilarity, upSimilarity);
	}

	double GetUpProfitChance() const
	{
		return GetChance(upTotalMagnitude.close, -downTotalMagnitude.close);
	}

	double GetDownProfitChance() const
	{
		return GetChance(-downTotalMagnitude.close, upTotalMagnitude.close);
	}

	double GetUpMaxProfitChance() const
	{
		return GetChance(upTotalMagnitude.high, -downTotalMagnitude.low);
	}

	double GetDownMaxProfitChance() const
	{
		return GetChance(-downTotalMagnitude.low, upTotalMagnitude.high);
	}

	static double GetUpWinChance(const SUpDownPriceMove& move)
	{
		return move.GetUpWinChance();
	}

	static double GetDownWinChance(const SUpDownPriceMove& move)
	{
		return move.GetDownWinChance();
	}

	static double GetUpProfitChance(const SUpDownPriceMove& move)
	{
		return move.GetUpProfitChance();
	}

	static double GetDownProfitChance(const SUpDownPriceMove& move)
	{
		return move.GetDownProfitChance();
	}

	static double GetUpMaxProfitChance(const SUpDownPriceMove& move)
	{
		return move.GetUpMaxProfitChance();
	}

	static double GetDownMaxProfitChance(const SUpDownPriceMove& move)
	{
		return move.GetDownMaxProfitChance();
	}

	SPriceMove GetUpAvgMagnitude() const
	{
		if (upCount > 0)
		{
			return upTotalMagnitude / upCount;
		}
		return SPriceMove();
	}

	SPriceMove GetDownAvgMagnitude() const
	{
		if (downCount > 0)
		{
			return downTotalMagnitude / downCount;
		}
		return SPriceMove();
	}

	static SPriceMove GetUpAvgMagnitude(const SUpDownPriceMove& move)
	{
		return move.GetUpAvgMagnitude();
	}

	static SPriceMove GetDownAvgMagnitude(const SUpDownPriceMove& move)
	{
		return move.GetDownAvgMagnitude();
	}

	SPriceMove upTotalMagnitude;
	SPriceMove downTotalMagnitude;
	double upSimilarity = 0.0;
	double downSimilarity = 0.0;
	size_t upCount = 0;
	size_t downCount = 0;
};

template <size_t N>
using TUpDownPriceMoves = std::array<SUpDownPriceMove, N>;

template <class ReturnType>
using TUpDownPriceStaticFunction = ReturnType (*)(const SUpDownPriceMove&);

template <size_t N, class ValueType>
inline ValueType GetAverageValue(TUpDownPriceMoves<N> moves, TUpDownPriceStaticFunction<ValueType> function)
{
	static_assert(N > 0, "Array size must be at least 1");
	ValueType value = {};
	for (const SUpDownPriceMove& move : moves)
	{
		value += function(move);
	}
	return value / N;
}

template <size_t N>
inline double GetUpWinChance(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetUpWinChance);
}

template <size_t N>
inline double GetDownWinChance(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetDownWinChance);
}

template <size_t N>
inline double GetUpProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetUpProfitChance);
}

template <size_t N>
inline double GetDownProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetDownProfitChance);
}

template <size_t N>
inline double GetUpMaxProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetUpMaxProfitChance);
}

template <size_t N>
inline double GetDownMaxProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetDownMaxProfitChance);
}

template <size_t N>
inline SPriceMove GetUpAvgMagnitude(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetUpAvgMagnitude);
}

template <size_t N>
inline SPriceMove GetDownAvgMagnitude(TUpDownPriceMoves<N> moves)
{
	return GetAverageValue(moves, SUpDownPriceMove::GetDownAvgMagnitude);
}

} // namespace bot
