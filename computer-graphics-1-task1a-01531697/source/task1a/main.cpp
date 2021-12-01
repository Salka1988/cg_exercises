


#include <filesystem>
#include <iostream>

#include <framework/utils/argparse.h>

#include "Task.h"

using namespace std::literals;


std::ostream& printUsage(std::ostream& out)
{
	return out << R"""(usage: task1a [--test-intersections] <config-file>
	--test-intersections    render intersection test images
	-h --help               display this message
)""";
}

int taskMain(int argc, char* argv[])
{
	unsigned int mode = 0U;
	std::filesystem::path input_file;

	for (const char* const * a = argv + 1; *a; ++a)
	{
		if (parseBoolFlag(a, "-test-intersections"sv))
			mode |= 1U;
		else if (parseBoolFlag(a, "h"sv) || parseBoolFlag(a, "-help"sv))
			printUsage(std::cout) << std::endl;
		else if (input_file.empty())
			input_file = *a;
		else
			throw usage_error("invalid argument");
	}

	if (input_file.empty())
		throw usage_error("missing config file argument");

	Task task(input_file);

	std::filesystem::path output_dir = "output/task1a";

	std::filesystem::create_directories(output_dir);

	if (mode)
	{
		if (mode & 1U)
			task.renderIntersectionTest((output_dir/input_file.filename()).replace_extension(".intersections.pfm"));
	}
	else
		task.render((output_dir/input_file.filename()).replace_extension(".png"));

	return 0;
}
