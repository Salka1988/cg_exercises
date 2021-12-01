


#ifndef INCLUDED_TASK1B_BVH
#define INCLUDED_TASK1B_BVH

#pragma once

#include <tuple>
#include <iosfwd>

#include <framework/bvh.h>

#include "task1b.h"


struct BVHVisitor;

struct BVHNode
{
	virtual std::tuple<const triangle_t*, int> findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const = 0;
	virtual bool intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const = 0;

	virtual void accept(BVHVisitor& visitor) const = 0;

protected:
	BVHNode() = default;
	BVHNode(const BVHNode&) = default;
	BVHNode(BVHNode&&) = default;
	BVHNode& operator =(const BVHNode&) = default;
	BVHNode& operator =(BVHNode&&) = default;
	~BVHNode() = default;
};

class BVHInnerNode : public BVHNode
{
	BoundingBox bb_left;
	BoundingBox bb_right;
	BVHNode& left;
	BVHNode& right;

public:
	BVHInnerNode(const BoundingBox& bb_left, BVHNode& left, const BoundingBox& bb_right, BVHNode& right);

	std::tuple<const triangle_t*, int> findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const override;
	bool intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const override;

	void accept(BVHVisitor& visitor) const override;
};

class BVHLeafNode : public BVHNode
{
	std::unique_ptr<triangle_t[]> triangles;
	std::unique_ptr<int[]> material_ids;
	std::size_t num_triangles;

public:
	BVHLeafNode(std::unique_ptr<triangle_t[]>&& triangles, std::size_t num_triangles, std::unique_ptr<int[]>&& material_ids);

	std::tuple<const triangle_t*, int> findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const override;
	bool intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const override;

	void accept(BVHVisitor& visitor) const override;
};


class BVHRoot : public BVHNode
{
	BoundingBox bb;
	BVHNode& root;

	std::vector<std::unique_ptr<BVHInnerNode>> nodes;
	std::vector<std::unique_ptr<BVHLeafNode>> leafs;

public:
	BVHRoot(const BoundingBox& bb, BVHNode& root, std::vector<std::unique_ptr<BVHInnerNode>>&& nodes, std::vector<std::unique_ptr<BVHLeafNode>>&& leafs);

	std::tuple<const triangle_t*, int> findClosestHit(const float3& p, const float3& d, const float3* vertices, float& t, float& lambda_1, float& lambda_2) const override;
	bool intersectsRay(const float3& p, const float3& d, const float3* vertices, float t_min, float t_max) const override;

	void accept(BVHVisitor& visitor) const override;
};

std::ostream& serialize(std::ostream& file, const BVHRoot& bvh);
BVHRoot deserializeBVH(std::istream& file);
std::ostream& visualize(std::ostream& file, const BVHRoot& bvh, const float3* vertices);


struct BVHVisitor
{
	virtual void visitNode(const BoundingBox& bb_left, const BVHNode& left, const BoundingBox& bb_right, const BVHNode& right) = 0;
	virtual void visitLeaf(const triangle_t* triangles, std::size_t num_triangles, const int* material_ids) = 0;
	virtual void visitRoot(const BoundingBox& bb, const BVHNode& root) = 0;

protected:
	BVHVisitor() = default;
	BVHVisitor(const BVHVisitor&) = default;
	BVHVisitor(BVHVisitor&&) = default;
	BVHVisitor& operator =(const BVHVisitor&) = default;
	BVHVisitor& operator =(BVHVisitor&&) = default;
	~BVHVisitor() = default;
};

#endif  // INCLUDED_TASK1B_BVH
