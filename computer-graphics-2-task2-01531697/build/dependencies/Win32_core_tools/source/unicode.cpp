


#include <cstring>
#include <cwchar>

#include "platform.h"
#include "unicode.h"


namespace Win32
{
	std::wstring widen(const char* string)
	{
		return widen(string, std::strlen(string));
	}

	std::wstring widen(const char* string, size_t length)
	{
		size_t output_length = MultiByteToWideChar(CP_UTF8, 0U, string, static_cast<int>(length), nullptr, 0);
		std::wstring str(output_length, L'\0');
		if (MultiByteToWideChar(CP_UTF8, 0U, string, static_cast<int>(length), &str[0], static_cast<int>(output_length)) <= 0)
			throw std::runtime_error("MultiByteToWideChar() failed");
		return str;
	}

	std::wstring widen(const std::string& string)
	{
		return widen(&string[0], string.length());
	}


	std::string narrow(const wchar_t* string)
	{
		return narrow(string, std::wcslen(string));
	}

	std::string narrow(const wchar_t* string, size_t length)
	{
		size_t output_length = WideCharToMultiByte(CP_UTF8, 0U, string, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
		std::string str;
		str.resize(output_length);
		WideCharToMultiByte(CP_UTF8, 0U, &string[0], static_cast<int>(length), &str[0], static_cast<int>(output_length), nullptr, nullptr);
		return str;
	}

	std::string narrow(const std::wstring& string)
	{
		size_t output_length = WideCharToMultiByte(CP_UTF8, 0U, &string[0], static_cast<int>(string.length()), nullptr, 0, nullptr, nullptr);
		std::string str;
		str.resize(output_length);
		WideCharToMultiByte(CP_UTF8, 0U, &string[0], static_cast<int>(string.length()), &str[0], static_cast<int>(output_length), nullptr, nullptr);
		return str;
	}
}
