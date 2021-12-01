


#include <limits>
#include <stdexcept>
#include <memory>
#include <string_view>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <framework/utils/memory.h>
#include <framework/utils/io.h>

#include <framework/png.h>
#include <framework/pfm.h>

#include "Task.h"


namespace
{
	template <typename A, typename B>
	float timeElapsed(const A& start, const B& end)
	{
		return std::chrono::duration<float>(end - start).count();
	}

	std::filesystem::path constructCacheFileName(const std::filesystem::path& output_dir, std::filesystem::path config_file)
	{
		return output_dir/config_file.replace_extension(".cached").filename();
	}

	constexpr char CACHE_FILE_MAGIC[] = { 'C', 'G', '1', 'b' };
}

Task::Task(const std::filesystem::path& output_dir, const std::filesystem::path& config_file, bool no_cache)
{
	auto cache_file_name = constructCacheFileName(output_dir, config_file);

	if (no_cache || !readCachedTask(cache_file_name))
	{
		// record current timestamp to make sure modifications to dependencies while the scene is being built are not missed
		auto time_begin = std::filesystem::file_time_type::clock::now();

		scene.emplace<SceneBuilder>();
		load(*this, config_file);
		addDependency(config_file);
		scene.emplace<Scene>(std::get<SceneBuilder>(scene).finish());

		if (!no_cache)
		{
			writeCachedTask(cache_file_name);
			std::filesystem::last_write_time(cache_file_name, time_begin);
		}
	}

	//{
	//	std::ofstream file(output_dir/(config_file.filename().replace_extension(".bvh.obj")), std::ios::binary);
	//	std::get<Scene>(scene).visualizeBVH(file);
	//}
}

void Task::writeCachedTask(const std::filesystem::path& filename) const
{
	std::ofstream file(filename, std::ios::binary);

	if (!file)
		throw std::runtime_error("error opening task cache file");

	if (size(dependencies) > std::numeric_limits<std::uint16_t>::max())
		throw std::runtime_error("too many dependencies");

	write(file, CACHE_FILE_MAGIC);

	write(file, static_cast<std::uint16_t>(size(dependencies)));
	for (const auto& d : dependencies)
	{
		if (size(d) > std::numeric_limits<std::uint16_t>::max())
			throw std::runtime_error("dependency path too long");
		write(file, static_cast<std::uint16_t>(size(d)));
		write(file, data(d), size(d));
	}

	std::get<Scene>(scene).serialize(file);

	write(file, static_cast<std::int16_t>(canvas_width));
	write(file, static_cast<std::int16_t>(canvas_height));
	write(file, background_color.x); write(file, background_color.y); write(file, background_color.z);

	write(file, camera.w_s);
	write(file, camera.f);
	write(file, camera.eye.x); write(file, camera.eye.y); write(file, camera.eye.z);
	write(file, camera.lookat.x); write(file, camera.lookat.y); write(file, camera.lookat.z);
	write(file, camera.up.x); write(file, camera.up.y); write(file, camera.up.z);

	if (size(lights) > std::numeric_limits<std::uint16_t>::max())
		throw std::runtime_error("too many lights");
	write(file, static_cast<std::uint16_t>(size(lights)));
	for (const auto& l : lights)
	{
		write(file, l.position.x); write(file, l.position.y); write(file, l.position.z);
		write(file, l.color.x); write(file, l.color.y); write(file, l.color.z);
	}
}

bool Task::readCachedTask(const std::filesystem::path& scene_cache_file_name)
{
	if (!std::filesystem::exists(scene_cache_file_name))
		return false;

	std::ifstream file(scene_cache_file_name, std::ios::binary);

	if (!file)
	{
		std::cout << "WARNING: failed to open " << scene_cache_file_name << std::endl;
		return false;
	}

	{
		char buffer[sizeof(CACHE_FILE_MAGIC)];
		read(buffer, file);

		if (std::string_view { buffer, sizeof(buffer) } != std::string_view { CACHE_FILE_MAGIC, sizeof(CACHE_FILE_MAGIC) })
		{
			std::cout << "WARNING: " << scene_cache_file_name << " is not a valid cache file" << std::endl;
			return false;
		}
	}

	auto num_dependencies = read<std::uint16_t>(file);
	for (unsigned int i = 0; i < num_dependencies; ++i)
	{
		auto len = read<std::uint16_t>(file);
		auto buffer = make_unique_default<char[]>(len);

		read(buffer.get(), file, len);

		auto dependency_path = std::filesystem::path { buffer.get(), buffer.get() + len };

		if (!std::filesystem::exists(dependency_path) || std::filesystem::last_write_time(scene_cache_file_name) <= std::filesystem::last_write_time(dependency_path))
		{
			std::cout << scene_cache_file_name << " out of date. rebuilding..." << std::endl;
			return false;
		}
	}

	std::cout << "using cached version of task found in " << scene_cache_file_name << std::endl;

	scene.emplace<Scene>(file);

	canvas_width = read<std::int16_t>(file);
	canvas_height = read<std::int16_t>(file);
	read(background_color.x, file); read(background_color.y, file); read(background_color.z, file);

	read(camera.w_s, file);
	read(camera.f, file);
	read(camera.eye.x, file); read(camera.eye.y, file); read(camera.eye.z, file);
	read(camera.lookat.x, file); read(camera.lookat.y, file); read(camera.lookat.z, file);
	read(camera.up.x, file); read(camera.up.y, file); read(camera.up.z, file);

	auto num_lights = read<std::uint16_t>(file);
	lights.reserve(num_lights);
	for (unsigned int i = 0; i < num_lights; ++i)
		lights.push_back({ { read<float>(file), read<float>(file), read<float>(file) }, { read<float>(file), read<float>(file), read<float>(file) } });

	if (!file)
		throw std::runtime_error("failed to read cached task file");

	return true;
}

void Task::addDependency(const std::filesystem::path& filename)
{
	dependencies.emplace_back(std::filesystem::canonical(filename).u8string());
}

void Task::defineCanvas(int width, int height, const float3& background_color)
{
	this->canvas_width = width;
	this->canvas_height = height;
	this->background_color = background_color;
}

void Task::defineCamera(float w_s, float f, const float3& eye, const float3& lookat, const float3& up)
{
	camera = { w_s, f, eye, lookat, up };
}

void Task::definePlane(const float4& p, const std::string& material)
{
	std::get<SceneBuilder>(scene).addPlane(p, material);
}

void Task::defineCone(const float3& p, const float3& d, float r, float h, const std::string& material)
{
	std::get<SceneBuilder>(scene).addCone(p, d, r, h, material);
}

void Task::defineModel(const std::filesystem::path& filename, const float4x4& M, const std::string& material)
{
	addDependency(filename);

	std::get<SceneBuilder>(scene).addModel(filename, M, material);
}

void Task::defineMaterial(const std::string& name, const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess)
{
	if (!diffuse_map.empty())
		addDependency(diffuse_map);

	if (!specular_map.empty())
		addDependency(specular_map);

	std::get<SceneBuilder>(scene).addMaterial(name, diffuse, diffuse_map, specular, specular_map, shininess);
}

void Task::defineLight(const float3& p, const float3& color)
{
	lights.push_back({ p, color });
}

void Task::render(const std::filesystem::path& output_file, int res_x, int res_y, int max_bounces, int num_threads)
{
	auto start = std::chrono::steady_clock::now();
	auto framebuffer = raytracer.render(res_x > 0 ? res_x : canvas_width, res_y > 0 ? res_y : canvas_height, std::get<Scene>(scene), camera, data(lights), size(lights), background_color, max_bounces, num_threads);
	auto end = std::chrono::steady_clock::now();

	std::cout << "rendered in " << std::fixed << std::setprecision(3) << timeElapsed(start, end) << " s using " << num_threads << " threads\n" << std::flush;

	auto img = raytracer.tonemap(framebuffer);
	PNG::saveImageR8G8B8(output_file.u8string().c_str(), img);
}

void Task::renderIntersectionTest(const std::filesystem::path& output_file, int res_x, int res_y, int num_threads)
{
	auto img = raytracer.renderIntersectionTest(res_x > 0 ? res_x : 1024, res_y > 0 ? res_y : 512, std::get<Scene>(scene), camera, num_threads);
	PFM::saveRGB32F(output_file.u8string().c_str(), img);
}

void Task::renderShadingTest(const std::filesystem::path& output_file, int res_x, int res_y, int num_threads)
{
	auto img = raytracer.renderShadingTest(res_x > 0 ? res_x : 1024, res_y > 0 ? res_y : 512, std::get<Scene>(scene), camera, data(lights), size(lights), num_threads);
	PFM::saveRGB32F(output_file.u8string().c_str(), img);
}
