


#ifndef INCLUDED_WIN32_COM_ERROR
#define INCLUDED_WIN32_COM_ERROR

#pragma once

#include <string>

#include "platform.h"


namespace COM
{
	class error
	{
	public:
		virtual ~error() {}

		virtual const wchar_t* message() const = 0;
	};

	class default_error
	{
	private:
		std::wstring msg;
	public:
		explicit default_error(HRESULT hresult);
		default_error(const std::wstring& message);
		default_error(std::wstring&& message);

		const wchar_t* message() const;
	};

	inline void checkError(HRESULT hresult)
	{
		if (FAILED(hresult))
			throw default_error(hresult);
	}
}

#endif  // INCLUDED_WIN32_COM_ERROR
