


#include <limits>

#include "Scene.h"


void Scene::addPlane(float a, float b, float c, float d)
{
	planes.emplace_back(a, b, c, d);
}

void Scene::addSphere(const float3& c, float r)
{
	spheres.push_back({ c, r });
}

void Scene::addBox(const float3& min, const float3& max)
{
	boxes.push_back({ min, max });
}

bool Scene::findClosestHit(const float3& p, const float3& d, float& t) const
{
	float t_min = std::numeric_limits<float>::max();

	for (const auto& plane : planes)
		if (float t; intersectRayPlane(p, d, plane, t))
			t_min = std::min(t_min, t);

	for (const auto& sphere : spheres)
		if (float t; intersectRaySphere(p, d, sphere.c, sphere.r, t))
			t_min = std::min(t_min, t);

	for (const auto& box : boxes)
		if (float t; intersectRayBox(p, d, box.min, box.max, t))
			t_min = std::min(t_min, t);

	t = t_min;

	return t_min != std::numeric_limits<float>::max();
}
