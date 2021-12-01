#ifndef INCLUDED_CLOTH
#define INCLUDED_CLOTH

#pragma once

#include "GL/buffer.h"
#include "GL/vertex_array.h"
#include "math/vector.h"
#include <array>
#include <vector>

struct Sphere;

class Cloth
{
  private:
    GL::Buffer vbo_;
    GL::Buffer ebo_;
    GL::VertexArray vao_;

    float2 cloth_size_ = {1.f, 1.f};
    int2 grid_size_ = {10, 10};
    size_t total_grid_size_ = grid_size_.x * grid_size_.y;

    float cloth_mass_ = 0.05f;
    float particle_mass_ = cloth_mass_ / total_grid_size_;

    int next_, current_, prev_;
    std::array<std::vector<float3>, 3> p_buffers_;
    std::vector<float3> normals_;
    std::vector<float> inv_masses_;
    size_t num_indices_ = 0;

    void setupBuffers();
    void writePositionData(const std::vector<float3>& p);
    void writeNormalData(const std::vector<float3>& p);
    void writeColorData(const std::vector<float4>& p);

    size_t getPositionDataSize() const;
    size_t getNormalDataSize() const;
    size_t getColorDataSize() const;

  public:
    Cloth() { initGrid({ 0.f, 0.f, 0.f}, grid_size_, cloth_size_, cloth_mass_, {}); }

    void initGrid(const float3& pos_offset, int2 grid_size, const float2& cloth_size, float cloth_mass, const std::vector<std::pair<int, int>>& fixed_particles);
    void render() const;
    void doSimStep(const float3& gravity, const float3& wind, float damping, float fixup_percent, int fixup_iterations, const std::vector<Sphere>& obstacles, float dt, bool apply_structural_fixup, bool apply_shear_fixup, bool apply_flexion_fixup);
    void updateBuffers();
};

#endif // INCLUDED_CLOTH