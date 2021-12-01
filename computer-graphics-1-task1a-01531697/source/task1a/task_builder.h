

#ifndef INCLUDED_TASK_BUILDER
#define INCLUDED_TASK_BUILDER

#pragma once

#include <filesystem>

#include <framework/math/vector.h>


struct TaskBuilder
{
	virtual void defineCanvas(int width, int height, const float4& background_color) = 0;
	virtual void definePlane(float a, float b, float c, float d) = 0;
	virtual void defineSphere(const float3& c, float r) = 0;
	virtual void defineBox(const float3& min, const float3& max) = 0;
	virtual void defineCamera(float w_s, float f) = 0;
	virtual void finalize() = 0;

protected:
	TaskBuilder() = default;
	TaskBuilder(TaskBuilder&&) = default;
	TaskBuilder(const TaskBuilder&) = default;
	TaskBuilder& operator =(TaskBuilder&&) = default;
	TaskBuilder& operator =(const TaskBuilder&) = default;
	~TaskBuilder() = default;
};


void load(TaskBuilder& builder, const std::filesystem::path& config_file);

#endif  // INCLUDED_TASK_BUILDER
