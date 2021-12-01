

#ifndef INCLUDED_TASK
#define INCLUDED_TASK

#pragma once

#include <optional>

#include "task_builder.h"
#include "Scene.h"
#include "RayTracer.h"


class Task : private virtual TaskBuilder
{
	float w_s = 0.036f;
	float f = 0.024f;

	//float max_distance = 0.0f;

	Scene scene;

	float4 background_color;

	std::optional<RayTracer> raytracer;

	void defineCanvas(int width, int height, const float4& background_color) override;
	void definePlane(float a, float b, float c, float d) override;
	void defineSphere(const float3& c, float r) override;
	void defineBox(const float3& min, const float3& max) override;
	void defineCamera(float w_s, float f) override;
	void finalize() override;

public:
	Task(const std::filesystem::path& config_file);

	void render(const std::filesystem::path& output_file);
	void renderIntersectionTest(const std::filesystem::path& output_file);
};


#endif  // INCLUDED_TASK
