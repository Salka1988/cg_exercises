


#ifndef INCLUDED_TASK1A
#define INCLUDED_TASK1A

#pragma once

#include <framework/math/vector.h>
#include <framework/image.h>


class Scene;

bool intersectRayPlane(const float3& start, const float3& dir, const float4& plane, float& t);
bool intersectRaySphere(const float3& start, const float3& dir, const float3& c, float r, float& t);
bool intersectRayBox(const float3& start, const float3& dir, const float3& min, const float3& max, float& t);

void render(image2D<float4>& framebuffer, const Scene& scene, float w_s, float f, const float4& background_color);


#endif  // INCLUDED_TASK1A
