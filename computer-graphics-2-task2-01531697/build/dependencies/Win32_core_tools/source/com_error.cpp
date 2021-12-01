


#include <sstream>

#include "com_error.h"


namespace
{
	std::wstring error_message(HRESULT hresult)
	{
		std::wostringstream str;
		str << L"HRESULT(" << hresult << L')';
		return str.str();
	}
}

namespace COM
{
	default_error::default_error(HRESULT hresult)
		: msg(error_message(hresult))
	{
	}

	default_error::default_error(const std::wstring& message)
		: msg(message)
	{
	}

	default_error::default_error(std::wstring&& message)
		: msg(std::move(message))
	{
	}

	const wchar_t* default_error::message() const
	{
		return msg.c_str();
	}
}
