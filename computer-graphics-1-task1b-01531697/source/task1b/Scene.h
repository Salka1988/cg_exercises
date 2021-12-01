


#ifndef INCLUDED_TASK1B_SCENE
#define INCLUDED_TASK1B_SCENE

#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include <iosfwd>

#include <framework/math/vector.h>
#include <framework/image.h>

#include "task1b.h"

#include "BVH.h"


struct Plane
{
	float4 p;
	int material;
};

struct Cone
{
	float3 p;
	float3 d;
	float r;
	float h;
	int material;
};

struct Material
{
	float3 diffuse;
	int diffuse_map;
	float3 specular;
	int specular_map;
	float shininess;
};

class Scene
{
	std::unique_ptr<float3[]> positions;
	std::unique_ptr<float3[]> normals;
	std::unique_ptr<float2[]> texcoords;
	std::size_t num_vertices;

	//std::unique_ptr<triangle_t[]> triangles;
	//std::unique_ptr<int[]> material_ids;
	//std::size_t num_triangles;

	std::optional<BVHRoot> bvh;

	std::unique_ptr<Plane[]> planes;
	std::size_t num_planes;

	std::unique_ptr<Cone[]> cones;
	std::size_t num_cones;

	std::unique_ptr<Material[]> materials;
	std::size_t num_materials;

	std::unique_ptr<image2D<std::uint32_t>[]> textures;
	std::size_t num_textures;

	const Plane* findClosestHitPlane(const float3& p, const float3& d, float& t) const;
	bool intersectsPlane(const float3& p, const float3& d, float t_min, float t_max) const;
	const Cone* findClosestHitCone(const float3& p, const float3& d, float& t) const;
	bool intersectsCone(const float3& p, const float3& d, float t_min, float t_max) const;
	std::tuple<const triangle_t*, int> findClosestHitTriangle(const float3& p, const float3& d, float& t, float& lambda_1, float& lambda_2) const;
	HitPoint hit(const Plane& plane, const float3& p, const float3& d, float t) const;
	HitPoint hit(const Cone& cone, const float3& p, const float3& d, float t) const;
	HitPoint hit(const triangle_t& tri, int material_id, float lambda_1, float lambda_2) const;

	void deserialize(std::istream& file);

public:
	Scene(std::unique_ptr<float3[]>&& positions, std::unique_ptr<float3[]>&& normals, std::unique_ptr<float2[]>&& texcoords, std::size_t num_vertices, /*std::unique_ptr<triangle_t[]>&& triangles, std::unique_ptr<int[]>&& material_ids, std::size_t num_triangles,*/ BVHRoot&& bvh, std::unique_ptr<Plane[]> planes, std::size_t num_planes, std::unique_ptr<Cone[]>&& cones, std::size_t num_cones, std::unique_ptr<Material[]>&& materials, std::size_t num_materials, std::unique_ptr<image2D<std::uint32_t>[]>&& textures, std::size_t num_textures);
	Scene(std::istream& file);

	std::optional<HitPoint> findClosestHit(const float3& p, const float3& d) const;
	bool intersectsRay(const float3& p, const float3& d, float t_min, float t_max) const;

	float3 visualizeIntersections(const float3& p, const float3& d) const;

	std::ostream& serialize(std::ostream& file) const;
	std::ostream& visualizeBVH(std::ostream& file) const;
};

#endif  // INCLUDED_TASK1B_SCENE
