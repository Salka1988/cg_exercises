#pragma once

#include "math/vector.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

struct Step
{
  uint64_t num;
  std::vector<float3> position_current;
  std::vector<float3> position_previous;
  std::vector<float3> normals;
  std::vector<float3> position_verlet;
};


class StepImport
{
public:
  StepImport(const std::string& path);
  bool readNextStep(Step& step);
  const std::vector<float>& getInvMasses() const { return inv_masses_; }

private:
  std::ifstream ifs_;
  std::string path_;
  std::vector<float> inv_masses_;

  template<typename T>
  T readValue()
  {
    T value;
    ifs_.read(reinterpret_cast<char*>(&value), sizeof(T));
    return value;
  }

  template<typename T>
  std::vector<T> readArray()
  {
    uint64_t size_in_bytes;
    std::vector<T> data;

    ifs_.read(reinterpret_cast<char*>(&size_in_bytes), sizeof(uint64_t));

    if (size_in_bytes % sizeof(T) != 0)
      throw std::runtime_error("error data size does not match");

    data.resize(size_in_bytes / sizeof(T));
    ifs_.read(reinterpret_cast<char*>(data.data()), size_in_bytes);
    return data;
  }
};


class StepExport
{
public:
  static StepExport& getInstance()
  {
    static StepExport instance;
    return instance;
  }

  void setOutputPath(const std::string& output_path) { output_path_ = output_path; }
  void setExportModulo(uint32_t modulo) { export_mod_ = modulo; }

  bool begin(uint64_t current_step);
  void end();

  template<typename T>
  void writeValue(T value)
  {
    write(&value, sizeof(T));
  }

  template<typename T>
  void writeArray(const std::vector<T>& data)
  {
    uint64_t size_in_bytes = data.size() * sizeof(T);
    write(&size_in_bytes, sizeof(uint64_t));
    write(data.data(), size_in_bytes);
  }

private:
  std::ofstream ofs_;
  std::string output_path_;
  uint32_t export_mod_ = 0;

  void write(const void* data, uint64_t length);
};
