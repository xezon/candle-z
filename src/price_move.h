
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
	a *= b;
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
		return GetChance(upCount, downCount);
	}

	double GetDownWinChance() const
	{
		return GetChance(downCount, upCount);
	}

	double GetUpProfitChance() const
	{
		return GetChance(up.close, -down.close);
	}

	double GetDownProfitChance() const
	{
		return GetChance(-down.close, up.close);
	}

	double GetUpMaxProfitChance() const
	{
		return GetChance(up.high, -down.low);
	}

	double GetDownMaxProfitChance() const
	{
		return GetChance(-down.low, up.high);
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

	SPriceMove up;
	SPriceMove down;
	double upCount = 0.0;
	double downCount = 0.0;
};

template <size_t N>
using TUpDownPriceMoves = std::array<SUpDownPriceMove, N>;
using TUpDownPriceChanceFunction = double (*)(const SUpDownPriceMove&);

template <size_t N>
inline double GetChance(TUpDownPriceMoves<N> moves, TUpDownPriceChanceFunction function)
{
	double chance = 0.0;
	for (const SUpDownPriceMove& move : moves)
	{
		chance += function(move);
	}
	return chance / N;
}

template <size_t N>
inline double GetUpWinChance(TUpDownPriceMoves<N> moves)
{
	return GetChance(moves, SUpDownPriceMove::GetUpWinChance);
}

template <size_t N>
inline double GetDownWinChance(TUpDownPriceMoves<N> moves)
{
	return GetChance(moves, SUpDownPriceMove::GetDownWinChance);
}

template <size_t N>
inline double GetUpProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetChance(moves, SUpDownPriceMove::GetUpProfitChance);
}

template <size_t N>
inline double GetDownProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetChance(moves, SUpDownPriceMove::GetDownProfitChance);
}

template <size_t N>
inline double GetUpMaxProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetChance(moves, SUpDownPriceMove::GetUpMaxProfitChance);
}

template <size_t N>
inline double GetDownMaxProfitChance(TUpDownPriceMoves<N> moves)
{
	return GetChance(moves, SUpDownPriceMove::GetDownMaxProfitChance);
}

} // namespace bot
