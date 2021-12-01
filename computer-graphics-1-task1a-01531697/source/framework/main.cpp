


#include <exception>
#include <filesystem>
#include <iostream>

#include <framework/config.h>
#include <framework/utils/argparse.h>


std::ostream& printUsage(std::ostream& out);
int taskMain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	try
	{
		taskMain(argc, argv);
	}
	catch (const usage_error& e)
	{
		std::cerr << e.what() << '\n';
		printUsage(std::cout) << std::endl;
		return -1;
	}
	catch (const config::parse_error& e)
	{
		std::cerr << "ERROR parsing config file: " << e.what() << '\n';
		return -1;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		return -1;
	}
	catch (...)
	{
		std::cerr << "ERROR: unknown exception\n";
		return -128;
	}

	return 0;
}
