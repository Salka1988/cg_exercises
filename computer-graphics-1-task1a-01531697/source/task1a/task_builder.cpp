

#include <limits>
#include <cmath>
#include <memory>
#include <variant>
#include <optional>

#include <framework/config.h>

#include "task_builder.h"


namespace
{
	constexpr float NaN = std::numeric_limits<float>::quiet_NaN();

	class CameraObjectConsumer : public config::RejectingParser
	{
		float w_s = NaN;
		float f = NaN;

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::NumberConsumer<float>
		> consumers;

	public:
		CameraObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "f")
				return &consumers.emplace<config::NumberConsumer<float>>(this, f);
			else if (str == "w_s")
				return &consumers.emplace<config::NumberConsumer<float>>(this, w_s);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (std::isnan(w_s))
				throw config::parse_error("camera definition missing w_s");
			if (std::isnan(f))
				throw config::parse_error("camera definition missing f");

			builder.defineCamera(w_s, f);
			return ret;
		}
	};

	using CameraParser = config::ObjectParser<CameraObjectConsumer>;


	class PlaneSceneObjectConsumer : public config::RejectingParser
	{
		float3 p = { NaN, NaN, NaN };
		float3 n = { NaN, NaN, NaN };

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::VectorParser<float, 3>
		> consumers;

	public:
		PlaneSceneObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "point")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, p);
			else if (str == "normal")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, n);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (std::isnan(p.x) || std::isnan(p.y) || std::isnan(p.z))
				throw config::parse_error("plane missing point");
			if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z))
				throw config::parse_error("plane missing normal");

			builder.definePlane(n.x, n.y, n.z, dot(p, n));
			return ret;
		}
	};

	class SphereSceneObjectConsumer : public config::RejectingParser
	{
		float3 c = { NaN, NaN, NaN };
		float r = NaN;

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::NumberConsumer<float>,
			config::VectorParser<float, 3>
		> consumers;

	public:
		SphereSceneObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "center")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, c);
			else if (str == "radius")
				return &consumers.emplace<config::NumberConsumer<float>>(this, r);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (std::isnan(c.x) || std::isnan(c.y) || std::isnan(c.z))
				throw config::parse_error("sphere missing center");
			if (std::isnan(r))
				throw config::parse_error("sphere missing radius");

			if (r < 0.0f)
				throw config::parse_error("radius must not be negative");

			builder.defineSphere(c, r);
			return ret;
		}
	};

	class BoxSceneObjectConsumer : public config::RejectingParser
	{
		float3 min = { NaN, NaN, NaN };
		float3 max = { NaN, NaN, NaN };

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::VectorParser<float, 3>
		> consumers;

	public:
		BoxSceneObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "min")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, min);
			else if (str == "max")
				return &consumers.emplace<config::VectorParser<float, 3>>(this, max);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			if (std::isnan(min.x) || std::isnan(min.y) || std::isnan(min.z))
				throw config::parse_error("box missing min");
			if (std::isnan(max.x) || std::isnan(max.y) || std::isnan(max.z))
				throw config::parse_error("box missing max");

			if (max.x < min.x || max.y < min.y || max.z < min.z)
				throw config::parse_error("invalid box dimensions");

			builder.defineBox(min, max);
			return ret;
		}
	};


	class SceneObjectConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			PlaneSceneObjectConsumer,
			SphereSceneObjectConsumer,
			BoxSceneObjectConsumer
		> consumers;

	public:
		SceneObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseString(std::string&& str) override
		{
			if (str == "plane")
				return &consumers.emplace<PlaneSceneObjectConsumer>(ret, builder);
			else if (str == "sphere")
				return &consumers.emplace<SphereSceneObjectConsumer>(ret, builder);
			else if (str == "box")
				return &consumers.emplace<BoxSceneObjectConsumer>(ret, builder);

			return RejectingParser::parseKey(std::move(str));
		}
	};


	class SceneObjectArrayConsumer : public config::RejectingParser
	{
		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			SceneObjectConsumer
		> consumers;

	public:
		SceneObjectArrayConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* beginObject() override
		{
			return this;
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "type")
				return &consumers.emplace<SceneObjectConsumer>(this, builder);

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
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			CameraParser,
			SceneObjectArrayParser
		> consumers;

	public:
		SceneConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "camera")
				return &consumers.emplace<CameraParser>(this, builder);
			else if (str == "objects")
				return &consumers.emplace<SceneObjectArrayParser>(this, builder);

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
		float4 background_color = { 0.0f, 0.0f, 0.0f, 1.0f };

		config::Parser* ret;
		TaskBuilder& builder;

		std::variant<
			std::monostate,
			config::NumberConsumer<int>,
			config::VectorParser<float, 4>,
			SceneParser
		> consumers;

	public:
		RenderingObjectConsumer(config::Parser* ret, TaskBuilder& builder)
			: ret(ret), builder(builder)
		{
		}

		Parser* parseKey(std::string&& str) override
		{
			if (str == "width")
				return &consumers.emplace<config::NumberConsumer<int>>(this, width);
			else if (str == "height")
				return &consumers.emplace<config::NumberConsumer<int>>(this, height);
			else if (str == "background")
				return &consumers.emplace<config::VectorParser<float, 4>>(this, background_color);
			else if (str == "scene")
				return &consumers.emplace<SceneParser>(this, builder);

			return RejectingParser::parseKey(std::move(str));
		}

		Parser* endObject(std::size_t num_members) override
		{
			builder.defineCanvas(width, height, background_color);
			builder.finalize();
			return ret;
		}
	};

	using RenderingParser = config::RootObjectParser<RenderingObjectConsumer>;
}

void load(TaskBuilder& builder, const std::filesystem::path& config_file)
{
	RenderingParser parser(builder);
	config::read(parser, config_file);
}
