


#include <framework/math/math.h>
#include <framework/utils/color.h>

#include "Scene.h"
#include "RayTracer.h"


RayTracer::RayTracer(int canvas_width, int canvas_height)
	: framebuffer(canvas_width, canvas_height)
{
}

void RayTracer::render(const Scene& scene, float w_s, float f, const float4& background_color)
{
	::render(framebuffer, scene, w_s, f, background_color);
}

image2D<float> RayTracer::renderIntersectionTest(const Scene& scene) const
{
	image2D<float> img(1024, 512);

	for (int y = 0; y < height(img); ++y)
	{
		for (int x = 0; x < width(img); ++x)
		{
			float phi = ((x + 0.5f) / width(img)) * 2.0f * math::constants<float>::pi;
			float theta = (0.5f - ((y + 0.5f) / height(img))) * math::constants<float>::pi;

			float ct = std::cos(theta);
			float st = std::sin(theta);
			float cp = std::cos(phi);
			float sp = std::sin(phi);

			float3 d { ct * -cp, st, ct * -sp };

			// test with 2 * d to make sure they actually return the ray parameter, not just the distance
			if (float t; scene.findClosestHit({ 0.0f, 0.0f, 0.0f }, 2.0f * d, t))
				img(x, y) = t;
			else
				img(x, y) = 0.0f;
		}
	}

	return img;
}

image2D<std::uint32_t> RayTracer::tonemap() const
{
	image2D<std::uint32_t> img(width(framebuffer), height(framebuffer));

	for (int y = 0; y < height(framebuffer); ++y)
		for (int x = 0; x < width(framebuffer); ++x)
			img(x, y) = gammaencodeR8G8B8A8(framebuffer(x, y));

	return img;
}
