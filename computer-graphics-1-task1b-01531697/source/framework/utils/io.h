


#ifndef INCLUDED_FRAMEWORK_UTILS_IO
#define INCLUDED_FRAMEWORK_UTILS_IO

#pragma once

#include <type_traits>
#include <iostream>


template <typename T>
T read(std::istream& in)
{
	static_assert(std::is_trivially_copyable_v<T>);

	T value;
	in.read(reinterpret_cast<char*>(&value), sizeof(T));
	return value;
}

template <typename T>
std::istream& read(T& value, std::istream& in)
{
	static_assert(std::is_trivially_copyable_v<T>);
	return in.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
std::istream& read(T* values, std::istream& in, size_t count)
{
	static_assert(std::is_trivially_copyable_v<T>);
	return in.read(reinterpret_cast<char*>(values), sizeof(T) * count);
}

template <typename T, size_t N>
std::istream& read(T(&values)[N], std::istream& in)
{
	static_assert(std::is_trivially_copyable_v<T>);
	return in.read(reinterpret_cast<char*>(values), sizeof(values));
}

template <typename T>
std::ostream& write(std::ostream& out, T value)
{
	static_assert(std::is_trivially_copyable_v<T>);
	return out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
std::ostream& write(std::ostream& out, const T* values, size_t count)
{
	static_assert(std::is_trivially_copyable_v<T>);
	return out.write(reinterpret_cast<const char*>(values), sizeof(T) * count);
}

template <typename T, size_t N>
std::ostream& write(std::ostream& out, const T(&values)[N])
{
	static_assert(std::is_trivially_copyable_v<T>);
	return out.write(reinterpret_cast<const char*>(values), sizeof(values));
}


template <typename Arg>
inline std::ostream& concat(std::ostream& out, Arg&& arg)
{
	return out << arg;
}

template <typename Arg, typename... Args>
inline std::ostream& concat(std::ostream& out, Arg&& arg, Args&&... args)
{
	return concat(out << arg, std::forward<Args>(args)...);
}

#endif  // INCLUDED_FRAMEWORK_UTILS_IO
