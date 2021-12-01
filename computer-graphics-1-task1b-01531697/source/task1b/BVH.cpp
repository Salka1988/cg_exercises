


#include <limits>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <stack>
#include <iostream>

#include <framework/utils/memory.h>
#include <framework/utils/io.h>

#include "BVH.h"


namespace
{
	class BVHOBJDumper : public virtual BVHVisitor
	{
		std::ostream& file;

		const float3* vertices;

		int num_nodes = 0;

		void dump(const BoundingBox& bb)
		{
			file << "v " << bb.limit[0].min << ' ' << bb.limit[1].min << ' ' << bb.limit[2].min << '\n';
			file << "v " << bb.limit[0].max << ' ' << bb.limit[1].min << ' ' << bb.limit[2].min << '\n';
			file << "v " << bb.limit[0].min << ' ' << bb.limit[1].min << ' ' << bb.limit[2].max << '\n';
			file << "v " << bb.limit[0].max << ' ' << bb.limit[1].min << ' ' << bb.limit[2].max << '\n';
			file << "v " << bb.limit[0].min << ' ' << bb.limit[1].max << ' ' << bb.limit[2].min << '\n';
			file << "v " << bb.limit[0].max << ' ' << bb.limit[1].max << ' ' << bb.limit[2].min << '\n';
			file << "v " << bb.limit[0].min << ' ' << bb.limit[1].max << ' ' << bb.limit[2].max << '\n';
			file << "v " << bb.limit[0].max << ' ' << bb.limit[1].max << ' ' << bb.limit[2].max << '\n';

			file << "l -8 -7 -5 -6 -8\n";
			file << "l -4 -3 -1 -2 -4\n";
			file << "l -8 -4\n";
			file << "l -7 -3\n";
			file << "l -6 -2\n";
			file << "l -5 -1\n";
		}

	public:
		BVHOBJDumper(std::ostream& file, const float3* vertices)
			: file(file),
			  vertices(vertices)
		{
		}

		void visitNode(const BoundingBox& bb_left, const BVHNode& left, const BoundingBox& bb_right, const BVHNode& right) override
		{
			file << "g node_" << num_nodes++ << '\n';
			dump(bb_left);
			left.accept(*this);

			file << "g node_" << num_nodes++ << '\n';
			dump(bb_right);
			right.accept(*this);
		}

		void visitLeaf(const triangle_t* triangles, std::size_t num_triangles, const int* material_ids) override
		{
			for (auto tri = triangles; tri != triangles + num_triangles; ++tri)
			{
				for (int i = 0; i < 3; ++i)
					file << "v " << vertices[(*tri)[i]].x << ' ' << vertices[(*tri)[i]].y << ' ' << vertices[(*tri)[i]].z << '\n';
				file << "f -1 -2 -3\n";
			}
		}

		void visitRoot(const BoundingBox& bb, const BVHNode& root) override
		{
			file << "g node_" << num_nodes++ << '\n';
			dump(bb);
			root.accept(*this);
		}
	};


	std::ostream& writeBB(std::ostream& file, const BoundingBox& bb)
	{
		return write(file, bb.limit);
	}

	BoundingBox readBB(std::istream& file)
	{
		return { { { read<float>(file), read<float>(file) }, { read<float>(file), read<float>(file) }, { read<float>(file), read<float>(file) } } };
	}

	class BVHSerializer : public virtual BVHVisitor
	{
		std::ostream& file;

		unsigned int num_nodes = 0U;

	public:
		BVHSerializer(std::ostream& file)
			: file(file)
		{
		}

		void visitNode(const BoundingBox& bb_left, const BVHNode& left, const BoundingBox& bb_right, const BVHNode& right) override
		{
			left.accept(*this);
			right.accept(*this);

			write(file, static_cast<std::int32_t>(-1));
			writeBB(file, bb_left);
			writeBB(file, bb_right);
		}

		void visitLeaf(const triangle_t* triangles, std::size_t num_triangles, const int* material_ids) override
		{
			if (num_triangles > std::numeric_limits<std::int32_t>::max())
				throw std::runtime_error("too many triangles in leaf");

			write(file, static_cast<std::uint32_t>(num_triangles));
			write(file, triangles, num_triangles);
			write(file, material_ids, num_triangles);
			++num_nodes;
		}

		void visitRoot(const BoundingBox& bb, const BVHNode& root) override
		{
			root.accept(*this);
			write(file, static_cast<std::int32_t>(-2));
			writeBB(file, bb);
		}
	};
}

std::ostream& serialize(std::ostream& file, const BVHRoot& bvh)
{
	BVHSerializer serializer(file);
	bvh.accept(serializer);
	return file;
}

BVHRoot deserializeBVH(std::istream& file)
{
	BoundingBox bb;

	std::vector<std::unique_ptr<BVHInnerNode>> nodes;
	std::vector<std::unique_ptr<BVHLeafNode>> leafs;

	std::stack<BVHNode*> node_stack;

	while (true)
	{
		auto num_triangles = read<std::int32_t>(file);

		if (num_triangles < 0)
		{
			if (num_triangles == -1)
			{
				auto bb_left = readBB(file);
				auto bb_right = readBB(file);
				auto right = node_stack.top();
				node_stack.pop();
				auto left = node_stack.top();
				node_stack.pop();

				node_stack.push(nodes.emplace_back(std::make_unique<BVHInnerNode>(bb_left, *left, bb_right, *right)).get());
			}
			else if (num_triangles == -2)
			{
				bb = readBB(file);
				break;
			}
			else
				throw std::runtime_error("invalid node type");
		}
		else
		{
			auto triangles = num_triangles ? make_unique_default<triangle_t[]>(num_triangles) : nullptr;
			auto material_ids = num_triangles ? make_unique_default<int[]>(num_triangles) : nullptr;
			read(triangles.get(), file, num_triangles);
			read(material_ids.get(), file, num_triangles);

			node_stack.push(leafs.emplace_back(std::make_unique<BVHLeafNode>(std::move(triangles), num_triangles, std::move(material_ids))).get());
		}
	}

	return { bb, *node_stack.top(), std::move(nodes), std::move(leafs) };
}

std::ostream& visualize(std::ostream& file, const BVHRoot& bvh, const float3* vertices)
{
	BVHOBJDumper dumper(file, vertices);
	bvh.accept(dumper);
	return file;
}

BVHInnerNode::BVHInnerNode(const BoundingBox& bb_left, BVHNode& left, const BoundingBox& bb_right, BVHNode& right)
	: bb_left(bb_left),
	  bb_right(bb_right),
	  left(left),
	  right(right)
{
}

void BVHInnerNode::accept(BVHVisitor& visitor) const
{
	visitor.visitNode(bb_left, left, bb_right, right);
}

namespace
{
	void lalala(const triangle_t*& tri, float& t, float& lambda_1, float& lambda_2, int& mat, const float3& p, const float3& d, const float3* vertices, const BoundingBox& bb, const BVHNode& node)
	{
		if (intersect(bb, p, d, 0.0f))
		{
			float t_found;
			if (auto [tri_found, mat_found] = node.findClosestHit(p, d, vertices, t_found, lambda_1, lambda_2); tri_found != nullptr)
			{
				tri = tri_found;
				t = t_found;
				mat = mat_found;
			}
		}
	}
}

std::tuple<const triangle_t*, int> BVHInnerNode::findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const
{
	const triangle_t* tri_l = nullptr;
	float t_l = std::numeric_limits<float>::infinity();
	float lambda_1_l;
	float lambda_2_l;
	int mat_l;
	lalala(tri_l, t_l, lambda_1_l, lambda_2_l, mat_l, p, d, vertices, bb_left, left);

	const triangle_t* tri_r;
	float t_r = std::numeric_limits<float>::infinity();
	float lambda_1_r;
	float lambda_2_r;
	int mat_r;
	lalala(tri_r, t_r, lambda_1_r, lambda_2_r, mat_r, p, d, vertices, bb_right, right);

	if (t_r < t_l)
	{
		t = t_r;
		lambda_1 = lambda_1_r;
		lambda_2 = lambda_2_r;
		return { tri_r, mat_r };
	}

	if (tri_l != nullptr)
	{
		t = t_l;
		lambda_1 = lambda_1_l;
		lambda_2 = lambda_2_l;
		return { tri_l, mat_l };
	}

	return { nullptr, -1 };
}

bool BVHInnerNode::intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const
{
	return (intersect(bb_left, p, d, t_min, t_max) && left.intersectsRay(p, d, vertices, t_min, t_max)) ||
	       (intersect(bb_right, p, d, t_min, t_max) && right.intersectsRay(p, d, vertices, t_min, t_max));
}

BVHLeafNode::BVHLeafNode(std::unique_ptr<triangle_t[]>&& triangles, std::size_t num_triangles, std::unique_ptr<int[]>&& material_ids)
	: triangles(std::move(triangles)),
	  material_ids(std::move(material_ids)),
	  num_triangles(num_triangles)
{
}

std::tuple<const triangle_t*, int> BVHLeafNode::findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const
{
	if (auto tri = ::findClosestHit(p, d, &triangles[0], num_triangles, vertices, t, lambda_1, lambda_2))
		return { tri, material_ids[tri - &triangles[0]] };
	return { nullptr, -1 };
}

bool BVHLeafNode::intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const
{
	return ::intersectsRay(p, d, &triangles[0], num_triangles, vertices, t_min, t_max);
}

void BVHLeafNode::accept(BVHVisitor& visitor) const
{
	visitor.visitLeaf(&triangles[0], num_triangles, &material_ids[0]);
}


BVHRoot::BVHRoot(const BoundingBox& bb, BVHNode& root, std::vector<std::unique_ptr<BVHInnerNode>>&& nodes, std::vector<std::unique_ptr<BVHLeafNode>>&& leafs)
	: bb(bb), root(root), nodes(std::move(nodes)), leafs(std::move(leafs))
{
}

std::tuple<const triangle_t*, int> BVHRoot::findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const
{
	if (intersect(bb, p, d, 0.0f))
		return root.findClosestHit(p, d, vertices, t, lambda_1, lambda_2);
	return { nullptr, -1 };
}

bool BVHRoot::intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const
{
	return intersect(bb, p, d, t_min, t_max) && root.intersectsRay(p, d, vertices, t_min, t_max);
}

void BVHRoot::accept(BVHVisitor& visitor) const
{
	visitor.visitRoot(bb, root);
}
