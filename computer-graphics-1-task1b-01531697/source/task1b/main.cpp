


#include <thread>
#include <filesystem>
#include <iostream>

#include <framework/utils/argparse.h>

#include "Task.h"

using namespace std::literals;


std::ostream& printUsage(std::ostream& out)
{
	return out << R"""(usage: task1b [-b <max-bounces>] [-t [<num-threads>]|--num-threads <num-threads>] [--test-intersections] [--test-shading] <config-file>
	-w --res-x              override horizontal resolution
	-h --res-y              override vertical resolution
	-b                      set maximum number of bounces
	--test-intersections    render intersection test images
	--test-shading          render shading test images
	-t --num-threads        render in parallel using the specified number of threads (default when only -t is given: )""" << std::thread::hardware_concurrency() <<  R"""()
	--no-cache              do not use or generate a task cache file
	-h --help               display this message
)""";
}

int taskMain(int argc, char* argv[])
{
	int res_x = 0;
	int res_y = 0;
	unsigned int mode = 0U;
	int num_threads = 1;
	std::filesystem::path input_file;
	bool no_cache = false;
	int max_bounces = 3;

	for (const char* const * a = argv + 1; *a; ++a)
	{
		if (parseIntArgument(num_threads, a, "t"sv, static_cast<int>(std::thread::hardware_concurrency())));
		else if (parseIntArgument(num_threads, a, "-num-threads"sv));
		else if (parseIntArgument(res_x, a, "w"sv) || parseIntArgument(res_x, a, "-res-x"sv));
		else if (parseIntArgument(res_y, a, "h"sv) || parseIntArgument(res_y, a, "-res-y"sv));
		else if (parseBoolFlag(a, "-test-intersections"sv))
			mode |= 1U;
		else if (parseBoolFlag(a, "-test-shading"sv))
			mode |= 2U;
		else if (parseBoolFlag(a, "-no-cache"sv))
			no_cache = true;
		else if (parseIntArgument(max_bounces, a, "b"sv));
		else if (parseBoolFlag(a, "h"sv) || parseBoolFlag(a, "-help"sv))
			printUsage(std::cout) << std::endl;
		else if (input_file.empty())
			input_file = *a;
		else
			throw usage_error("invalid argument");
	}

	if (input_file.empty())
		throw usage_error("missing config file argument");

	std::filesystem::path output_dir = "output/task1b";

	std::filesystem::create_directories(output_dir);

	Task task(output_dir, input_file, no_cache);

	if (mode)
	{
		if (mode & 1U)
			task.renderIntersectionTest((output_dir/input_file.filename()).replace_extension(".intersections.pfm"), res_x, res_y, num_threads);

		if (mode & 2U)
			task.renderShadingTest((output_dir/input_file.filename()).replace_extension(".shading.pfm"), res_x, res_y, num_threads);
	}
	else
		task.render((output_dir/input_file.filename()).replace_extension(".png"), res_x, res_y, max_bounces, num_threads);

	return 0;
}
