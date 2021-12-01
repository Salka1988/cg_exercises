#include "StepTest.h"
#include "StepImportExport.h"
#include "SceneConfig.h"
#include "task2.h"
#include "math/vector.h"
#include <experimental/filesystem>

#include <iostream>
#include <vector>

namespace fs = std::experimental::filesystem;

template<typename T>
void writeArray(std::ofstream& ofs, const std::vector<T>& data)
{
  uint32_t size_in_bytes = static_cast<uint32_t> (data.size() * sizeof(T));
  ofs.write(reinterpret_cast<const char*>(&size_in_bytes), sizeof(uint32_t));
  ofs.write(reinterpret_cast<const char*>(data.data()), size_in_bytes);
}

void StepTest::executeTests(const std::string& scene_config_file, const std::string& test_data_path, const std::string& output_path)
{
  // Read scene config
  auto config = SceneConfig::read(scene_config_file);
  size_t num_particles = config.grid_size.x * config.grid_size.y;

  // Read inverse masses
  StepImport importer(test_data_path);
  std::vector<float> inv_masses = importer.getInvMasses();

  // Setup some buffers
  std::array<std::vector<float3>, 2> fixup_buffers;
  for (auto& b : fixup_buffers)
    b.resize(num_particles);

  std::vector<float3> rslt_normals(num_particles);
  std::vector<float3> rslt_verlet(num_particles);
  std::vector<float3> rslt_windacc(num_particles);
  std::vector<float3> rslt_fixup(num_particles);

  uint32_t step_cnt = 0;

  // Collisions
  int3 dim = { 11, 11, 11 };
  float size = 2.f;
  float3 spacing = { size / (dim.x - 1), size / (dim.y - 1), size / (dim.z - 1) };

  std::vector<float3> rslt_collision(dim.x * dim.y * dim.z);
  for (int y = 0; y < dim.y; y++)
    for (int x = 0; x < dim.x; x++)
      for (int z = 0; z < dim.z; z++)
        rslt_collision[x + z * dim.x + y * dim.x * dim.z] = float3(spacing.x * x, spacing.y * y, spacing.z * z) - float3(size / 2.f);

  Sphere sphere = { {1.f, 0.25f , -0.5f}, 1.8f };
  for (auto& particle_position : rslt_collision)
    fixCollision(particle_position, sphere);

  // Open result file
  fs::path result_path = fs::path(output_path) / "results";
  std::ofstream ofs(result_path, std::fstream::binary);
  if (!ofs.is_open())
    throw std::runtime_error("error writing results file");

  // Write collision results
  writeArray(ofs, rslt_collision);
  ofs.flush();

  // Iterate over step files and execute task functions
  Step step;
  while (importer.readNextStep(step))
  {
    std::cout << "Executing step: " << step.num << std::endl;

    // Normals
    calcNormals(rslt_normals, step.position_current, config.grid_size.x, config.grid_size.y);

    // Verlet-Integration
    verletIntegration(rslt_verlet, step.position_current, step.position_previous, step.normals, inv_masses, config.damp, config.gravity, config.wind, config.dt);

    // Wind accelerations
    for (size_t i = 0; i < rslt_windacc.size(); i++)
      rslt_windacc[i] = calcWindAcc(step.normals[i], config.wind, inv_masses[i]);

    // Fixup
    int out = 1, in = 0;
    fixup_buffers[in] = std::move(step.position_verlet);
    for (int i = 0; i < config.fixup_iterations; i++)
    {
      fixupStep(fixup_buffers[out], fixup_buffers[in], config.grid_size.x, config.grid_size.y, config.cloth_size.x, config.cloth_size.y,
        inv_masses, config.fixup_percent, config.obstacles, config.apply_structural_fixup, config.apply_shear_fixup, config.apply_flexion_fixup);
      std::swap(in, out);
    }
    rslt_fixup = fixup_buffers[out];

    // Write results to file
    auto step_num32 = static_cast<uint32_t> (step.num);
    ofs.write(reinterpret_cast<const char*>(&step_num32), sizeof(uint32_t));
    writeArray(ofs, rslt_normals);
    writeArray(ofs, rslt_verlet);
    writeArray(ofs, rslt_windacc);
    writeArray(ofs, rslt_fixup);
    ofs.flush();
    step_cnt++;
  }

  std::cout << "Results written to '" + result_path.string() + "'" << std::endl;
}