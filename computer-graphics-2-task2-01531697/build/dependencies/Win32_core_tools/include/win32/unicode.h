


#ifndef INCLUDED_WIN32_UNICODE
#define INCLUDED_WIN32_UNICODE

#pragma once

#include <string>


namespace Win32
{
	std::wstring widen(const char* string);
	std::wstring widen(const char* string, size_t length);
	std::wstring widen(const std::string& string);

	template <size_t N>
	inline std::wstring widen(const char (&string)[N])
	{
		return widen(string, N - 1);
	}

	std::string narrow(const wchar_t* string);
	std::string narrow(const wchar_t* string, size_t length);
	std::string narrow(const std::wstring& string);
	
	template <size_t N>
	inline std::string narrow(const wchar_t (&string)[N])
	{
		return narrow(string, N - 1);
	}
}

using Win32::widen;
using Win32::narrow;

#endif	// INCLUDED_WIN32_UNICODE
