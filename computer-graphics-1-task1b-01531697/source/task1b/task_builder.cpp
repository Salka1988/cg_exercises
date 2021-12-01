


#include <limits>
#include <memory>
#include <variant>
#include <optional>

#include <framework/config.h>

#include "task_builder.h"


namespace
{
	constexpr float NaN = std::numeric_limits<float>::quiet_NaN();

	std::filesystem::path searchPath(std::filesystem::path p, const std::filesystem::path& search_path)
	{
		if (!p.is_absolute())
			return search_path/p;
		return p;
	}


	class CameraObjectConsumer : public config::RejectingParser
	{
		float w_s = 0.036f;
		float f = 0.024f;
		float3 eye = { 0.0f, 0.0f, 0.0f };
		float3 lookat = { 0.0f, 0.0f, 0.0f };
		float3 up = { 0.0f, 0.0f, 0.0f };

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::NumberConsumer<float>,
			config::VectorParser<float, 3>
		> consumers;

	public:
		CameraObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "eye")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, eye);
			else if (str == "lookat")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, lookat);
			else if (str == "up")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, up);
			else if (str == "f")
				return &consumers.emplace<config::NumberConsumer<float>>(this, f);
			else if (str == "w_s")
				return &consumers.emplace<config::NumberConsumer<float>>(this, w_s);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			builder.defineCamera(w_s, f, eye, lookat, up);
			return ret;
		}
	};

	using CameraParser = config::ObjectParser<CameraObjectConsumer>;


	class LightObjectConsumer : public config::RejectingParser
	{
		float3 p;
		float3 color;

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::VectorParser<float, 3>
		> consumers;

	public:
		LightObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "p")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, p);
			else if (str == "color")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, color);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			builder.defineLight(p, color);
			return ret;
		}
	};

	class LightsConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			LightObjectConsumer
		> consumers;

	public:
		LightsConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* beginObject() override
		{
			return &consumers.emplace<LightObjectConsumer>(this, builder);
		}

		Parser* endArray(std::size_t num_elements) override
		{
			return ret;
		}
	};

	using LightArrayParser = config::ArrayParser<LightsConsumer>;


	class MaterialObjectConsumer : public config::RejectingParser
	{
		std::string name;
		float3 diffuse = { 0.8f, 0.8f, 0.8f };
		std::string diffuse_map;
		float3 specular = { 0.0f, 0.0f, 0.0f };
		std::string specular_map;
		float shininess = 1.0f;
		bool mirror = false;

		config::Parser* ret;
		const std::filesystem::path& search_path;
		TaskBuilder& builder;

		std::filesystem::path texturePath(std::filesystem::path tex_name)
		{
			return tex_name.empty() ? tex_name : searchPath(tex_name, search_path);
		}

		std::variant<
			std::monostate,
			config::NumberConsumer<float>,
			config::BoolConsumer,
			config::VectorParser<float, 3>,
			config::StringConsumer
		> consumers;

	public:
		MaterialObjectConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "name")
				return &consumers.emplace<config::StringConsumer>(this, name);
			else if (str == "diffuse")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, diffuse);
			else if (str == "diffuse_map")
				return &consumers.emplace<config::StringConsumer>(this, diffuse_map);
			else if (str == "specular")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, specular);
			else if (str == "specular_map")
				return &consumers.emplace<config::StringConsumer>(this, specular_map);
			else if (str == "shininess")
				return &consumers.emplace<config::NumberConsumer<float>>(this, shininess);
			else if (str == "mirror")
				return &consumers.emplace<config::BoolConsumer>(this, mirror);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (name.empty())
				throw config::parse_error("material must be given a name");

			builder.defineMaterial(name, diffuse, texturePath(diffuse_map), specular, texturePath(specular_map), mirror ? std::numeric_limits<float>::infinity() : shininess);

			return ret;
		}
	};

	class MaterialsConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		const std::filesystem::path& search_path;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			MaterialObjectConsumer
		> consumers;

	public:
		MaterialsConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* beginObject() override
		{
			return &consumers.emplace<MaterialObjectConsumer>(this, builder, search_path);
		}

		Parser* endArray(std::size_t num_elements) override
		{
			return ret;
		}
	};

	using MaterialArrayParser = config::ArrayParser<MaterialsConsumer>;


	class PlaneSceneObjectConsumer : public config::RejectingParser
	{
		float4 p = { NaN, NaN, NaN, NaN };
		std::string material;

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::VectorParser<float, 4>,
			config::StringConsumer
		> consumers;

	public:
		PlaneSceneObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "p")
				return &consumers.emplace<config::VectorParser<float, 4>>(this, p);
			else if (str == "material")
				return &consumers.emplace<config::StringConsumer>(this, material);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (std::isnan(p.x) || std::isnan(p.y) || std::isnan(p.z) || std::isnan(p.w))
				throw config::parse_error("plane missing coefficients");

			builder.definePlane(p, material);
			return ret;
		}
	};

	class ConeSceneObjectConsumer : public config::RejectingParser
	{
		float3 p = { NaN, NaN, NaN };
		float3 d = { NaN, NaN, NaN };
		float r = NaN;
		float h = NaN;
		std::string material;

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::NumberConsumer<float>,
			config::VectorParser<float, 3>,
			config::StringConsumer
		> consumers;

	public:
		ConeSceneObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "p")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, p);
			else if (str == "d")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, d);
			else if (str == "r")
				return &consumers.emplace<config::NumberConsumer<float>>(this, r);
			else if (str == "h")
				return &consumers.emplace<config::NumberConsumer<float>>(this, h);
			else if (str == "material")
				return &consumers.emplace<config::StringConsumer>(this, material);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (std::isnan(p.x) || std::isnan(p.y) || std::isnan(p.z))
				throw config::parse_error("cone missing position");

			if (std::isnan(d.x) || std::isnan(d.y) || std::isnan(d.z))
				throw config::parse_error("cone missing direction");

			if (std::isnan(r))
				throw config::parse_error("cone missing radius");

			if (std::isnan(h))
				throw config::parse_error("cone missing height");

			builder.defineCone(p, d, r, h, material);
			return ret;
		}
	};

	class ObjSceneObjectConsumer : public config::RejectingParser
	{
		std::string filename;
		float4x4 M = math::identity<float4x4>;
		std::string material;

		config::Parser* ret;
		const std::filesystem::path& search_path;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::StringConsumer,
			config::MatrixParser<float, 4, 4>
		> consumers;

	public:
		ObjSceneObjectConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "file")
				return &consumers.emplace<config::StringConsumer>(this, filename);
			else if (str == "M")
				return &consumers.emplace<config::MatrixParser<float, 4, 4>>(this, M);
			else if (str == "material")
				return &consumers.emplace<config::StringConsumer>(this, material);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			builder.defineModel(searchPath(filename, search_path), M, material);
			return ret;
		}
	};


	class SceneObjectConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		const std::filesystem::path& search_path;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			PlaneSceneObjectConsumer,
			ConeSceneObjectConsumer,
			ObjSceneObjectConsumer
		> consumers;

	public:
		SceneObjectConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* parseString(std::string&& str) override
		{
			if (str == "obj")
				return &consumers.emplace<ObjSceneObjectConsumer>(ret, builder, search_path);
			else if (str == "plane")
				return &consumers.emplace<PlaneSceneObjectConsumer>(ret, builder);
			else if (str == "cone")
				return &consumers.emplace<ConeSceneObjectConsumer>(ret, builder);

			return RejectingParser::parseKey(std::move(str));
		}
	};


	class SceneObjectArrayConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		const std::filesystem::path& search_path;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			SceneObjectConsumer
		> consumers;

	public:
		SceneObjectArrayConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* beginObject() override
		{
			return this;
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "type")
				return &consumers.emplace<SceneObjectConsumer>(this, builder, search_path);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endArray(std::size_t num_elements) override
		{
			return ret;
		}
	};

	using SceneObjectArrayParser = config::ArrayParser<SceneObjectArrayConsumer>;


	class SceneConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		const std::filesystem::path& search_path;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			CameraParser,
			SceneObjectArrayParser,
			MaterialArrayParser,
			LightArrayParser
		> consumers;

	public:
		SceneConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "camera")
				return &consumers.emplace<CameraParser>(this, builder);
			else if (str == "objects")
				return &consumers.emplace<SceneObjectArrayParser>(this, builder, search_path);
			else if (str == "materials")
				return &consumers.emplace<MaterialArrayParser>(this, builder, search_path);
			else if (str == "lights")
				return &consumers.emplace<LightArrayParser>(this, builder);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			return ret;
		}
	};

	using SceneParser = config::ObjectParser<SceneConsumer>;


	class RenderingObjectConsumer : public config::RejectingParser
	{
		int width = 800;
		int height = 600;
		float3 background_color = { 1.0f, 1.0f, 1.0f };
		
		config::Parser* ret;
		const std::filesystem::path search_path;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::NumberConsumer<int>,
			config::VectorParser<float, 3>,
			SceneParser
		> consumers;

	public:
		RenderingObjectConsumer(config::Parser* ret, TaskBuilder& builder, const std::filesystem::path& search_path)
			: ret(ret), builder(builder), search_path(search_path)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "width")
				return &consumers.emplace<config::NumberConsumer<int>>(this, width);
			else if (str == "height")
				return &consumers.emplace<config::NumberConsumer<int>>(this, height);
			else if (str == "background")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, background_color);
			else if (str == "scene")
				return &consumers.emplace<SceneParser>(this, builder, search_path);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			builder.defineCanvas(width, height, background_color);
			return ret;
		}
	};

	using RenderingParser = config::RootObjectParser<RenderingObjectConsumer>;
}

void load(TaskBuilder& builder, const std::filesystem::path& config_file)
{
	RenderingParser parser(builder, config_file.parent_path());
	config::read(parser, config_file);
}
