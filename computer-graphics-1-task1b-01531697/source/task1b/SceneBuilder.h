


#ifndef INCLUDED_TASK1B_SCENEBUILDER
#define INCLUDED_TASK1B_SCENEBUILDER

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

#include <framework/math/vector.h>
#include <framework/math/matrix.h>

#include "Scene.h"


class SceneBuilder
{
	std::unique_ptr<float3[]> positions;
	std::unique_ptr<float3[]> normals;
	std::unique_ptr<float2[]> texcoords;
	std::size_t num_vertices = 0;

	std::unique_ptr<triangle_t[]> triangles;
	std::unique_ptr<int[]> material_ids;
	std::size_t num_triangles = 0;

	std::unique_ptr<Plane[]> planes;
	std::size_t num_planes = 0;

	std::unique_ptr<Cone[]> cones;
	std::size_t num_cones = 0;

	std::unique_ptr<Material[]> materials;
	std::size_t num_materials = 0;

	std::unique_ptr<image2D<std::uint32_t>[]> textures;
	std::size_t num_textures = 0;

	std::unordered_map<std::string, int> material_map;

	int addMaterialID(const std::string& name, int id);
	int lookupMaterialID(const std::string& name) const;

public:
	SceneBuilder();

	int addTexture(const std::filesystem::path& filename);
	int addMaterial(const float3& diffuse, int diffuse_map, const float3& specular, int specular_map, float shininess);
	int addMaterial(const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess);
	int addMaterial(const std::string& name, float3& diffuse, int diffuse_map, const float3& specular, int specular_map, float shininess);
	int addMaterial(const std::string& name, const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess);

	void addPlane(const float4& p, const std::string& material);
	void addCone(const float3& p, const float3& d, float r, float h, const std::string& material);
	void addModel(const std::filesystem::path& filename, const float4x4& M, const std::string& material);

	Scene finish();
};

#endif  // INCLUDED_TASK1B_SCENEBUILDER
