


#include <utility>
#include <memory>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>

#include <framework/utils/memory.h>
#include <framework/bvh.h>
#include <framework/obj.h>
#include <framework/png.h>

#include "SceneBuilder.h"


namespace
{
	template <typename A, typename B>
	float timeElapsed(const A& start, const B& end)
	{
		return std::chrono::duration<float>(end - start).count();
	}

	class BVHBuilder
	{
		std::vector<std::unique_ptr<BVHInnerNode>> nodes;
		std::vector<std::unique_ptr<BVHLeafNode>> leafs;

		BoundingBox bb;
		BVHNode* root;

		template <typename TriangleIterator, typename TriangleSentinel>
		void unpackTriangles(triangle_t* triangles, int* material_ids, TriangleIterator triangles_begin, TriangleSentinel triangles_end)
		{
			for (auto it = triangles_begin; it != triangles_end; ++it)
			{
				*triangles++ = { (*it)[0], (*it)[1], (*it)[2] };
				*material_ids++ = (*it)[3];
			}
		}

	public:
		BVHNode* createNode(const BoundingBox& bb_left, BVHNode* left, const BoundingBox& bb_right, BVHNode* right)
		{
			return nodes.emplace_back(std::make_unique<BVHInnerNode>(bb_left, *left, bb_right, *right)).get();
		}

		template <typename TriangleIterator, typename TriangleSentinel>
		BVHNode* createLeaf(TriangleIterator triangles_begin, TriangleSentinel triangles_end)
		{
			auto num_triangles = static_cast<std::size_t>(triangles_end - triangles_begin);
			auto triangles = make_unique_default<triangle_t[]>(num_triangles);
			auto material_ids = make_unique_default<int[]>(num_triangles);

			unpackTriangles(triangles.get(), material_ids.get(), triangles_begin, triangles_end);

			return leafs.emplace_back(std::make_unique<BVHLeafNode>(std::move(triangles), num_triangles, std::move(material_ids))).get();
		}

		void finish(const BoundingBox& bb, BVHNode* root)
		{
			this->bb = bb;
			this->root = root;
		}

		BVHRoot finish()
		{
			return { bb, *root, std::move(nodes), std::move(leafs) };
		}
	};

	template <typename TriangleIterator, typename MaterialIterator>
	class TriangleMaterialIterator
	{
		TriangleIterator tri_it;
		MaterialIterator mat_it;

	public:
		using difference_type = typename std::iterator_traits<TriangleIterator>::difference_type;
		using value_type = std::array<int, 4>;
		using pointer = const value_type*;
		using reference = const value_type&;
		using iterator_category = std::forward_iterator_tag;

		TriangleMaterialIterator(TriangleIterator tri_it, MaterialIterator mat_it)
			: tri_it(tri_it), mat_it(mat_it)
		{
		}

		value_type operator *() const
		{
			return { (*tri_it)[0], (*tri_it)[1], (*tri_it)[2], *mat_it };
		}

		friend TriangleMaterialIterator& operator ++(TriangleMaterialIterator& me) noexcept(noexcept(++me.tri_it, ++me.mat_it))
		{
			++me.tri_it, ++me.mat_it;
			return me;
		}

		friend TriangleMaterialIterator operator ++(TriangleMaterialIterator me, int) noexcept(noexcept(++me))
		{
			return ++me;
		}

		friend TriangleMaterialIterator& operator +=(TriangleMaterialIterator& me, difference_type n) noexcept(noexcept(me.tri_it += n, me.mat_it += n))
		{
			me.tri_it += n, me.mat_it += n;
			return me;
		}

		friend TriangleMaterialIterator& operator --(TriangleMaterialIterator& me) noexcept(noexcept(--me.tri_it, --me.mat_it))
		{
			--me.tri_it, --me.mat_it;
			return me;
		}

		friend TriangleMaterialIterator operator --(TriangleMaterialIterator me, int) noexcept(noexcept(--me))
		{
			return --me;
		}

		friend TriangleMaterialIterator& operator -=(TriangleMaterialIterator& me, difference_type n) noexcept(noexcept(me.tri_it -= n, me.mat_it -= n))
		{
			me.tri_it -= n, me.mat_it -= n;
			return me;
		}

		friend TriangleMaterialIterator operator +(TriangleMaterialIterator a, difference_type n) noexcept(noexcept(a.it + n))
		{
			a.it = a.it + n;
			return a;
		}

		friend TriangleMaterialIterator operator +(difference_type n, TriangleMaterialIterator a) noexcept(noexcept(n + a.it))
		{
			a.it = n + a.it;
			return a;
		}

		friend TriangleMaterialIterator operator -(TriangleMaterialIterator a, difference_type n) noexcept(noexcept(a.it - n))
		{
			a.it = a.it - n;
			return a;
		}

		friend auto operator -(const TriangleMaterialIterator& a, const TriangleMaterialIterator& b) noexcept(noexcept(a.tri_it - b.tri_it))
		{
			return a.tri_it - b.tri_it;
		}

		friend auto operator -(const TriangleIterator& a, const TriangleMaterialIterator& b) noexcept(noexcept(a - b.tri_it))
		{
			return a - b.tri_it;
		}

		friend bool operator ==(const TriangleMaterialIterator& a, const TriangleMaterialIterator& b) noexcept(noexcept(a.tri_it == b.tri_it))
		{
			return a.tri_it == b.tri_it;
		}

		friend bool operator ==(const TriangleMaterialIterator& a, const TriangleIterator& b) noexcept(noexcept(a.tri_it == b))
		{
			return a.tri_it == b;
		}

		friend bool operator !=(const TriangleMaterialIterator& a, const TriangleMaterialIterator& b) noexcept(noexcept(!(a == b)))
		{
			return !(a == b);
		}

		friend bool operator !=(const TriangleMaterialIterator& a, const TriangleIterator& b) noexcept(noexcept(!(a == b)))
		{
			return !(a == b);
		}
	};

	template <typename T>
	auto extendBuffer(std::unique_ptr<T[]>& buffer, std::size_t buffer_size, std::size_t extension_size)
	{
		auto new_buffer = make_unique_default<T[]>(buffer_size + extension_size);
		auto extension_begin = std::copy(buffer.get(), buffer.get() + buffer_size, new_buffer.get());
		buffer = std::move(new_buffer);
		return extension_begin;
	}
}

SceneBuilder::SceneBuilder()
{
	addMaterial({ 0.8f, 0.8f, 0.8f }, -1, { 0.0f, 0.0f, 0.0f }, -1, 1.0f);
}

int SceneBuilder::addTexture(const std::filesystem::path& filename)
{
	*extendBuffer(textures, num_textures, 1) = PNG::loadImage2DR8G8B8A8(filename.u8string().c_str());
	return static_cast<int>(num_textures++);
}

int SceneBuilder::addMaterial(const float3& diffuse, int diffuse_map, const float3& specular, int specular_map, float shininess)
{
	*extendBuffer(materials, num_materials, 1) = { diffuse, diffuse_map, specular, specular_map, shininess };
	return static_cast<int>(num_materials++);
}

int SceneBuilder::addMaterial(const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess)
{
	int diffuse_map_id = !diffuse_map.empty() ? addTexture(diffuse_map) : -1;
	int specular_map_id = !specular_map.empty() ? addTexture(specular_map) : -1;
	return addMaterial(diffuse, diffuse_map_id, specular, specular_map_id, shininess);
}

int SceneBuilder::addMaterial(const std::string& name, float3& diffuse, int diffuse_map, const float3& specular, int specular_map, float shininess)
{
	return addMaterialID(name, addMaterial(diffuse, diffuse_map, specular, specular_map, shininess));
}

int SceneBuilder::addMaterial(const std::string& name, const float3& diffuse, const std::filesystem::path& diffuse_map, const float3& specular, const std::filesystem::path& specular_map, float shininess)
{
	return addMaterialID(name, addMaterial(diffuse, diffuse_map, specular, specular_map, shininess));
}

int SceneBuilder::addMaterialID(const std::string& name, int id)
{
	if (!name.empty())
		material_map[name] = id;
	return id;
}

int SceneBuilder::lookupMaterialID(const std::string& name) const
{
	if (name.empty())
		return 0;

	auto found = material_map.find(name);

	if (found != end(material_map))
		return found->second;

	throw std::runtime_error("material '" + name + "' not found");
}

void SceneBuilder::addPlane(const float4& p, const std::string& material)
{
	*extendBuffer(planes, num_planes++, 1) = { p, lookupMaterialID(material) };
}

void SceneBuilder::addCone(const float3& p, const float3& d, float r, float h, const std::string& material)
{
	*extendBuffer(cones, num_cones++, 1) = { p, d, r, h, lookupMaterialID(material) };
}

void SceneBuilder::addModel(const std::filesystem::path& filename, const float4x4& M, const std::string& material)
{
	auto time_start = std::chrono::steady_clock::now();
	auto [obj_positions, obj_normals, obj_texcoords, obj_triangles] = OBJ::readTriangles(filename);
	auto time_end = std::chrono::steady_clock::now();

	std::cout << "loaded " << filename.filename() << " in " << std::fixed << std::setprecision(3) << timeElapsed(time_start, time_end) << " s\n" << std::flush;

	std::transform(begin(obj_positions), end(obj_positions), extendBuffer(positions, num_vertices, size(obj_positions)), [&M](const float3& p) -> float3
	{
		auto v = M * float4 { p, 1.0f };
		return { v.x, v.y, v.z };
	});

	std::transform(begin(obj_normals), end(obj_normals), extendBuffer(normals, num_vertices, size(obj_positions)), [M = transpose(inverse(M))](const float3& p) -> float3
	{
		auto v = M * float4 { p, 0.0f };
		return { v.x, v.y, v.z };
	});

	std::copy(begin(obj_texcoords), end(obj_texcoords), extendBuffer(texcoords, num_vertices, size(obj_positions)));


	std::transform(begin(obj_triangles), end(obj_triangles), extendBuffer(triangles, num_triangles, size(obj_triangles)), [offset = static_cast<int>(num_vertices)](const triangle_t& t) -> triangle_t
	{
		return { t[0] + offset, t[1] + offset, t[2] + offset };
	});


	auto material_ids_begin = extendBuffer(material_ids, num_triangles, size(obj_triangles));
	std::fill(material_ids_begin, material_ids_begin + size(obj_triangles), lookupMaterialID(material));

	num_vertices += size(obj_positions);
	num_triangles += size(obj_triangles);
}

Scene SceneBuilder::finish()
{
	BVHBuilder builder;
	BVH::build(builder, TriangleMaterialIterator(&triangles[0], &material_ids[0]), &triangles[0] + num_triangles, &positions[0], 12, 42);
	auto bvh = builder.finish();
	return { std::move(positions), std::move(normals), std::move(texcoords), num_vertices, /*std::move(triangles), std::move(material_ids), num_triangles,*/ std::move(bvh), std::move(planes), num_planes, std::move(cones), num_cones, std::move(materials), num_materials, std::move(textures), num_textures };
}
