#pragma once

#ifndef INCLUDED_SCENE_CONFIG
#define INCLUDED_SCENE_CONFIG

#include "math/vector.h"
#include <string>
#include <vector>

struct Sphere
{
  float3 position;
  float radius;
};

struct SceneConfig
{
  float dt = 0.005f;
  float damp = 0.997f;
  int fixup_iterations = 30;
  float fixup_percent = 0.3f;

  float2 cloth_size = { 1.f, 1.f };
  int2 grid_size = { 20, 20 };
  float cloth_mass = 1.f;
  float cloth_y_offset = 0.f;
  std::vector<std::pair<int, int>> fixed_particles;

  float3 gravity = { 0.f, -9.8f, 0.f };
  std::vector<Sphere> obstacles;
  float3 wind = { 0.f, 0.f, 0.f };
  float wind_multiplier = 1.f;
  float wind_per_particle_factor = 0.f;

  bool apply_structural_fixup = true;
  bool apply_shear_fixup = true;
  bool apply_flexion_fixup = true;

  static SceneConfig read(const std::string& path);
  static void write(const SceneConfig& config, const std::string& path);
};

#endif // INCLUDED_SCENE_CONFIG