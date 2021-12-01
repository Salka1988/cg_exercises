


#include <limits>
#include <utility>
#include <iostream>

#include <framework/utils/memory.h>
#include <framework/utils/color.h>
#include <framework/utils/io.h>
#include <framework/png.h>

#include "Scene.h"


namespace
{
	bool intersectRayPlane(const float3& p, const float3& d, const float4& plane, float& t)
	{
		float3 n = { plane.x, plane.y , plane.z };
		float dn = dot(d, n);

		if (dn == 0.0f)
			return false;

		t = (plane.w - dot(p, n)) / dn;

		if (t < 0.0f)
			return false;

		return true;
	}

	bool intersectRayCone(const float3& p, const float3& d, const float3& C, const float3& V, float r, float h, float& t)
	{
		float3 V_ = normalize(V);
		float3 d_ = normalize(d);
		float3 Cp = p - C;

		float cos_phi = h / std::sqrt(h*h + r*r);
		float cos_phi2 = cos_phi * cos_phi;
		float dot_dV = dot(d_, V_);
		float dot_CpV = dot(Cp, V_);

		float a = dot_dV * dot_dV - cos_phi2;
		float b = 2.0f * (dot_dV * dot_CpV - dot(d_, Cp) * cos_phi2);
		float c = dot_CpV * dot_CpV - dot(Cp, Cp) * cos_phi2;

		float det = b * b - 4 * a * c;

		float closest_t = std::numeric_limits<float>::max();
		if (det == 0.0f)
		{
			float t_ = -b / (2.0f * a);
			float on_axis = dot(p + t_ * d_ - C, V_);
			if (on_axis > 0 && on_axis < h && t_ > 0)
			{
				closest_t = t_;
			}
		}
		else if (det > 0.0f)
		{
			float t_1 = (-b + std::sqrt(det)) / (2.0f * a);
			float t_2 = (-b - std::sqrt(det)) / (2.0f * a);
			float on_axis1 = dot(p + t_1 * d_ - C, V_);
			float on_axis2 = dot(p + t_2 * d_ - C, V_);
			bool t_1_valid = on_axis1 > 0.0f && on_axis1 < h && t_1 > 0;
			bool t_2_valid = on_axis2 > 0.0f && on_axis2 < h && t_2 > 0;

			if (t_1_valid && !t_2_valid)
			{
				closest_t = t_1;
			}
			else if (t_2_valid && !t_1_valid)
			{
				closest_t = t_2;
			}
			else 
			if(t_1_valid && t_2_valid)
			{
				closest_t = std::min(t_1, t_2);
			}
		}

		float3 p0 = C + h * V_;
		float4 plane(V_.x, V_.y, V_.z, dot(V_,p0));
		float t_;
		if (intersectRayPlane(p, d_, plane, t_))
		{
			float3 on_plane = p + d_ * t_;
			float3 v = on_plane - p0;
			float d2 = dot(v, v);
			if (d2 <= r*r)
			{
				closest_t = std::min(t_, closest_t);
			}
		}

		if (closest_t != std::numeric_limits<float>::max())
		{
			t = closest_t / length(d);
			return true;
		}

		return false;
	}

	template <typename T>
	std::unique_ptr<T[]> readBunch(std::istream& file, std::size_t size)
	{
		auto data = make_unique_default<T[]>(size);
		read(data.get(), file, size);
		return data;
	}
}

Scene::Scene(std::unique_ptr<float3[]>&& positions, std::unique_ptr<float3[]>&& normals, std::unique_ptr<float2[]>&& texcoords, std::size_t num_vertices, /*std::unique_ptr<triangle_t[]>&& triangles, std::unique_ptr<int[]>&& material_ids, std::size_t num_triangles,*/ BVHRoot&& bvh, std::unique_ptr<Plane[]> planes, std::size_t num_planes, std::unique_ptr<Cone[]>&& cones, std::size_t num_cones, std::unique_ptr<Material[]>&& materials, std::size_t num_materials, std::unique_ptr<image2D<std::uint32_t>[]>&& textures, std::size_t num_textures)
	: positions(std::move(positions)),
	  normals(std::move(normals)),
	  texcoords(std::move(texcoords)),
	  num_vertices(num_vertices),
	  //triangles(std::move(triangles)),
	  //material_ids(std::move(material_ids)),
	  //num_triangles(num_triangles),
	  bvh(std::move(bvh)),
	  planes(std::move(planes)),
	  num_planes(num_planes),
	  cones(std::move(cones)),
	  num_cones(num_cones),
	  materials(std::move(materials)),
	  num_materials(num_materials),
	  textures(std::move(textures)),
	  num_textures(num_textures)
{
}

Scene::Scene(std::istream& file)
{
	deserialize(file);
}

void Scene::deserialize(std::istream& file)
{
	num_vertices = read<std::uint32_t>(file);
	if (num_vertices)
	{
		positions = readBunch<float3>(file, num_vertices);
		normals = readBunch<float3>(file, num_vertices);
		texcoords = readBunch<float2>(file, num_vertices);
	}

	//num_triangles = read<std::uint32_t>(file);
	//triangles = readBunch<triangle_t>(file, num_triangles);
	//material_ids = readBunch<int>(file, num_triangles);

	bvh.emplace(deserializeBVH(file));

	num_planes = read<std::uint32_t>(file);
	if (num_planes)
		planes = readBunch<Plane>(file, num_planes);

	num_cones = read<std::uint32_t>(file);
	if (num_cones)
		cones = readBunch<Cone>(file, num_cones);

	num_materials = read<std::uint32_t>(file);
	if (num_materials)
		materials = readBunch<Material>(file, num_materials);

	num_textures = read<std::uint32_t>(file);
	if (num_textures)
	{
		textures = make_unique_default<image2D<std::uint32_t>[]>(num_textures);
		for (unsigned int i = 0; i < num_textures; ++i)
			textures[i] = PNG::loadImage2DR8G8B8A8(file);
	}
}

std::ostream& Scene::serialize(std::ostream& file) const
{
	if (num_vertices > std::numeric_limits<std::uint32_t>::max())
		throw std::runtime_error("too many vertices");
	write(file, static_cast<std::uint32_t>(num_vertices));
	write(file, positions.get(), num_vertices);
	write(file, normals.get(), num_vertices);
	write(file, texcoords.get(), num_vertices);

	//if (num_triangles > std::numeric_limits<std::uint32_t>::max())
	//	throw std::runtime_error("too many triangles");
	//write(file, static_cast<std::uint32_t>(num_triangles));
	//write(file, triangles.get(), num_triangles);
	//write(file, material_ids.get(), num_triangles);

	::serialize(file, *bvh);

	if (num_planes > std::numeric_limits<std::uint32_t>::max())
		throw std::runtime_error("too many planes");
	write(file, static_cast<std::uint32_t>(num_planes));
	write(file, planes.get(), num_planes);

	if (num_cones > std::numeric_limits<std::uint32_t>::max())
		throw std::runtime_error("too many cones");
	write(file, static_cast<std::uint32_t>(num_cones));
	write(file, cones.get(), num_cones);

	if (num_materials > std::numeric_limits<std::uint32_t>::max())
		throw std::runtime_error("too many materials");
	write(file, static_cast<std::uint32_t>(num_materials));
	write(file, materials.get(), num_materials);

	if (num_textures > std::numeric_limits<std::uint32_t>::max())
		throw std::runtime_error("too many textures");
	write(file, static_cast<std::uint32_t>(num_textures));
	for (auto tex = textures.get(); tex < textures.get() + num_textures; ++tex)
		PNG::saveImageR8G8B8A8(file, *tex);

	return file;
}

std::ostream& Scene::visualizeBVH(std::ostream& file) const
{
	return visualize(file, *bvh, &positions[0]);
}

namespace
{
	float3 sampleTextureNearest(const image2D<std::uint32_t>& tex, float u, float v)
	{
		auto W = static_cast<int>(width(tex));
		auto H = static_cast<int>(height(tex));

		auto uu = W * u;
		auto vv = H * v;

		auto x_1 = std::round(uu - 0.5f);
		auto y_1 = std::round(vv - 0.5f);

		auto x = static_cast<int>(x_1);
		auto y = static_cast<int>(y_1);

		auto i = std::clamp(x, 0, W);
		auto j = std::clamp(y, 0, H);

		return gammadecodeR8G8B8(tex(i, j));
	}

	float3 sampleTextureBilinear(const image2D<std::uint32_t>& tex, float u, float v)
	{
		auto W = static_cast<int>(width(tex));
		auto H = static_cast<int>(height(tex));

		auto uu = W * u;
		auto vv = H * v;

		auto x_1 = std::floor(uu - 0.5f);
		auto y_1 = std::floor(vv - 0.5f);

		auto x = static_cast<int>(x_1);
		auto y = static_cast<int>(y_1);

		auto i_1 = std::clamp(x, 0, W);
		auto j_1 = std::clamp(y, 0, H);
		auto i_2 = std::clamp(x + 1, 0, W);
		auto j_2 = std::clamp(y + 1, 0, H);

		auto lambda_x = uu - x - 0.5f;
		auto lambda_y = vv - y - 0.5f;

		auto c_11 = gammadecodeR8G8B8(tex(i_1, j_1));
		auto c_12 = gammadecodeR8G8B8(tex(i_2, j_1));
		auto c_21 = gammadecodeR8G8B8(tex(i_1, j_2));
		auto c_22 = gammadecodeR8G8B8(tex(i_2, j_2));

		return (1.0f - lambda_y) * ((1.0f - lambda_x) * c_11 + lambda_x * c_12) + lambda_y * ((1.0f - lambda_x) * c_21 + lambda_x * c_22);
	}
}

HitPoint Scene::hit(const Plane& plane, const float3& p, const float3& d, float t) const
{
	float3 position = p + t * d;

	auto& mat = materials[plane.material];

	return { position, { plane.p.x, plane.p.y, plane.p.z }, mat.diffuse, mat.specular, mat.shininess };
}

HitPoint Scene::hit(const Cone& cone, const float3& p, const float3& d, float t) const
{
	float3 position = p + t * d;
	float3 CH = position - cone.p;
	float3 normal = normalize(cross(CH, normalize(cross(CH, cone.d))));

	if (dot(CH, normalize(cone.d)) > cone.h - 0.001f) //disk
	{
		normal = cone.d;
	}

	auto& mat = materials[cone.material];

	return { position, normal, mat.diffuse, mat.specular, mat.shininess };
}

HitPoint Scene::hit(const triangle_t& tri, int material_id, float lambda_1, float lambda_2) const
{
	auto i_1 = tri[0], i_2 = tri[1], i_3 = tri[2];
	auto p_1 = positions[i_1], p_2 = positions[i_2], p_3 = positions[i_3];
	auto n_1 = normals[i_1], n_2 = normals[i_2], n_3 = normals[i_3];

	auto p = (1.0f - lambda_1 - lambda_2) * p_1 + lambda_1 * p_2 + lambda_2 * p_3;
	auto n = normalize((1.0f - lambda_1 - lambda_2) * n_1 + lambda_1 * n_2 + lambda_2 * n_3);

	//auto& mat = materials[material_ids[tri - triangles.get()]];
	auto& mat = materials[material_id];

	float3 diffuse = mat.diffuse;
	float3 specular = mat.specular;

	if (mat.diffuse_map >= 0 || mat.specular_map >= 0)
	{
		auto u_1 = texcoords[i_1].x, u_2 = texcoords[i_2].x, u_3 = texcoords[i_3].x;
		auto v_1 = texcoords[i_1].y, v_2 = texcoords[i_2].y, v_3 = texcoords[i_3].y;
		auto u = (1.0f - lambda_1 - lambda_2) * u_1 + lambda_1 * u_2 + lambda_2 * u_3;
		auto v = (1.0f - lambda_1 - lambda_2) * v_1 + lambda_1 * v_2 + lambda_2 * v_3;

		if (mat.diffuse_map >= 0)
		{
			auto c_d = sampleTextureBilinear(textures[mat.diffuse_map], u, v);
			diffuse = { diffuse.x * c_d.x, diffuse.y * c_d.y, diffuse.z * c_d.z };
		}

		if (mat.specular_map >= 0)
		{
			auto c_s = sampleTextureBilinear(textures[mat.specular_map], u, v);
			specular = { specular.x * c_s.x, specular.y * c_s.y, specular.z * c_s.z };
		}
	}

	return { p, n, diffuse, specular, mat.shininess };
}

const Plane* Scene::findClosestHitPlane(const float3& p, const float3& d, float& t) const
{
	Plane* hit = nullptr;

	for (auto plane = &planes[0]; plane != &planes[0] + num_planes; ++plane)
	{
		if (float t_hit; intersectRayPlane(p, d, plane->p, t_hit) && t_hit < t)
		{
			t = t_hit;
			hit = plane;
		}
	}

	return hit;
}

bool Scene::intersectsPlane(const float3& p, const float3& d, float t_min, float t_max) const
{
	for (auto plane = &planes[0]; plane != &planes[0] + num_planes; ++plane)
		if (float t; intersectRayPlane(p, d, plane->p, t) && t >= t_min && t <= t_max)
			return true;
	return false;
}

const Cone* Scene::findClosestHitCone(const float3& p, const float3& d, float& t) const
{
	Cone* hit = nullptr;

	for (auto cone = &cones[0]; cone != &cones[0] + num_cones; ++cone)
	{
		if (float t_hit; intersectRayCone(p, d, cone->p, cone->d, cone->r, cone->h, t_hit) && t_hit < t)
		{
			t = t_hit;
			hit = cone;
		}
	}

	return hit;
}

bool Scene::intersectsCone(const float3& p, const float3& d, float t_min, float t_max) const
{
	for (auto cone = &cones[0]; cone != &cones[0] + num_cones; ++cone)
		if (float t; intersectRayCone(p, d, cone->p, cone->d, cone->r, cone->h, t) && t >= t_min && t <= t_max)
			return true;
	return false;
}

std::tuple<const triangle_t*, int> Scene::findClosestHitTriangle(const float3& p, const float3& d, float& t, float& lambda_1, float& lambda_2) const
{
	//return ::findClosestHit(p, d, triangles.get(), num_triangles, positions.get(), t, lambda_1, lambda_2);
	return bvh->findClosestHit(p, d, positions.get(), t, lambda_1, lambda_2);
}

std::optional<HitPoint> Scene::findClosestHit(const float3& p, const float3& d) const
{
	//if (float t, lambda_1, lambda_2; auto tri = findClosestHit(p, d, t, lambda_1, lambda_2))
	//	return hit(tri, lambda_1, lambda_2);

	//float t, lambda_1, lambda_2;
	//if (auto [tri, mat] = findClosestHit(p, d, t, lambda_1, lambda_2); tri != nullptr)
	//	return hit(tri, mat, lambda_1, lambda_2);
	//return {};

	float t_min = std::numeric_limits<float>::infinity();

	auto plane = findClosestHitPlane(p, d, t_min);
	auto cone = findClosestHitCone(p, d, t_min);

	float t, lambda_1, lambda_2;
	if (auto [tri, mat] = findClosestHitTriangle(p, d, t, lambda_1, lambda_2); tri != nullptr && t < t_min)
		return hit(*tri, mat, lambda_1, lambda_2);

	if (cone)
		return hit(*cone, p, d, t_min);

	if (plane)
		return hit(*plane, p, d, t_min);

	return {};
}

float3 Scene::visualizeIntersections(const float3& p, const float3& d) const
{
	float t = std::numeric_limits<float>::infinity();

	findClosestHitPlane(p, d, t);
	findClosestHitCone(p, d, t);

	if (float t_min, lambda_1, lambda_2; std::get<0>(findClosestHitTriangle(p, d, t_min, lambda_1, lambda_2)) != nullptr && t_min < t)
		return { t_min, lambda_1, lambda_2 };

	if (t != std::numeric_limits<float>::infinity())
		return { t, 0.0f, 0.0f };

	return { 0.0f, 0.0f, 0.0f };
}

bool Scene::intersectsRay(const float3& p, const float3& d, float t_min, float t_max) const
{
	//return ::findClosestHit(p, d, triangles.get(), num_triangles, positions.get(), t_min, t_max);
	return bvh->intersectsRay(p, d, positions.get(), t_min, t_max) || intersectsPlane(p, d, t_min, t_max) || intersectsCone(p, d, t_min, t_max);
}
