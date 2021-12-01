


#ifndef INCLUDED_FRAMEWORK_MATH
#define INCLUDED_FRAMEWORK_MATH

#pragma once

#include <cassert>
#include <cmath>
#include <algorithm>


namespace math
{
	template <typename T>
	struct constants;

	template <>
	struct constants<int>
	{
		static constexpr int zero = 0;
		static constexpr int one = 1;
	};

	template <>
	struct constants<float>
	{
		static constexpr float zero = 0.0f;
		static constexpr float one = 1.0f;
		static constexpr float pi = 3.1415926535897932384626434f;
		static constexpr float e = 2.7182818284590452353602875f;
	};

	template <>
	struct constants<double>
	{
		static constexpr double zero = 0.0;
		static constexpr double one = 1.0;
		static constexpr double pi = 3.1415926535897932384626434;
		static constexpr double e = 2.7182818284590452353602875;
	};

	template <>
	struct constants<long double>
	{
		static constexpr long double zero = 0.0l;
		static constexpr long double one = 1.0l;
		static constexpr long double pi = 3.1415926535897932384626434l;
		static constexpr long double e = 2.7182818284590452353602875l;
	};

	template <typename T>
	constexpr T zero = constants<T>::zero;
	template <typename T>
	constexpr T one = constants<T>::one;
	template <typename T>
	constexpr T pi = constants<T>::pi;
	template <typename T>
	constexpr T e = constants<T>::e;


	using std::min;
	using std::max;

	using std::abs;

	using std::exp;
	using std::frexp;
	using std::ldexp;
	using std::log;
	using std::log10;
	using std::modf;

	using std::cos;
	using std::sin;
	using std::tan;
	using std::acos;
	using std::asin;
	using std::atan;
	using std::atan2;
	using std::cosh;
	using std::sinh;
	using std::tanh;

	using std::pow;
	using std::sqrt;

	using std::floor;
	using std::ceil;

	using std::fmod;


	template <typename T>
	constexpr T clamp(T v, T min = zero<T>, T max = one<T>)
	{
		assert(min <= max);
		return static_cast<T>(math::min(math::max(v, min), max));
	}

	constexpr float rcp(float v)
	{
		return 1.0f / v;
	}

	constexpr double rcp(double v)
	{
		return 1.0 / v;
	}

	constexpr long double rcp(long double v)
	{
		return 1.0l / v;
	}

	constexpr float half(float v)
	{
		return v * 0.5f;
	}

	constexpr double half(double v)
	{
		return v * 0.5;
	}

	constexpr long double half(long double v)
	{
		return v * 0.5l;
	}

	constexpr float lerp(float a, float b, float t)
	{
		return (1.0f - t) * a + t * b;
	}

	constexpr double lerp(double a, double b, double t)
	{
		return (1.0 - t) * a + t * b;
	}

	constexpr long double lerp(long double a, long double b, long double t)
	{
		return (1.0l - t) * a + t * b;
	}
}

#endif  // INCLUDED_FRAMEWORK_MATH
