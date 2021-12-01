


#ifndef INCLUDED_FRAMEWORK_UTILS_COLOR
#define INCLUDED_FRAMEWORK_UTILS_COLOR

#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>

#include <framework/math/vector.h>


inline std::uint8_t encode8(float c)
{
	return static_cast<std::uint8_t>(std::min(std::max(c, 0.0f), 1.0f) * 255.0f);
}

inline float decode8(std::uint8_t v)
{
	return v * (1.0f / 255.0f);
}

inline std::uint8_t gammaencode8(float c)
{
	return encode8(std::pow(c, 1.0f / 2.2f));
}

inline float gammadecode8(std::uint8_t v)
{
	return std::pow(decode8(v), 2.2f);
}

inline std::uint32_t gammaencodeR8G8B8(const float3& c)
{
	return gammaencode8(c.x) | (gammaencode8(c.y) << 8) | (gammaencode8(c.z) << 16);
}

inline float3 gammadecodeR8G8B8(std::uint32_t v)
{
	return { gammadecode8(static_cast<std::uint8_t>(v)), gammadecode8(static_cast<std::uint8_t>(v >> 8)), gammadecode8(static_cast<std::uint8_t>(v >> 16)) };
}

inline std::uint32_t gammaencodeR8G8B8A8(const float4& c)
{
	return gammaencodeR8G8B8({ c.x, c.y, c.z }) | (encode8(c.w) << 24);
}

inline float4 gammadecodeR8G8B8A8(std::uint32_t v)
{
	return { gammadecodeR8G8B8(v), decode8(static_cast<std::uint8_t>(v >> 24)) };
}

#endif  // INCLUDED_FRAMEWORK_UTILS_COLOR
