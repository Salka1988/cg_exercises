


#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
#include <iomanip>

#include <framework/math/math.h>
#include <framework/utils/color.h>
#include <framework/utils/thread_pool.h>
#include <framework/utils/io.h>

#include "Scene.h"
#include "RayTracer.h"


namespace
{
	template <typename Image, typename F>
	void render360(Image& img, F&& f, int num_threads, const float3& eye = { 0.0f, 0.0f, 0.0f })
	{
		launchTiled([&img, &f, &eye](int left, int top, int right, int bottom, int n, int N, int thread_id)
		{
			for (int y = top; y < bottom; ++y)
			{
				for (int x = left; x < right; ++x)
				{
					float phi = ((x + 0.5f) / width(img)) * 2.0f * math::constants<float>::pi;
					float theta = (0.5f - ((y + 0.5f) / height(img))) * math::constants<float>::pi;

					float ct = std::cos(theta);
					float st = std::sin(theta);
					float cp = std::cos(phi);
					float sp = std::sin(phi);

					float3 d { ct * -cp, st, ct * -sp };

					img(x, y) = f(eye, d);

					if (thread_id == 0)
						std::cout << '\r' << std::fixed << std::setprecision(0) << n * 100.0f / N << '%' << std::flush;
				}
			}
		}, num_threads, static_cast<int>(width(img)), static_cast<int>(height(img)), 64, 64);
		std::cout << "\r...done" << std::endl;
	}
}

image2D<float3> RayTracer::render(int res_x, int res_y, const Scene& scene, const Camera& camera, const Pointlight* lights, std::size_t num_lights, const float3& background_color, int max_bounces, int num_threads)
{
	image2D<float3> framebuffer(res_x, res_y);

	std::cout << "rendering..." << std::endl;

	launchTiled([&framebuffer, &scene, &camera, lights, num_lights, &background_color, max_bounces](int left, int top, int right, int bottom, int n, int N, int thread_id)
	{
		::render(framebuffer, left, top, right, bottom, scene, camera, lights, num_lights, background_color, max_bounces);

		if (thread_id == 0)
			std::cout << '\r' << std::fixed << std::setprecision(0) << n * 100.0f / N << '%' << std::flush;
	}, num_threads, static_cast<int>(width(framebuffer)), static_cast<int>(height(framebuffer)), 64, 64);
	std::cout << "\r...done" << std::endl;

	return framebuffer;
}

image2D<RGB32F> RayTracer::renderIntersectionTest(int res_x, int res_y, const Scene& scene, const Camera& camera, int num_threads) const
{
	image2D<RGB32F> img(res_x, res_y);

	render360(img, [&scene](const float3& p, const float3& d) -> RGB32F
	{
		// test with 2 * d to make sure they actually return the ray parameter, not just the distance
		auto c = scene.visualizeIntersections(p, 2.0f * d);
		return { c.x, c.y, c.z };
	}, num_threads, camera.eye);

	return img;
}

image2D<RGB32F> RayTracer::renderShadingTest(int res_x, int res_y, const Scene& scene, const Camera& camera, const Pointlight* lights, std::size_t num_lights, int num_threads) const
{
	image2D<RGB32F> img(res_x, res_y);

	render360(img, [&scene, lights, num_lights](const float3& p, const float3& d) -> RGB32F
	{
		if (auto hit = scene.findClosestHit(p, d))
		{
			auto c = shade(p, d, *hit, scene, lights, num_lights);
			return { c.x, c.y, c.z };
		}
		return { 0.0f, 0.0f, 0.0f };
	}, num_threads, camera.eye);

	return img;
}

image2D<std::uint32_t> RayTracer::tonemap(const image2D<float3>& framebuffer) const
{
	image2D<std::uint32_t> img(width(framebuffer), height(framebuffer));

	for (int y = 0; y < height(framebuffer); ++y)
		for (int x = 0; x < width(framebuffer); ++x)
			img(x, y) = gammaencodeR8G8B8(framebuffer(x, y));

	return img;
}
