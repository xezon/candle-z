
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
		return close > 0.0;
	}

	bool Down() const
	{
		return close < 0.0;
	}
	
	double high = 0;
	double low = 0;
	double close = 0;
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

struct SPriceMoves
{
	double GetUpWinChance() const
	{
		if (upCount == 0 && downCount == 0)
			return 0.0;

		if (downCount == 0)
			return 1.0;

		double up   = static_cast<double>(upCount);
		double down = static_cast<double>(downCount);
		double sum  = up + down;
		return up / sum;
	}

	double GetUpProfitChance() const
	{
		if (up.close == 0.0 && down.close == 0.0)
			return 0.0;

		if (down.close == 0)
			return 1.0;

		double sum = up.close + (-down.close);
		return up.close / sum;
	}

	double GetUpMaxProfitChance() const
	{
		if (up.high == 0.0 && down.high == 0.0)
			return 0.0;

		if (down.high == 0)
			return 1.0;

		double sum = up.high + (-down.high);
		return up.high / sum;
	}

	SPriceMove up;
	SPriceMove down;
	size_t upCount = 0;
	size_t downCount = 0;
};

} // namespace bot
