

#ifndef INCLUDED_TASK1B_TASK
#define INCLUDED_TASK1B_TASK

#pragma once

#include <optional>
#include <variant>
#include <vector>
#include <filesystem>

#include "task_builder.h"
#include "SceneBuilder.h"
#include "Scene.h"
#include "RayTracer.h"

#include "task1b.h"


class Task : private virtual TaskBuilder
{
	std::variant<std::monostate, Scene, SceneBuilder> scene;

	int canvas_width;
	int canvas_height;
	float3 background_color;

	RayTracer raytracer;

	Camera camera = { 0.036f, 0.024f, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

	std::vector<Pointlight> lights;

	std::vector<std::string> dependencies;

	void defineCanvas(int width, int height, const float3& background_color) override;
	void definePlane(const float4& p, const std::string& material) override;
	void defineCone(const float3& p, const float3& d, float r, float h, const std::string& material) override;
	void defineModel(const std::filesystem::path& filename, const float4x4& M, const std::string& material) override;
	void defineMaterial(const std::string& name, const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess) override;
	void defineLight(const float3& p, const float3& color) override;
	void defineCamera(float w_s, float f, const float3& eye, const float3& lookat, const float3& up) override;

	bool readCachedTask(const std::filesystem::path& filename);
	void writeCachedTask(const std::filesystem::path& filename) const;
	void addDependency(const std::filesystem::path& filename);

public:
	Task(const std::filesystem::path& output_dir, const std::filesystem::path& config_file, bool no_cache = false);

	void render(const std::filesystem::path& output_file, int res_x, int res_y, int max_bounces, int num_threads = 1);
	void renderIntersectionTest(const std::filesystem::path& output_file, int res_x, int res_y, int num_threads = 1);
	void renderShadingTest(const std::filesystem::path& output_file, int res_x, int res_y, int num_threads = 1);
};

#endif  // INCLUDED_TASK1B_TASK
