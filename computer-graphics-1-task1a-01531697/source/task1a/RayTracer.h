


#ifndef INCLUDED_RENDERER
#define INCLUDED_RENDERER

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "task1a.h"


class RayTracer
{
	RayTracer(const RayTracer&) = default;
	RayTracer& operator =(const RayTracer&) = default;

	image2D<float4> framebuffer;

public:
	RayTracer(int canvas_width, int canvas_height);

	void render(const Scene& scene, float w_s, float f, const float4& background_color);
	image2D<float> renderIntersectionTest(const Scene& scene) const;

	image2D<std::uint32_t> tonemap() const;
};

#endif  // INCLUDED_RENDERER
