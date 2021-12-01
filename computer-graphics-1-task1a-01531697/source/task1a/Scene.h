


#ifndef INCLUDED_SCENE
#define INCLUDED_SCENE

#pragma once

#include <vector>

#include "task1a.h"


struct Sphere
{
	float3 c;
	float r;
};

struct Cube
{
	float3 min;
	float3 max;
};

class Scene
{
	Scene(const Scene&) = default;
	Scene& operator =(const Scene&) = default;

	std::vector <float4> planes;
	std::vector<Sphere> spheres;
	std::vector<Cube> boxes;

public:
	Scene() = default;

	void addPlane(float a, float b, float c, float d);
	void addSphere(const float3& c, float r);
	void addBox(const float3& min, const float3& max);

	bool findClosestHit(const float3& p, const float3& d, float& t) const;
};

#endif  // INCLUDED_SCENE
