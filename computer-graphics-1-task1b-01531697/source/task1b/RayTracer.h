


#ifndef INCLUDED_TASK1B_RAYTRACER
#define INCLUDED_TASK1B_RAYTRACER

#pragma once

#include <cstdint>

#include <framework/image.h>
#include <framework/rgb32f.h>

#include "task1b.h"


class RayTracer
{
public:
	image2D<float3> render(int res_x, int res_y, const Scene& scene, const Camera& camera, const Pointlight* lights, std::size_t num_lights, const float3& background_color, int max_bounces, int num_threads);
	image2D<RGB32F> renderIntersectionTest(int res_x, int res_y, const Scene& scene, const Camera& camera, int num_threads) const;
	image2D<RGB32F> renderShadingTest(int res_x, int res_y, const Scene& scene, const Camera& camera, const Pointlight* lights, std::size_t num_lights, int num_threads) const;

	image2D<std::uint32_t> tonemap(const image2D<float3>& framebuffer) const;
};

#endif  // INCLUDED_TASK1B_RAYTRACER
