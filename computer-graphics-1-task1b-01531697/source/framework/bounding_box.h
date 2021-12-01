


#ifndef INCLUDED_FRAMEWORK_BOUNDING_BOX
#define INCLUDED_FRAMEWORK_BOUNDING_BOX

#pragma once

#include <array>

#include <framework/math/vector.h>


struct BoundingBox
{
	struct limits
	{
		float min, max;

		friend constexpr float extent(const limits& l)
		{
			return l.max - l.min;
		}
	};

	limits limit[3];
};

constexpr float2 intersect(const BoundingBox& bb, const float3& p, const float3& d)
{
	auto dirfrac = float3{ 1.0f / d.x, 1.0f / d.y, 1.0f / d.z };

	float t1 = (bb.limit[0].min - p.x) * dirfrac.x;
	float t2 = (bb.limit[0].max - p.x) * dirfrac.x;
	float t3 = (bb.limit[1].min - p.y) * dirfrac.y;
	float t4 = (bb.limit[1].max - p.y) * dirfrac.y;
	float t5 = (bb.limit[2].min - p.z) * dirfrac.z;
	float t6 = (bb.limit[2].max - p.z) * dirfrac.z;

	auto t_1 = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	auto t_2 = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	return { t_1, t_2 };
}

constexpr bool intersect(const BoundingBox& bb, const float3& p, const float3& d, float t_min)
{
	auto [t_1, t_2] = intersect(bb, p, d);

	if (t_1 > t_2)
		return false;

	if (t_2 < t_min)
		return false;

	return true;
}

constexpr bool intersect(const BoundingBox& bb, const float3& p, const float3& d, float t_min, float t_max)
{
	auto [t_1, t_2] = intersect(bb, p, d);

	if (t_1 > t_2)
		return false;

	if (t_1 > t_max || t_2 < t_min)
		return false;

	return true;
}

#endif  // INCLUDED_FRAMEWORK_BVH
