


#include <cstdlib>
#include <stack>
#include <fstream>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>

#include <framework/config.h>


namespace
{
	class ConfigHandler
	{
		config::Parser* parser;

	public:
		ConfigHandler(config::Parser& parser)
			: parser(&parser)
		{
		}

		bool Null()
		{
			parser = parser->parseNull();
			return true;
		}

		bool Bool(bool b)
		{
			parser = parser->parseBool(b);
			return true;
		}

		bool Int(int i)
		{
			parser = parser->parseNumber(i);
			return true;
		}

		bool Uint(unsigned int i)
		{
			parser = parser->parseNumber(i);
			return true;
		}

		bool Int64(int64_t i)
		{
			parser = parser->parseNumber(i);
			return true;
		}

		bool Uint64(uint64_t i)
		{
			parser = parser->parseNumber(i);
			return true;
		}

		bool Double(double d)
		{
			parser = parser->parseNumber(d);
			return true;
		}

		bool RawNumber(const char* str, std::size_t length, bool copy)
		{
			throw config::parse_error("RawNumber unexpected");
		}

		bool String(const char* str, std::size_t length, bool copy)
		{
			if (copy)
			{
				parser = parser->parseString(std::string(str, str + length));
				return true;
			}

			throw config::parse_error("insitu string unexpected");
		}

		bool Key(const char* str, std::size_t length, bool copy)
		{
			if (copy)
			{
				parser = parser->parseKey(std::string(str, str + length));
				return true;
			}

			throw config::parse_error("insitu key unexpected");
		}

		bool StartObject()
		{
			parser = parser->beginObject();
			return true;
		}

		bool EndObject(std::size_t memberCount)
		{
			parser = parser->endObject(memberCount);
			return true;
		}

		bool StartArray()
		{
			parser = parser->beginArray();
			return true;
		}

		bool EndArray(std::size_t elementCount)
		{
			parser = parser->endArray(elementCount);
			return true;
		}
	};
}

namespace config
{
	Parser* RejectingParser::parseNull()
	{
		throw parse_error("unexpected");
	}

	Parser* RejectingParser::parseBool(bool b)
	{
		throw parse_error("unexpected bool");
	}

	Parser* RejectingParser::parseNumber(int i)
	{
		throw parse_error(i, ": number unexpected");
	}

	Parser* RejectingParser::parseNumber(unsigned int i)
	{
		throw parse_error(i, ": number unexpected");
	}

	Parser* RejectingParser::parseNumber(std::int64_t i)
	{
		throw parse_error(i, ": number unexpected");
	}

	Parser* RejectingParser::parseNumber(std::uint64_t i)
	{
		throw parse_error(i, ": number unexpected");
	}

	Parser* RejectingParser::parseNumber(double d)
	{
		throw parse_error(d, ": number unexpected");
	}

	Parser* RejectingParser::parseString(std::string&& str)
	{
		throw parse_error("unexpected string '", str, '\'');
	}

	Parser* RejectingParser::parseKey(std::string&& str)
	{
		throw parse_error("unexpected key '", str, '\'');
	}

	Parser* RejectingParser::beginObject()
	{
		throw parse_error("unexpected object");
	}

	Parser* RejectingParser::endObject(std::size_t num_members)
	{
		return nullptr;
	}

	Parser* RejectingParser::beginArray()
	{
		throw parse_error("unexpected array");
	}

	Parser* RejectingParser::endArray(std::size_t num_elements)
	{
		return nullptr;
	}


	Parser* GobblingParser::parseNull()
	{
		return this;
	}

	Parser* GobblingParser::parseBool(bool b)
	{
		return this;
	}

	Parser* GobblingParser::parseNumber(int i)
	{
		return this;
	}

	Parser* GobblingParser::parseNumber(unsigned int i)
	{
		return this;
	}

	Parser* GobblingParser::parseNumber(std::int64_t i)
	{
		return this;
	}

	Parser* GobblingParser::parseNumber(std::uint64_t i)
	{
		return this;
	}

	Parser* GobblingParser::parseNumber(double d)
	{
		return this;
	}

	Parser* GobblingParser::parseString(std::string&& str)
	{
		return this;
	}

	Parser* GobblingParser::parseKey(std::string&& str)
	{
		return this;
	}

	Parser* GobblingParser::beginObject()
	{
		return this;
	}

	Parser* GobblingParser::endObject(std::size_t num_members)
	{
		return this;
	}

	Parser* GobblingParser::beginArray()
	{
		return this;
	}

	Parser* GobblingParser::endArray(std::size_t num_elements)
	{
		return this;
	}


	StringConsumer::StringConsumer(Parser* ret, std::string& v)
		: v(v), ret(ret)
	{
	}

	Parser* StringConsumer::parseString(std::string&& str)
	{
		v = std::move(str);
		return ret;
	}


	BoolConsumer::BoolConsumer(Parser* ret, bool& v)
		: v(v), ret(ret)
	{
	}

	Parser* BoolConsumer::parseBool(bool b)
	{
		v = b;
		return ret;
	}


	ColorConsumer::ColorConsumer(Parser* ret, std::uint32_t& v)
		: v(v), ret(ret)
	{
	}

	Parser* ColorConsumer::parseString(std::string&& str)
	{
		if (str[0] != '#')
			throw parse_error("color literal must start with #");

		char* end_ptr;
		v = std::strtoul(&str[0] + 1, &end_ptr, 16);

		if (end_ptr != &str[0] + str.length())
			throw parse_error(str, " is not a valid color literal");

		return ret;
	}


	void read(Parser& parser, std::istream& file)
	{
		rapidjson::IStreamWrapper filestream(file);
		rapidjson::EncodedInputStream<rapidjson::UTF8<>, rapidjson::IStreamWrapper> stream(filestream);

		rapidjson::Reader reader;
		ConfigHandler handler(parser);
		if (!reader.Parse(stream, handler))
			throw parse_error(GetParseError_En(reader.GetParseErrorCode()));
	}

	void read(Parser& parser, const std::filesystem::path& file)
	{
		std::ifstream stream(file, std::ios::binary);

		if (!stream)
			throw std::runtime_error("failed to open config file");

		read(parser, stream);
	}
}
