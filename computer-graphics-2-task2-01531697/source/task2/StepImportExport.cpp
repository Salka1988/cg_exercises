#include "StepImportExport.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

StepImport::StepImport(const std::string& path)
{
  ifs_.open(path, std::fstream::binary);
  if (!ifs_.is_open())
    throw std::runtime_error("error opening stepdata file");

  inv_masses_ = readArray<float>();
}

bool StepImport::readNextStep(Step& step)
{
  if (ifs_.peek() == EOF)
    return false;

  step.num = readValue<uint64_t>();
  step.position_current = readArray<float3>();
  step.position_previous = readArray<float3>();
  step.position_verlet = readArray<float3>();
  step.normals = readArray<float3>();
  return true;
}

bool StepExport::begin(uint64_t current_step)
{
  if (export_mod_ == 0 || current_step % export_mod_ != 0)
    return false;

  if (!ofs_.is_open())
  {
    auto path = fs::path(output_path_) / "stepdata";
    ofs_.open(path, std::fstream::binary);
    if (!ofs_.is_open())
      throw std::runtime_error("error opening data export file");
  }

  ofs_.flush();
  return true;
}

void StepExport::end()
{
  ofs_.flush();
}

void StepExport::write(const void* data, uint64_t length)
{
  ofs_.write(reinterpret_cast<const char*>(data), length);
}
