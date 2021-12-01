#include "SceneConfig.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

namespace
{
  void jsonWriteFloat3(rapidjson::Document& document, const char* name, const float3& val)
  {
    rapidjson::Value arr(rapidjson::kArrayType);
    arr.PushBack(val.x, document.GetAllocator());
    arr.PushBack(val.y, document.GetAllocator());
    arr.PushBack(val.z, document.GetAllocator());
    document.AddMember(rapidjson::StringRef(name), arr, document.GetAllocator());
  }

  float3 jsonReadFloat3(rapidjson::Document& document, const char* name)
  {
    rapidjson::Value& arr = document[name];
    return float3(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat());
  }
}


SceneConfig SceneConfig::read(const std::string& path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error("error open sceneconfig file '" + path + "'");

  rapidjson::IStreamWrapper json_is(file);
  rapidjson::Document d;
  rapidjson::ParseResult ok = d.ParseStream(json_is);
  if (!ok)
    throw std::runtime_error("error parsing sceneconfig file '" + path + "'");

  SceneConfig config;

  config.dt = d["dt"].GetFloat();
  config.damp = d["damp"].GetFloat();
  config.fixup_iterations = d["fixup_iterations"].GetInt();
  config.fixup_percent = d["fixup_percent"].GetFloat();

  config.cloth_size.x = d["cloth_size_x"].GetFloat();
  config.cloth_size.y = d["cloth_size_y"].GetFloat();

  config.grid_size.x = d["grid_size_x"].GetInt();
  config.grid_size.y = d["grid_size_y"].GetInt();

  config.cloth_mass = d["cloth_mass"].GetFloat();
  config.cloth_y_offset = d["cloth_y_offset"].GetFloat();

  config.gravity = jsonReadFloat3(d, "gravity");
  config.wind = jsonReadFloat3(d, "wind");

  config.wind_multiplier = d["wind_multiplier"].GetFloat();
  config.wind_per_particle_factor = d["wind_per_particle_factor"].GetFloat();

  if(d.HasMember("apply_structural_fixup"))
    config.apply_structural_fixup = d["apply_structural_fixup"].GetBool();

  if (d.HasMember("apply_shear_fixup"))
    config.apply_shear_fixup = d["apply_shear_fixup"].GetBool();
  
  if (d.HasMember("apply_flexion_fixup"))
    config.apply_flexion_fixup = d["apply_flexion_fixup"].GetBool();

  for (rapidjson::SizeType i = 0; i < d["fixed_particles"].Size(); i++)
  {
    rapidjson::Value& json_index = d["fixed_particles"][i];
    config.fixed_particles.push_back(std::make_pair(json_index[0].GetInt(), json_index[1].GetInt()));
  }

  for (rapidjson::SizeType i = 0; i < d["obstacles"].Size(); i++)
  {
    rapidjson::Value& json_sphere = d["obstacles"][i];
    Sphere sphere;
    sphere.radius = json_sphere["radius"].GetFloat();
    sphere.position.x = json_sphere["position"][0].GetFloat();
    sphere.position.y = json_sphere["position"][1].GetFloat();
    sphere.position.z = json_sphere["position"][2].GetFloat();
    config.obstacles.push_back(sphere);
  }

  return config;
}

void SceneConfig::write(const SceneConfig& config, const std::string& path)
{
  rapidjson::Document d;
  d.SetObject();

  d.AddMember("dt", config.dt, d.GetAllocator());
  d.AddMember("damp", config.damp, d.GetAllocator());
  d.AddMember("fixup_iterations", config.fixup_iterations, d.GetAllocator());
  d.AddMember("fixup_percent", config.fixup_percent, d.GetAllocator());

  d.AddMember("cloth_size_x", config.cloth_size.x, d.GetAllocator());
  d.AddMember("cloth_size_y", config.cloth_size.y, d.GetAllocator());
  d.AddMember("grid_size_x", config.grid_size.x, d.GetAllocator());
  d.AddMember("grid_size_y", config.grid_size.y, d.GetAllocator());
  d.AddMember("cloth_mass", config.cloth_mass, d.GetAllocator());
  d.AddMember("cloth_y_offset", config.cloth_y_offset, d.GetAllocator());

  jsonWriteFloat3(d, "gravity", config.gravity);
  jsonWriteFloat3(d, "wind", config.wind);
  d.AddMember("wind_multiplier", config.wind_multiplier, d.GetAllocator());
  d.AddMember("wind_per_particle_factor", config.wind_per_particle_factor, d.GetAllocator());

  rapidjson::Value json_fixed_particles(rapidjson::kArrayType);
  for (const auto& fixed_particle : config.fixed_particles)
  {
    rapidjson::Value json_index(rapidjson::kArrayType);
    json_index.PushBack(fixed_particle.first, d.GetAllocator());
    json_index.PushBack(fixed_particle.second, d.GetAllocator());
    json_fixed_particles.PushBack(json_index, d.GetAllocator());
  }
  d.AddMember("fixed_particles", json_fixed_particles, d.GetAllocator());


  rapidjson::Value json_obstacles(rapidjson::kArrayType);
  for (const auto& sphere: config.obstacles)
  {
    rapidjson::Value json_sphere(rapidjson::kObjectType);
    json_sphere.AddMember("radius", sphere.radius, d.GetAllocator());
    rapidjson::Value position(rapidjson::kArrayType);
    position.PushBack(sphere.position.x, d.GetAllocator());
    position.PushBack(sphere.position.y, d.GetAllocator());
    position.PushBack(sphere.position.z, d.GetAllocator());
    json_sphere.AddMember("position", position, d.GetAllocator());
    json_obstacles.PushBack(json_sphere, d.GetAllocator());
  }
  d.AddMember("obstacles", json_obstacles, d.GetAllocator());

  // write
  std::ofstream ofs(path);
  rapidjson::OStreamWrapper osw(ofs);
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
  d.Accept(writer);
}
