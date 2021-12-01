


#include <cstring>

#include "argparse.h"


namespace
{
	bool isOption(const char* arg)
	{
		return *arg && *arg == '-';
	}

	bool compareOption(const char* arg, std::string_view option)
	{
		if (!isOption(arg))
			return false;
		return std::strncmp(arg + 1, option.data(), option.length()) == 0;
	}

	const char* findArg(const char* const *& argv, std::string_view option)
	{
		const char* startptr = *argv + option.length();

		if (*startptr)
			return startptr;

		startptr = *++argv;

		if (*startptr)
			return startptr;

		return nullptr;
	}
}

bool parseBoolFlag(const char* const *& argv, std::string_view option)
{
	if (!isOption(*argv))
		return false;
	if (*argv + 1 == option)
		return true;
	return false;
}

bool parseStringArgument(const char*& value, const char* const *& argv, std::string_view option)
{
	if (!compareOption(*argv, option))
		return false;

	const char* arg = findArg(argv, option);

	if (!arg)
		throw usage_error("expected argument");

	value = arg;
	return true;
}

bool parseIntArgument(int& value, const char* const *& argv, std::string_view option)
{
	if (!compareOption(*argv, option))
		return false;

	const char* arg = findArg(argv, option);

	if (!arg)
		throw usage_error("expected integer argument");

	char* endptr = nullptr;

	int v = std::strtol(arg, &endptr, 10);

	if (*endptr)
		throw usage_error("argument is not an integer");

	value = v;
	return true;
}
