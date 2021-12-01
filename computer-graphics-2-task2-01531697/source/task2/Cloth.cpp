#include "Cloth.h"
#include "task2.h"
#include "GL/gl.h"
#include "GL/error.h"


void Cloth::setupBuffers()
{
  // Create indices
  size_t num_triangles = (grid_size_.x - 1) * (grid_size_.y - 1) * 2;
  std::vector<uint32_t> indices;
  num_indices_ = 3 * num_triangles;
  indices.reserve(num_indices_);

  for (int y = 0; y < (grid_size_.y - 1); y++)
  {
    for (int x = 0; x < (grid_size_.x - 1); x++)
    {
      // o----o
      // |   /
      // | /
      // o
      indices.push_back((x + 0) + (y + 0) * (grid_size_.x)); // upper left
      indices.push_back((x + 0) + (y + 1) * (grid_size_.x)); // bottom left
      indices.push_back((x + 1) + (y + 0) * (grid_size_.x)); // upper right
      //      o
      //    / |
      //  /   |
      // o----o
      indices.push_back((x + 0) + (y + 1) * (grid_size_.x)); // bottom left
      indices.push_back((x + 1) + (y + 1) * (grid_size_.x)); // bottom right
      indices.push_back((x + 1) + (y + 0) * (grid_size_.x)); // upper right
    }
  }

  // Determine VBO size
  size_t total_buffer_size = getPositionDataSize() + getNormalDataSize() + getColorDataSize();

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(total_buffer_size), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(uint32_t) * indices.size()), indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0); //Vertex positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), reinterpret_cast<void*> (NULL));
  glEnableVertexAttribArray(1); //Vertex normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float3), reinterpret_cast<void*> (getPositionDataSize()));
  glEnableVertexAttribArray(2); //Vertex colors
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float4), reinterpret_cast<void*> (getPositionDataSize() + getNormalDataSize()));
  glBindVertexArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  GL::checkError();
}

void Cloth::initGrid(const float3& pos_offset, int2 grid_size, const float2& cloth_size, float cloth_mass, const std::vector<std::pair<int, int>>& fixed_particles)
{
  next_ = 2, current_ = 1, prev_ = 0;
  total_grid_size_ = grid_size.x * grid_size.y;
  grid_size_ = grid_size;
  cloth_size_ = cloth_size;
  cloth_mass_ = cloth_mass;
  particle_mass_ = cloth_mass / total_grid_size_;

  for (auto& positions : p_buffers_)
  {
    positions.clear();
    positions.reserve(total_grid_size_);
  }

  normals_.clear();
  normals_.reserve(total_grid_size_);

  inv_masses_.clear();
  inv_masses_.resize(total_grid_size_);
  std::fill(inv_masses_.begin(), inv_masses_.end(), 1.f / particle_mass_);
  for (const auto& fixed : fixed_particles)
    inv_masses_[fixed.first + fixed.second * grid_size_.x] = 0.f;

  std::vector<float4> colors;
  colors.reserve(total_grid_size_);

  math::float2 inv_grid_size = { 1.f / grid_size_.x , 1.f / grid_size_.y };
  float xstep = cloth_size_.x / (grid_size_.x - 1);
  float zstep = cloth_size_.y / (grid_size_.y - 1);

  for (size_t y = 0; y < grid_size_.y; ++y)
    for (size_t x = 0; x < grid_size_.x; ++x)
    {
      float3 p, n;
      float4 c;

      p = pos_offset + float3(-0.5f * cloth_size_.x + x * xstep, 0.f, -0.5f * cloth_size_.y + y * zstep);
      n  = { 0.f, 1.f, 0.f};
      c = { x * inv_grid_size.x, y * inv_grid_size.y, 0.5f, 1.0f };

      p_buffers_[0].emplace_back(p);
      normals_.emplace_back(n);
      colors.emplace_back(c);
    }

  p_buffers_[2] = p_buffers_[1] = p_buffers_[0];

  setupBuffers();
  writePositionData(p_buffers_[current_]);
  writeNormalData(normals_);
  writeColorData(colors);

  GL::checkError();
}

void Cloth::render() const
{
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(num_indices_), GL_UNSIGNED_INT, NULL);
  GL::checkError();
}

void Cloth::doSimStep(const float3& gravity, const float3& wind, float damping, float fixup_percent, int fixup_iterations, const std::vector<Sphere>& obstacles, float dt, bool apply_structural_fixup, bool apply_shear_fixup, bool apply_flexion_fixup)
{
  simulationStep(p_buffers_, next_, current_, prev_, grid_size_.x, grid_size_.y,
    cloth_size_.x, cloth_size_.y, inv_masses_, normals_,
    gravity, wind, damping, fixup_percent, fixup_iterations, obstacles, dt, apply_structural_fixup, apply_shear_fixup, apply_flexion_fixup);
}

void Cloth::updateBuffers()
{
  writePositionData(p_buffers_[current_]);
  writeNormalData(normals_);
}

size_t Cloth::getPositionDataSize() const
{
  return total_grid_size_ * sizeof(float3);
}

size_t Cloth::getNormalDataSize() const
{
  return total_grid_size_ * sizeof(float3);
}

size_t Cloth::getColorDataSize() const
{
  return total_grid_size_ * sizeof(float4);
}

void Cloth::writePositionData(const std::vector<float3>& p)
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(getPositionDataSize()), p.data());
  GL::checkError();
}

void Cloth::writeNormalData(const std::vector<float3>& n)
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr> (getPositionDataSize()), static_cast<GLsizeiptr>(getNormalDataSize()), n.data());
  GL::checkError();
}

void Cloth::writeColorData(const std::vector<float4>& c)
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr> (getPositionDataSize() + getNormalDataSize()), static_cast<GLsizeiptr>(getColorDataSize()), c.data());
  GL::checkError();
}