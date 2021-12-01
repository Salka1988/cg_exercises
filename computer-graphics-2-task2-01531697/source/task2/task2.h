#ifndef INCLUDED_TASK2
#define INCLUDED_TASK2

#pragma once

#include "Renderer.h"
#include "StepImportExport.h"
#include "Camera.h"
#include "Cloth.h"
#include "math/vector.h"
#include <algorithm>
#include <array>
#include <vector>


void calcNormals(std::vector<float3>& normals, const std::vector<float3>& positions, int grid_x, int grid_y);

void verletIntegration(
  std::vector<float3>& p_next, const std::vector<float3>& p_cur, const std::vector<float3>& p_prev, 
  const std::vector<float3>& normals, const std::vector<float>& inv_masses, float damp, const float3& gravity, const float3& wind_force, float dt);

float3 calcWindAcc(const float3& normal, const float3& wind_force, float inv_mass);

void fixupStep(
  std::vector<float3>& p_out, const std::vector<float3>& p_in, 
  int grid_x, int grid_y, float cloth_x, float cloth_y,
  const std::vector<float>& inv_masses, 
  float fixup_percent, const std::vector<Sphere>& obstacles, bool apply_structural_fixup, bool apply_shear_fixup, bool apply_flexion_fixup);

void fixCollision(float3& particle_position, const Sphere& sphere);

void simulationStep(
  std::array<std::vector<float3>, 3>& p_buffers, int& next, int& current, int& previous,
  int grid_x, int grid_y, float cloth_x, float cloth_y,
  const std::vector<float>& inv_masses, std::vector<float3>& normals,
  const float3& gravity, const float3& wind_force,
  float damping, float fixup_percent, int fixup_iterations,
  const std::vector<Sphere>& obstacles,
  float dt, bool apply_structural_fixup, bool apply_shear_fixup, bool apply_flexion_fixup);

int flattenIndex(int x, int y, int size_x);

#endif // INCLUDED_TASK2
