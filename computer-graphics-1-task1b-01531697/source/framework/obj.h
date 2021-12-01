


#ifndef INCLUDED_FRAMEWORK_OBJ
#define INCLUDED_FRAMEWORK_OBJ

#pragma once

#include <stdexcept>
#include <array>
#include <vector>
#include <filesystem>

#include <framework/math/vector.h>


namespace OBJ
{
	class parse_error : std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};


	struct Triangles
	{
		std::vector<float3> positions;
		std::vector<float3> normals;
		std::vector<float2> texcoords;
		std::vector<std::array<int, 3>> triangles;
	};

	Triangles readTriangles(const char* begin, const char* end, const char* name);
	Triangles readTriangles(const std::filesystem::path& path);
}

#endif  // INCLUDED_FRAMEWORK_OBJ
