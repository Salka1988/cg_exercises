

#ifndef INCLUDED_FRAMEWORK_BVH
#define INCLUDED_FRAMEWORK_BVH

#pragma once

#include <limits>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>

#include <framework/math/vector.h>
#include <framework/bounding_box.h>


namespace BVH
{
	template <typename Triangle>
	constexpr BoundingBox& updateBB(BoundingBox& bb, const Triangle& tr, const float3* vertices)
	{
		bb.limit[0].min = std::min(std::min(std::min(bb.limit[0].min, vertices[tr[0]].x), vertices[tr[1]].x), vertices[tr[2]].x);
		bb.limit[1].min = std::min(std::min(std::min(bb.limit[1].min, vertices[tr[0]].y), vertices[tr[1]].y), vertices[tr[2]].y);
		bb.limit[2].min = std::min(std::min(std::min(bb.limit[2].min, vertices[tr[0]].z), vertices[tr[1]].z), vertices[tr[2]].z);
		bb.limit[0].max = std::max(std::max(std::max(bb.limit[0].max, vertices[tr[0]].x), vertices[tr[1]].x), vertices[tr[2]].x);
		bb.limit[1].max = std::max(std::max(std::max(bb.limit[1].max, vertices[tr[0]].y), vertices[tr[1]].y), vertices[tr[2]].y);
		bb.limit[2].max = std::max(std::max(std::max(bb.limit[2].max, vertices[tr[0]].z), vertices[tr[1]].z), vertices[tr[2]].z);
		return bb;
	}

	constexpr std::tuple<BoundingBox, BoundingBox> splitBB(const BoundingBox& bb, int axis)
	{
		auto bbox_left = bb;
		auto bbox_right = bb;
		auto half = extent(bb.limit[axis]) / 2.0f;
		bbox_left.limit[axis].max -= half;
		bbox_right.limit[axis].min += half;
		return { bbox_left, bbox_right };
	}

	template <typename TriangleIterator, typename TriangleSentinel>
	constexpr BoundingBox boundingBox(TriangleIterator triangles_begin, TriangleSentinel triangles_end, const float3* vertices)
	{
		constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
		BoundingBox bbox = { { {FLOAT_MAX, -FLOAT_MAX}, {FLOAT_MAX, -FLOAT_MAX}, {FLOAT_MAX, -FLOAT_MAX} } };
		for (auto tr = triangles_begin; tr != triangles_end; ++tr)
			updateBB(bbox, *tr, vertices);
		return bbox;
	}


	template <int axis> float coord(const math::float3&) = delete;
	template <> inline float coord<0>(const math::float3& v) { return v.x; }
	template <> inline float coord<1>(const math::float3& v) { return v.y; }
	template <> inline float coord<2>(const math::float3& v) { return v.z; }


	template <typename TriangleIterator, typename TriangleSentinel>
	void updateBoundingBox(BoundingBox& bb, TriangleIterator triangles_begin, TriangleSentinel triangles_end, const float3* vertices)
	{
		auto new_bb = boundingBox(triangles_begin, triangles_end, vertices);
		for (int i = 0; i < 3; ++i)
		{
			bb.limit[i].min = std::max(bb.limit[i].min, new_bb.limit[i].min);
			bb.limit[i].max = std::min(bb.limit[i].max, new_bb.limit[i].max);
		}
	};


	template <typename TriangleIterator, typename TriangleSentinel, typename Node>
	bool continueProcessing(TriangleIterator triangles_begin, TriangleSentinel triangles_end, const Node& l, const Node& r)
	{
		auto[bb_l, tr_l] = l;
		auto[bb_r, tr_r] = r;
		auto n_l = size(tr_l);
		auto n_r = size(tr_r);
		std::size_t num_triangles = triangles_end - triangles_begin;
		return n_l < num_triangles && n_r < num_triangles;
	}

	template <int axis, typename TriangleIterator, typename TriangleSentinel>
	decltype(auto) split(const BoundingBox& bb, TriangleIterator triangles_begin, TriangleSentinel triangles_end, const float3* vertices)
	{
		auto[bb_l, bb_r] = splitBB(bb, axis);
		std::vector<typename std::iterator_traits<TriangleIterator>::value_type> tr_l, tr_r;
		for (auto tr = triangles_begin; tr != triangles_end; ++tr)
		{
			auto v0 = vertices[(*tr)[0]];
			auto v1 = vertices[(*tr)[1]];
			auto v2 = vertices[(*tr)[2]];
			auto m = bb_l.limit[axis].max;
			auto v0_ = coord<axis>(v0);
			auto v1_ = coord<axis>(v1);
			auto v2_ = coord<axis>(v2);
			//because we know how the splitting works. changed splitting? change this.
			if (coord<axis>(v0) <= bb_l.limit[axis].max ||
				coord<axis>(v1) <= bb_l.limit[axis].max ||
				coord<axis>(v2) <= bb_l.limit[axis].max)
				tr_l.push_back((*tr));
			if (coord<axis>(v0) >= bb_r.limit[axis].min ||
				coord<axis>(v1) >= bb_r.limit[axis].min ||
				coord<axis>(v2) >= bb_r.limit[axis].min)
				tr_r.push_back((*tr));
		}
		updateBoundingBox(bb_l, data(tr_l), data(tr_l) + size(tr_l), vertices);
		updateBoundingBox(bb_r, data(tr_r), data(tr_r) + size(tr_r), vertices);
		return std::tuple { std::tuple { bb_l, tr_l }, std::tuple { bb_r, tr_r } };
	};

	template <typename TriangleIterator, typename TriangleSentinel>
	decltype(auto) findBestSplit(const BoundingBox& bb, TriangleIterator triangles_begin, TriangleSentinel triangles_end, const float3* vertices)
	{
		auto[l0, r0] = split<0>(bb, triangles_begin, triangles_end, vertices);
		auto[l1, r1] = split<1>(bb, triangles_begin, triangles_end, vertices);
		auto[l2, r2] = split<2>(bb, triangles_begin, triangles_end, vertices);

		auto[bb_l0, tr_l0] = l0;
		auto[bb_r0, tr_r0] = r0;
		auto n0 = size(tr_l0) + size(tr_r0);
		auto[bb_l1, tr_l1] = l1;
		auto[bb_r1, tr_r1] = r1;
		auto n1 = size(tr_l1) + size(tr_r1);
		auto[bb_l2, tr_l2] = l2;
		auto[bb_r2, tr_r2] = r2;
		auto n2 = size(tr_l2) + size(tr_r2);

		if (n0 <= n1 && n0 <= n2)
			return std::tuple { l0, r0 };

		if (n1 <= n0 && n1 <= n2)
			return std::tuple { l1, r1 };

		if (n2 <= n0 && n2 <= n1)
			return std::tuple { l2, r2 };

		throw std::runtime_error("this should never happen");
	}

	template <typename TriangleIterator, typename TriangleSentinel, typename Builder>
	decltype(auto) build(Builder& builder, int depth, const BoundingBox& bb, TriangleIterator triangles_begin, TriangleSentinel triangles_end, const float3* vertices, int max_depth, int max_triangles_per_node)
	{
		if ((triangles_end - triangles_begin) <= max_triangles_per_node || depth >= max_depth)
			return builder.createLeaf(triangles_begin, triangles_end);

		auto [l, r] = findBestSplit(bb, triangles_begin, triangles_end, vertices);
		if (!continueProcessing(triangles_begin, triangles_end, l, r))
			return builder.createLeaf(triangles_begin, triangles_end);

		auto [bb_l, tr_l] = l;
		auto left_child = build(builder, depth + 1, bb_l, begin(tr_l), end(tr_l), vertices, max_depth, max_triangles_per_node);

		auto [bb_r, tr_r] = r;
		auto right_child = build(builder, depth + 1, bb_r, begin(tr_r), end(tr_r), vertices, max_depth, max_triangles_per_node);

		return builder.createNode(bb_l, std::move(left_child), bb_r, std::move(right_child));
	};

	template <typename TriangleIterator, typename TriangleSentinel, typename Builder>
	Builder& build(Builder& builder, TriangleIterator triangles_begin, TriangleSentinel triangles_end, const float3* vertices, int max_depth, int max_triangles_per_node)
	{
		auto bb = boundingBox(triangles_begin, triangles_end, vertices);
		auto root = build(builder, 1, bb, triangles_begin, triangles_end, vertices, max_depth, max_triangles_per_node);
		builder.finish(bb, root);
		return builder;
	}
}

#endif  // INCLUDED_FRAMEWORK_BVH
