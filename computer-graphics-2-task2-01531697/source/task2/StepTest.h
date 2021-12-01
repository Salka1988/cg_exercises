#pragma once

#ifndef INCLUDED_STEP_TEST
#define INCLUDED_STEP_TEST

#include <string>

class StepTest
{
public:
  static void executeTests(const std::string& scene_config_file, const std::string& test_data_path, const std::string& output_path);
};

#endif // INCLUDED_STEP_TEST