


#ifndef INCLUDED_FRAMEWORK_COLOR
#define INCLUDED_FRAMEWORK_COLOR

#pragma once


template <typename C>
constexpr decltype(auto) r(const C& color)
{
	return channel<0>(color);
}

template <typename C>
constexpr decltype(auto) g(const C& color)
{
	return channel<1>(color);
}

template <typename C>
constexpr decltype(auto) b(const C& color)
{
	return channel<2>(color);
}

template <typename C>
constexpr decltype(auto) a(const C& color)
{
	return channel<3>(color);
}

#endif  // INCLUDED_FRAMEWORK_COLOR
