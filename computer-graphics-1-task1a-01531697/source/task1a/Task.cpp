


#include <chrono>
#include <iostream>
#include <iomanip>

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
}

Task::Task(const std::filesystem::path& config_file)
{
	load(*this, config_file);
}

void Task::defineCanvas(int width, int height, const float4& background_color)
{
	this->background_color = background_color;
	raytracer.emplace(width, height);
}

void Task::defineCamera(float w_s, float f)
{
	this->w_s = w_s;
	this->f = f;
}

void Task::definePlane(float a, float b, float c, float d)
{
	//max_distance = std::max(max_distance, 4.0f * std::abs(d) / length(float3 { a, b, c }));
	scene.addPlane(a, b, c, d);
}

void Task::defineSphere(const float3& c, float r)
{
	//max_distance = std::max(max_distance, length(c) + r);
	scene.addSphere(c, r);
}

void Task::defineBox(const float3& min, const float3& max)
{
	//auto d = max - min;
	//auto c = 0.5f * (max + min);
	//max_distance = std::max(max_distance, length(c) + 0.5f * length(d));
	scene.addBox(min, max);
}

void Task::finalize()
{
}

void Task::render(const std::filesystem::path& output_file)
{
	auto start = std::chrono::steady_clock::now();
	raytracer->render(scene, w_s, f, background_color);
	auto end = std::chrono::steady_clock::now();

	std::cout << "rendered in " << std::fixed << std::setprecision(3) << timeElapsed(start, end) << " s\n" << std::flush;

	auto img = raytracer->tonemap();
	PNG::saveImage(output_file.u8string().c_str(), img);
}

void Task::renderIntersectionTest(const std::filesystem::path& output_file)
{
	auto img = raytracer->renderIntersectionTest(scene);
	PFM::saveR32F(output_file.u8string().c_str(), img);
}
