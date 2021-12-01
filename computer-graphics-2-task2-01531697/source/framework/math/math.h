


#ifndef INCLUDED_MATH
#define INCLUDED_MATH

#pragma once

#include <cmath>
#include <algorithm>


namespace math
{
	template <typename T>
	struct constants;

	template <>
	struct constants<float>
	{
		static float one() { return 1.0f; }
		static float zero() { return 0.0f; }
		static float pi() { return 3.1415926535897932384626434f; }
		static float e() { return 2.7182818284590452353602875f; }
		static float sqrtHalf() { return 0.70710678118654752440084436210485f; }
		static float sqrtTwo() { return 1.4142135623730950488016887242097f; }
		static float epsilon() { return 0.00000001f; }
	};

	template <>
	struct constants<double>
	{
		static double one() { return 1.0; }
		static double zero() { return 0.0; }
		static double pi() { return 3.1415926535897932384626434; }
		static double e() { return 2.7182818284590452353602875; }
		static double sqrtHalf() { return 0.70710678118654752440084436210485; }
		static double sqrtTwo() { return 1.4142135623730950488016887242097; }
		static double epsilon() { return 0.00000000001; }
	};

	template <>
	struct constants<long double>
	{
		static long double one() { return 1.0l; }
		static long double zero() { return 0.0l; }
		static long double pi() { return 3.1415926535897932384626434l; }
		static long double e() { return 2.7182818284590452353602875l; }
		static long double sqrtHalf() { return 0.70710678118654752440084436210485l; }
		static long double sqrtTwo() { return 1.4142135623730950488016887242097l; }
		static long double epsilon() { return 0.0000000000001l; }
	};


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
	inline T clamp(T v, T min = constants<T>::zero(), T max = constants<T>::one())
	{
		return static_cast<T>(math::min(math::max(v, min), max));
	}

	inline float saturate(float v)
	{
		return clamp(v, 0.0f, 1.0f);
	}

	inline double saturate(double v)
	{
		return clamp(v, 0.0, 1.0);
	}

	inline long double saturate(long double v)
	{
		return clamp(v, 0.0l, 1.0l);
	}

	inline float rcp(float v)
	{
		return 1.0f / v;
	}

	inline double rcp(double v)
	{
		return 1.0 / v;
	}

	inline long double rcp(long double v)
	{
		return 1.0l / v;
	}

	inline float frac(float v)
	{
		return v - floor(v);
	}

	inline double frac(double v)
	{
		return v - floor(v);
	}

	inline long double frac(long double v)
	{
		return v - floor(v);
	}

	inline float half(float v)
	{
		return v * 0.5f;
	}

	inline double half(double v)
	{
		return v * 0.5;
	}

	inline long double half(long double v)
	{
		return v * 0.5l;
	}

	inline float lerp(float a, float b, float t)
	{
		return (1.0f - t) * a + t * b;
	}

	inline double lerp(double a, double b, double t)
	{
		return (1.0 - t) * a + t * b;
	}

	inline long double lerp(long double a, long double b, long double t)
	{
		return (1.0l - t) * a + t * b;
	}

	inline float smoothstep(float t)
	{
		return t * t * (3.0f - 2.0f * t);
	}

	inline double smoothstep(double t)
	{
		return t * t * (3.0 - 2.0 * t);
	}

	inline long double smoothstep(long double t)
	{
		return t * t * (3.0l - 2.0l * t);
	}

	inline float smootherstep(float t)
	{
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	inline double smootherstep(double t)
	{
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}

	inline long double smootherstep(long double t)
	{
		return t * t * t * (t * (t * 6.0l - 15.0l) + 10.0l);
	}
}

#endif // INCLUDED_MATH
