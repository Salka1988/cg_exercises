


#ifndef INCLUDED_MATH_VECTOR
#define INCLUDED_MATH_VECTOR

#pragma once

#include "math.h"

#include <ostream>


namespace math
{
	template <typename T, unsigned int D>
	class vector;

	template <typename T>
	class vector<T, 2U>
	{
	public:
		static const unsigned int dim = 2U;
		typedef T field_type;

		T x;
		T y;

		vector() = default;

		explicit vector(T a)
		    : x(a), y(a)
		{
		}

		vector(T x, T y)
		    : x(x), y(y)
		{
		}

		template <typename U>
		vector(const vector<U, 2U>& v)
		    : x(v.x), y(v.y)
		{
		}

		vector& operator=(const vector& v) = default;

		vector yx() const
		{
			return vector(y, x);
		}

		vector operator-() const
		{
			return vector(-x, -y);
		}

		vector& operator+=(const vector& v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		vector& operator-=(const vector& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		vector& operator-=(T a)
		{
			x -= a;
			y -= a;
			return *this;
		}

		vector& operator*=(T a)
		{
			x *= a;
			y *= a;
			return *this;
		}

		vector& operator*=(const vector& v)
		{
			x *= v.x;
			y *= v.y;
			return *this;
		}

		vector& operator/=(T a)
		{
			x /= a;
			y /= a;
			return *this;
		}

		friend const vector operator+(const vector& a, const vector& b)
		{
			return vector(a.x + b.x, a.y + b.y);
		}

		friend const vector operator+(const vector& a, T b)
		{
			return vector(a.x + b, a.y + b);
		}

		friend const vector operator+(T a, const vector& b)
		{
			return vector(a + b.x, a + b.y);
		}

		friend const vector operator-(const vector& a, const vector& b)
		{
			return vector(a.x - b.x, a.y - b.y);
		}

		friend const vector operator-(const vector& a, T b)
		{
			return vector(a.x - b, a.y - b);
		}
		friend const vector operator-(T b, const vector& a)
		{
			return vector(b - a.x, b - a.y);
		}

		friend const vector operator*(T a, const vector& v)
		{
			return vector(a * v.x, a * v.y);
		}

		friend const vector operator*(const vector& v, T a)
		{
			return a * v;
		}

		friend const vector operator*(const vector& a, const vector& b)
		{
			return vector(a.x * b.x, a.y * b.y);
		}

		friend const vector operator/(const vector& v, T a)
		{
			return vector(v.x / a, v.y / a);
		}

		friend T dot(const vector& a, const vector& b)
		{
			return a.x * b.x + a.y * b.y;
		}

		friend vector abs(const vector& v)
		{
			return vector(abs(v.x), abs(v.y));
		}

		friend vector floor(const vector& v)
		{
			return vector(floor(v.x), floor(v.y));
		}

		friend vector ceil(const vector& v)
		{
			return vector(ceil(v.x), ceil(v.y));
		}

		friend vector max(const vector& v, T c)
		{
			return vector(max(v.x, c), max(v.y, c));
		}
		friend vector max(T c, const vector& v)
		{
			return vector(max(v.x, c), max(v.y, c));
		}
		friend vector min(const vector& v, T c)
		{
			return vector(min(v.x, c), min(v.y, c));
		}
		friend vector min(T c, const vector& v)
		{
			return vector(min(v.x, c), min(v.y, c));
		}

		friend vector min(const vector& a, const vector& b)
		{
			return vector(min(a.x, b.x), min(a.y, b.y));
		}
		friend vector max(const vector& a, const vector& b)
		{
			return vector(max(a.x, b.x), max(a.y, b.y));
		}
		friend T min(const vector& a)
		{
			return min(a.x, a.y);
		}
		friend T max(const vector& a)
		{
			return max(a.x, a.y);
		}

		friend T length(const vector& v)
		{
			return sqrt(dot(v, v));
		}

		friend vector normalize(const vector& v)
		{
			return v * rcp(length(v));
		}

		friend vector pow(const vector& v, T exponent)
		{
			return vector(pow(v.x, exponent), pow(v.y, exponent));
		}

		friend vector lerp(const vector& a, const vector& b, T t)
		{
			return vector(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
		}

		friend vector rcp(const vector& v)
		{
			return vector(rcp(v.x), rcp(v.y));
		}

		bool operator==(const vector& v) const
		{
			return x == v.x && y == v.y;
		}
		bool operator!=(const vector& v) const
		{
			return x != v.x || y != v.y;
		}
	};

	template <typename T>
	class vector<T, 3U>
	{
	public:
		static const unsigned int dim = 3U;
		typedef T field_type;

		T x;
		T y;
		T z;

		vector() = default;

		explicit vector(T a)
		    : x(a), y(a), z(a)
		{
		}

		vector(T x, T y, T z)
		    : x(x), y(y), z(z)
		{
		}

		template <typename U>
		vector(const vector<U, 3U>& v)
		    : x(v.x), y(v.y), z(v.z)
		{
		}

		template <typename U>
		vector(const vector<U, 2U>& v, T z)
		    : x(v.x), y(v.y), z(z)
		{
		}

		template <typename U>
		vector(T _x, const vector<U, 2U>& v)
		    : x(_x), y(v.x), z(v.y)
		{
		}

		vector& operator=(const vector& v) = default;

		vector<T, 2U> xy() const
		{
			return vector<T, 2U>(x, y);
		}
		vector<T, 2U> yx() const
		{
			return vector<T, 2U>(y, x);
		}
		vector<T, 2U> xz() const
		{
			return vector<T, 2U>(x, z);
		}
		vector<T, 2U> zx() const
		{
			return vector<T, 2U>(z, x);
		}
		vector<T, 2U> yz() const
		{
			return vector<T, 2U>(y, z);
		}
		vector<T, 2U> zy() const
		{
			return vector<T, 2U>(z, y);
		}
		vector xzy() const
		{
			return vector(x, y, x);
		}
		vector yxz() const
		{
			return vector(y, x, z);
		}
		vector yzx() const
		{
			return vector(y, z, z);
		}
		vector zxy() const
		{
			return vector(z, x, y);
		}
		vector zyx() const
		{
			return vector(z, y, x);
		}

		vector operator-() const
		{
			return vector(-x, -y, -z);
		}

		vector& operator+=(const vector& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		vector& operator-=(const vector& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		vector& operator-=(T a)
		{
			x -= a;
			y -= a;
			z -= a;
			return *this;
		}

		vector& operator*=(T a)
		{
			x *= a;
			y *= a;
			z *= a;
			return *this;
		}

		vector& operator/=(T a)
		{
			x /= a;
			y /= a;
			z /= a;
			return *this;
		}

		vector& operator*=(const vector& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		T& operator[](size_t pos)
		{
			return (*(&(this->x) + pos));
		}

		T operator[](size_t pos) const
		{
			return (*(&(this->x) + pos));
		}

		friend  const vector operator+(const vector& a, const vector& b)
		{
			return vector(a.x + b.x, a.y + b.y, a.z + b.z);
		}

		friend const vector operator+(const vector& a, T b)
		{
			return vector(a.x + b, a.y + b, a.z + b);
		}

		friend const vector operator+(T a, const vector& b)
		{
			return vector(a + b.x, a + b.y, a + b.z);
		}

		friend const vector operator-(const vector& a, const vector& b)
		{
			return vector(a.x - b.x, a.y - b.y, a.z - b.z);
		}

		friend const vector operator-(const vector& a, T b)
		{
			return vector(a.x - b, a.y - b, a.z - b);
		}

		friend const vector operator-(T b, const vector& a)
		{
			return vector(b - a.x, b - a.y, b - a.z);
		}

		friend const vector operator*(T a, const vector& v)
		{
			return vector(a * v.x, a * v.y, a * v.z);
		}

		friend const vector operator/(const vector& v, T a)
		{
			return vector(v.x / a, v.y / a, v.z / a);
		}

		friend const vector operator*(const vector& v, T a)
		{
			return a * v;
		}

		friend const vector operator*(const vector& a, const vector& b)
		{
			return vector(a.x * b.x, a.y * b.y, a.z * b.z);
		}

		friend T dot(const vector& a, const vector& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		friend vector cross(const vector& a, const vector& b)
		{
			return vector(a.y * b.z - a.z * b.y,
			              a.z * b.x - a.x * b.z,
			              a.x * b.y - a.y * b.x);
		}

		friend vector abs(const vector& v)
		{
			return vector(abs(v.x), abs(v.y), abs(v.z));
		}

		friend vector floor(const vector& v)
		{
			return vector(floor(v.x), floor(v.y));
		}

		friend vector ceil(const vector& v)
		{
			return vector(ceil(v.x), ceil(v.y));
		}

		friend vector max(const vector& v, T c)
		{
			return vector(max(v.x, c), max(v.y, c), max(v.z, c));
		}
		friend vector max(T c, const vector& v)
		{
			return vector(max(v.x, c), max(v.y, c), max(v.z, c));
		}
		friend vector min(const vector& v, T c)
		{
			return vector(min(v.x, c), min(v.y, c), min(v.z, c));
		}
		friend vector min(T c, const vector& v)
		{
			return vector(min(v.x, c), min(v.y, c), min(v.z, c));
		}

		friend vector min(const vector& a, const vector& b)
		{
			return vector(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
		}
		friend vector max(const vector& a, const vector& b)
		{
			return vector(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
		}
		friend T min(const vector& a)
		{
			return min(min(a.x, a.y), a.z);
		}
		friend T max(const vector& a)
		{
			return max(max(a.x, a.y), a.z);
		}

		friend T length(const vector& v)
		{
			return sqrt(dot(v, v));
		}

		friend vector normalize(const vector& v)
		{
			return v * rcp(length(v));
		}

		friend vector pow(const vector& v, T exponent)
		{
			return vector(pow(v.x, exponent), pow(v.y, exponent), pow(v.z, exponent));
		}

		friend vector lerp(const vector& a, const vector& b, T t)
		{
			return vector(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
		}

		friend vector rcp(const vector& v)
		{
			return vector(rcp(v.x), rcp(v.y), rcp(v.z));
		}

		bool operator==(const vector& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}
		bool operator!=(const vector& v) const
		{
			return x != v.x || y != v.y || z != v.z;
		}
	};

	template <typename T>
	class vector<T, 4U>
	{
	public:
		static const unsigned int dim = 4U;
		typedef T field_type;

		T x;
		T y;
		T z;
		T w;

		vector() = default;

		explicit vector(T a)
		    : x(a), y(a), z(a), w(a)
		{
		}

		vector(T x, T y, T z, T w)
		    : x(x), y(y), z(z), w(w)
		{
		}

		template <typename U>
		vector(const vector<U, 4>& v)
		    : x(v.x), y(v.y), z(v.z), w(v.w)
		{
		}

		template <typename U>
		vector(const vector<U, 3>& v, T w)
		    : x(v.x), y(v.y), z(v.z), w(w)
		{
		}

		template <typename U>
		vector(T x, const vector<U, 3>& v)
		    : x(x), y(v.x), z(v.y), w(v.z)
		{
		}

		template <typename U>
		vector(T x, T y, const vector<U, 2>& v)
		    : x(x), y(y), z(v.x), w(v.y)
		{
		}

		template <typename U>
		vector(const vector<U, 2>& v2, T z, T w)
		    : x(v2.x), y(v2.y), z(z), w(w)
		{
		}

		template <typename U>
		vector(const vector<U, 2>& v1, const vector<U, 2>& v2)
		    : x(v1.x), y(v1.y), z(v2.x), w(v2.y)
		{
		}

		vector& operator=(const vector& v) = default;

		vector<T, 3U> xyz() const
		{
			return vector<T, 3U>(x, y, z);
		}
		vector<T, 3U> xyw() const
		{
			return vector<T, 3U>(x, y, w);
		}
		vector<T, 2U> xy() const
		{
			return vector<T, 2U>(x, y);
		}

		vector operator-() const
		{
			return vector(-x, -y, -z, -w);
		}

		vector& operator+=(const vector& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}

		vector& operator-=(const vector& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;
			return *this;
		}

		vector& operator-=(T a)
		{
			x -= a;
			y -= a;
			z -= a;
			w -= a;
			return *this;
		}

		vector& operator*=(T a)
		{
			x *= a;
			y *= a;
			z *= a;
			w *= a;
			return *this;
		}

		vector& operator*=(const vector& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			w *= v.w;
			return *this;
		}

		vector& operator/=(T a)
		{
			x /= a;
			y /= a;
			z /= a;
			w /= a;
			return *this;
		}

		friend const vector operator+(const vector& a, const vector& b)
		{
			return vector(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}

		friend const vector operator+(const vector& a, T b)
		{
			return vector(a.x + b, a.y + b, a.z + b, a.w + b);
		}

		friend const vector operator+(T a, const vector& b)
		{
			return vector(a + b.x, a + b.y, a + b.z, a + b.w);
		}

		friend const vector operator-(const vector& a, const vector& b)
		{
			return vector(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}

		friend const vector operator-(const vector& a, T b)
		{
			return vector(a.x - b, a.y - b, a.z - b, a.w - b);
		}

		friend const vector operator-(T b, const vector& a)
		{
			return vector(b - a.x, b - a.y, b - a.z, b - a.w);
		}

		friend const vector operator*(T a, const vector& v)
		{
			return vector(a * v.x, a * v.y, a * v.z, a * v.w);
		}

		friend const vector operator/(const vector& v, T a)
		{
			return vector(v.x / a, v.y / a, v.z / a, v.w / a);
		}

		friend const vector operator*(const vector& v, T a)
		{
			return a * v;
		}

		friend const vector operator*(const vector& a, const vector& b)
		{
			return vector(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}

		friend T dot(const vector& a, const vector& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}

		friend vector abs(const vector& v)
		{
			return vector(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
		}

		friend vector floor(const vector& v)
		{
			return vector(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
		}

		friend vector ceil(const vector& v)
		{
			return vector(ceil(v.x), ceil(v.y), ceil(v.z), ceil(v.w));
		}

		friend vector max(const vector& v, T c)
		{
			return vector(max(v.x, c), max(v.y, c), max(v.z, c), max(v.w, c));
		}
		friend vector max(T c, const vector& v)
		{
			return vector(max(v.x, c), max(v.y, c), max(v.z, c), max(v.w, c));
		}
		friend vector min(const vector& v, T c)
		{
			return vector(min(v.x, c), min(v.y, c), min(v.z, c), min(v.w, c));
		}
		friend vector min(T c, const vector& v)
		{
			return vector(min(v.x, c), min(v.y, c), min(v.z, c), min(v.w, c));
		}

		friend vector min(const vector& a, const vector& b)
		{
			return vector(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
		}
		friend vector max(const vector& a, const vector& b)
		{
			return vector(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
		}

		friend T min(const vector& a)
		{
			return min(min(a.x, a.y), min(a.z, a.w));
		}
		friend T max(const vector& a)
		{
			return max(max(a.x, a.y), max(a.z, a.w));
		}

		friend T length(const vector& v)
		{
			return sqrt(dot(v, v));
		}

		friend vector normalize(const vector& v)
		{
			return v * rcp(length(v));
		}

		friend vector pow(const vector& v, T exponent)
		{
			return vector(pow(v.x, exponent), pow(v.y, exponent), pow(v.z, exponent), pow(v.w, exponent));
		}

		friend vector rcp(const vector& v)
		{
			return vector(rcp(v.x), rcp(v.y), rcp(v.z), rcp(v.w));
		}

		friend vector lerp(const vector& a, const vector& b, T t)
		{
			return vector(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t));
		}

		bool operator==(const vector& v) const
		{
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}
		bool operator!=(const vector& v) const
		{
			return x != v.x || y != v.y || z != v.z || w != v.w;
		}
	};
	
	template <typename T>
	std::ostream& operator<<(std::ostream& out, const vector<T, 2U>& vec)
	{
		out << "[" << vec.x << ", " << vec.y << "]";
		return out;
	}
	
	template <typename T>
	std::ostream& operator<<(std::ostream& out, const vector<T, 3U>& vec)
	{
		out << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
		return out;
	}
	
	template <typename T>
	std::ostream& operator<<(std::ostream& out, const vector<T, 4U>& vec)
	{
		out << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]";
		return out;
	}

	template <typename T, unsigned int D>
	inline vector<T, D> clamp(const vector<T, D>& v, T lower, T upper)
	{
		return min(max(v, lower), upper);
	}

	template <typename T, unsigned int D>
	inline T length2(const vector<T, D>& v)
	{
		return dot(v, v);
	}

	//template <typename V>
	//inline auto length(const V& v) -> typename V::field_type // WORKAROUND for decltype(sqrt(dot(v,v)))
	//{
	//  return sqrt(dot(v,v));
	//}

	//template <typename V>
	//inline auto normalize(const V& v) -> V // WORKAROUND for decltype(v * rcp(length(v)))
	//{
	//  return v * rcp(length(v));
	//}

	typedef vector<float, 2U> float2;
	typedef vector<float, 3U> float3;
	typedef vector<float, 4U> float4;

	typedef vector<short, 2U> short2;
	typedef vector<short, 3U> short3;
	typedef vector<short, 4U> short4;

	typedef vector<unsigned short, 2U> ushort2;
	typedef vector<unsigned short, 3U> ushort3;
	typedef vector<unsigned short, 4U> ushort4;

	typedef vector<int, 2U> int2;
	typedef vector<int, 3U> int3;
	typedef vector<int, 4U> int4;

	typedef vector<unsigned int, 2U> uint2;
	typedef vector<unsigned int, 3U> uint3;
	typedef vector<unsigned int, 4U> uint4;

  typedef vector<unsigned char, 2U> uchar2;
  typedef vector<unsigned char, 3U> uchar3;
  typedef vector<unsigned char, 4U> uchar4;
}

using math::float2;
using math::float3;
using math::float4;

using math::short2;
using math::short3;
using math::short4;

using math::ushort2;
using math::ushort3;
using math::ushort4;

using math::int2;
using math::int3;
using math::int4;

using math::uint2;
using math::uint3;
using math::uint4;

using math::uchar2;
using math::uchar3;
using math::uchar4;

#endif // INCLUDED_MATH_VECTOR
