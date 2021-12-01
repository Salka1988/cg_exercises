

#ifndef INCLUDED_TASK1B_TASK_BUILDER
#define INCLUDED_TASK1B_TASK_BUILDER

#pragma once

#include <string>
#include <filesystem>

#include <framework/math/vector.h>
#include <framework/math/matrix.h>


struct TaskBuilder
{
	virtual void defineCanvas(int width, int height, const float3& background_color) = 0;
	virtual void definePlane(const float4& p, const std::string& material) = 0;
	virtual void defineCone(const float3& p, const float3& d, float r, float h, const std::string& material) = 0;
	virtual void defineModel(const std::filesystem::path& filename, const float4x4& M, const std::string& material) = 0;
	virtual void defineMaterial(const std::string& name, const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess) = 0;
	virtual void defineLight(const float3& p, const float3& color) = 0;
	virtual void defineCamera(float w_s, float f, const float3& eye, const float3& lookat, const float3& up) = 0;

protected:
	TaskBuilder() = default;
	TaskBuilder(TaskBuilder&&) = default;
	TaskBuilder(const TaskBuilder&) = default;
	TaskBuilder& operator =(TaskBuilder&&) = default;
	TaskBuilder& operator =(const TaskBuilder&) = default;
	~TaskBuilder() = default;
};


void load(TaskBuilder& builder, const std::filesystem::path& config_file);

#endif  // INCLUDED_TASK1B_TASK_BUILDER
