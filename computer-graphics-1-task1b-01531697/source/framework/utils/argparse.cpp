


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
		const char* startptr = *argv + option.length() + 1;

		if (*startptr)
			return startptr;

		if (auto next = argv + 1; *next && !isOption(*next))
			return *++argv;

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

bool parseIntArgument(int& value, const char* const *& argv, std::string_view option, int default_value)
{
	if (!compareOption(*argv, option))
		return false;

	auto new_argv = argv;
	const char* arg = findArg(new_argv, option);

	if (!arg)
	{
		value = default_value;
		return true;
	}

	char* endptr = nullptr;

	int v = std::strtol(arg, &endptr, 10);

	if (*endptr)
	{
		value = default_value;
		return true;
	}

	argv = new_argv;
	value = v;
	return true;
}
