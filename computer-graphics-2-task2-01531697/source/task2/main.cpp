#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include <GL/platform/Application.h>
#include <GL/platform/Window.h>

#include "Camera.h"
#include "InputHandler.h"
#include "OrbitalNavigator.h"
#include "Renderer.h"
#include "StepImportExport.h"
#include "StepTest.h"

#include "framework/argparse.h"
#include "framework/png.h"

void printUsage(const char* argv0)
{
   std::cout << "usage: " << argv0
    << " [options]\n"
    "options:\n"
    "  --frozen                       (start with simulation frozen)\n"
    "  --windowsize <height> <width>  (window size in NxM)\n"
    "  --sceneconfig <path>           (scene configuration file)\n"
    "  --gridsize <N> <M>             (use an NxM particle grid)\n"
    "  --clothsize <N> <M>            (set the size of the cloth to NxM)\n"
    "  --mass <N>                     (mass of the cloth)\n"
    "  --scene <N>                    (selected scene (1-7)\n"
    "  --wind <N>                     (selected wind (0-5)\n"
    "         0 = NoWind\n"
    "         1 = LowSide\n"
    "         2 = StrongSide\n"
    "         3 = LowBottom\n"
    "         4 = StrongBottom\n"
    "         5 = MovingSide\n"
    "  --navigator_radius <N>        (navigator radius)\n"
    "  --cloth_y <N>                 (cloth offset in y direction)\n"
    "  --output <path>               (specifies output path for all files to be written)"
    "  --write <N> <M>               (write every <N> step/frame, max <M> step/frame)"
    "  --images                      (write image using [--write <N> <M>])\n"
    "  --steplog                     (write step results and intermediate values using [--write <N> <M>])\n"
    "  --test <path>                 (write step result using stepdata <path>)"
    ;
}

int main(int argc, char* argv[])
{
	try
	{
    bool start_frozen = false;
    int2 window_size = { 800, 600 };
    std::string scene_config_file = "";
    int2 grid_size = {50, 50};
    float2 cloth_size = {1.f, 1.f};
    float mass = 1.f;
    int scene = 0;
    int wind = 0;
    float navigator_radius = 3.f;
    float cloth_y = 0.5f;
    std::string output_path = "";
    int mod_write = 0;
    int max_write = 0;
    bool write_images = false;
    bool write_steplog = false;
    std::string steptest_path = "";


    const char frozen_token[] = "--frozen";
    const char window_size_token[] = "--windowsize";
    const char scene_config_token[] = "--sceneconfig";
    const char grid_size_token[] = "--gridsize";
    const char cloth_size_token[] = "--clothsize";
    const char mass_token[] = "--mass";
    const char scene_token[] = "--scene";
    const char wind_token[] = "--wind";
    const char navigator_radius_token[] = "--navigator_radius";
    const char cloth_y_token[] = "--cloth_y";
    const char output_path_token[] = "--output";
    const char write_token[] = "--write";
    const char write_images_token[] = "--images";
    const char write_steplog_token[] = "--steplog";
    const char write_steptest_token[] = "--test";

    for (char** a = &argv[1]; *a; ++a)
    {
      if (std::strcmp("--help", *a) == 0)
      {
        printUsage(argv[0]);
        return 0;
      }
      if (!argparse::checkArgument(frozen_token, a, start_frozen))
        if (!argparse::checkArgument(window_size_token, a, window_size.x, window_size.y))
          if (!argparse::checkArgument(scene_config_token, a, scene_config_file))
            if (!argparse::checkArgument(grid_size_token, a, grid_size.x, grid_size.y))
              if (!argparse::checkArgument(cloth_size_token, a, cloth_size.x, cloth_size.y))
                if (!argparse::checkArgument(mass_token, a, mass))
                  if (!argparse::checkArgument(scene_token, a, scene))
                    if (!argparse::checkArgument(wind_token, a, wind))
                      if (!argparse::checkArgument(navigator_radius_token, a, navigator_radius))
                        if (!argparse::checkArgument(cloth_y_token, a, cloth_y))
                          if (!argparse::checkArgument(output_path_token, a, output_path))
                            if (!argparse::checkArgument(write_token, a, mod_write, max_write))
                              if (!argparse::checkArgument(write_images_token, a, write_images))
                                if (!argparse::checkArgument(write_steplog_token, a, write_steplog))
                                  if (!argparse::checkArgument(write_steptest_token, a, steptest_path))
                                    std::cout << "warning: unknown option " << *a
                                    << " will be ignored" << std::endl;
    }

    if ((write_images || write_steplog) && (mod_write == 0 || max_write == 0))
      throw std::runtime_error("--write <N> <M>  => N and M have to be > 0");

    if (start_frozen)
    {
      std::cout << "starting with frozen simulation" << std::endl;
    }

    if (write_steplog)
    {
      StepExport::getInstance().setExportModulo(mod_write);
      StepExport::getInstance().setOutputPath(output_path);
    }

    if (steptest_path.empty())
    {
      // view mode
      OrbitalNavigator navigator(3.1415f / 4.f, 3.1415f / 8.f, navigator_radius, math::float3(0.f, 0.f, 0.f));
      Camera camera(navigator, 0.1f, 100.f, 45.f, window_size.x, window_size.y);

      GL::platform::Window window("CG2 — Cloth Simulation", window_size.x, window_size.y, 24, 0, false);
      Renderer renderer(window, camera, scene, scene_config_file, grid_size, cloth_size, mass, cloth_y, mod_write, max_write, output_path, write_images);

      if (wind != 0)
        renderer.setWind(static_cast<Renderer::WindType>(wind));

      InputHandler input_handler(renderer, navigator);

      window.attach(static_cast<GL::platform::KeyboardInputHandler*>(&input_handler));
      window.attach(static_cast<GL::platform::MouseInputHandler*>(&input_handler));

      GL::platform::run(renderer);
    }
    else
    {
      // step test mode
      StepTest::executeTests(scene_config_file, steptest_path, output_path);
    }

  }
  catch (std::exception& e)
  {
    std::cout << "error: " << e.what() << std::endl;
    return -1;
  }
  catch (...)
  {
    std::cout << "unknown exception" << std::endl;
    return -128;
  }

  return 0;
}
