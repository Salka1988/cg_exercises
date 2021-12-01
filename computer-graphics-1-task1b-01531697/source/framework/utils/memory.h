


#ifndef INCLUDED_FRAMEWORK_UTILS_MEMORY
#define INCLUDED_FRAMEWORK_UTILS_MEMORY

#pragma once

#include <type_traits>
#include <memory>


template <typename T>
inline auto make_unique_default()
{
	return std::unique_ptr<T> { new T };
}

template <typename T>
inline std::enable_if_t<std::is_array<T>::value && (std::extent<T>::value == 0), std::unique_ptr<T>> make_unique_default(std::size_t size)
{
	return std::unique_ptr<T> { new std::remove_extent_t<T>[size] };
}

#endif  // INCLUDED_FRAMEWORK_UTILS_MEMORY
