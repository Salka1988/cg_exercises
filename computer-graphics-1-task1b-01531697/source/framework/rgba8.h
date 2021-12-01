


#ifndef INCLUDED_FRAMEWORK_COLOR_RGBA8
#define INCLUDED_FRAMEWORK_COLOR_RGBA8

#pragma once

#include <cstdint>


class RGBA8
{
	std::uint32_t color;

public:
	RGBA8() = default;

	constexpr explicit RGBA8(std::uint32_t color)
		: color(color)
	{
	}

	constexpr RGBA8(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255U)
		: color(r | (g << 8U) | (b << 16U) | (a << 24U))
	{
	}

	constexpr operator std::uint32_t() const { return color; }

	template <int i>
	friend constexpr auto channel(const RGBA8& color);
};

template <int i>
constexpr auto channel(const RGBA8& color)
{
	static_assert(i >= 0 && i < 4, "invalid color channel index");
	return static_cast<std::uint8_t>(color.color >> (8U * i));
}

#endif  // INCLUDED_FRAMEWORK_COLOR_RGBA8
