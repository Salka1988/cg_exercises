


#ifndef INCLUDED_FRAMEWORK_CONFIG
#define INCLUDED_FRAMEWORK_CONFIG

#pragma once

#include <cstdint>
#include <utility>
#include <optional>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>

#include <framework/math/vector.h>
#include <framework/math/matrix.h>

#include <framework/utils/io.h>


namespace config
{
	class parse_error : public std::runtime_error
	{
		template <typename... Args>
		static std::string buildString(Args&&... args)
		{
			std::ostringstream builder;
			concat(builder, std::forward<Args>(args)...);
			return builder.str();
		}

	public:
		using std::runtime_error::runtime_error;

		template <typename... Args>
		parse_error(Args&&... args)
			: runtime_error(buildString(std::forward<Args>(args)...))
		{
		}
	};


	struct Parser
	{
		virtual Parser* parseNull() = 0;
		virtual Parser* parseBool(bool b) = 0;
		virtual Parser* parseNumber(int i) = 0;
		virtual Parser* parseNumber(unsigned int i) = 0;
		virtual Parser* parseNumber(std::int64_t i) = 0;
		virtual Parser* parseNumber(std::uint64_t i) = 0;
		virtual Parser* parseNumber(double d) = 0;
		virtual Parser* parseString(std::string&& str) = 0;
		virtual Parser* parseKey(std::string&& str) = 0;
		virtual Parser* beginObject() = 0;
		virtual Parser* endObject(std::size_t num_members) = 0;
		virtual Parser* beginArray() = 0;
		virtual Parser* endArray(std::size_t num_elements) = 0;

	protected:
		Parser() = default;
		Parser(Parser&&) = default;
		Parser(const Parser&) = default;
		Parser& operator =(Parser&&) = default;
		Parser& operator =(const Parser&) = default;
		~Parser() = default;
	};

	class RejectingParser : public virtual Parser
	{
	public:
		Parser* parseNull() override;
		Parser* parseBool(bool b) override;
		Parser* parseNumber(int i) override;
		Parser* parseNumber(unsigned int i) override;
		Parser* parseNumber(std::int64_t i) override;
		Parser* parseNumber(std::uint64_t i) override;
		Parser* parseNumber(double d) override;
		Parser* parseString(std::string&& str) override;
		Parser* parseKey(std::string&& str) override;
		Parser* beginObject() override;
		Parser* endObject(std::size_t num_members) override;
		Parser* beginArray() override;
		Parser* endArray(std::size_t num_elements) override;
	};

	class GobblingParser : public virtual Parser
	{
	public:
		Parser* parseNull() override;
		Parser* parseBool(bool b) override;
		Parser* parseNumber(int i) override;
		Parser* parseNumber(unsigned int i) override;
		Parser* parseNumber(std::int64_t i) override;
		Parser* parseNumber(std::uint64_t i) override;
		Parser* parseNumber(double d) override;
		Parser* parseString(std::string&& str) override;
		Parser* parseKey(std::string&& str) override;
		Parser* beginObject() override;
		Parser* endObject(std::size_t num_members) override;
		Parser* beginArray() override;
		Parser* endArray(std::size_t num_elements) override;
	};


	class StringConsumer : public config::RejectingParser
	{
		std::string& v;
		Parser* ret;

	public:
		StringConsumer(Parser* ret, std::string& v);

		Parser* parseString(std::string&& str) override;
	};

	template <typename T>
	class NumberConsumer : public config::RejectingParser
	{
		T& v;
		Parser* ret;

	public:
		NumberConsumer(Parser* ret, T& v)
			: v(v), ret(ret)
		{
		}

		Parser* parseNumber(int i) override
		{
			v = static_cast<T>(i);
			return ret;
		}

		Parser* parseNumber(unsigned int i) override
		{
			v = static_cast<T>(i);
			return ret;
		}

		Parser* parseNumber(std::int64_t i) override
		{
			v = static_cast<T>(i);
			return ret;
		}

		Parser* parseNumber(std::uint64_t i) override
		{
			v = static_cast<T>(i);
			return ret;
		}

		Parser* parseNumber(double d) override
		{
			v = static_cast<T>(d);
			return ret;
		}
	};

	class BoolConsumer : public config::RejectingParser
	{
		bool& v;
		Parser* ret;

	public:
		BoolConsumer(Parser* ret, bool& v);

		Parser* parseBool(bool b) override;
	};

	template <typename T, int D>
	struct array_to_vector_converter;

	template <typename T>
	struct array_to_vector_converter<T, 2>
	{
		math::vector<T, 2> operator ()(T(&a)[2]) const
		{
			return { a[0], a[1] };
		}
	};

	template <typename T>
	struct array_to_vector_converter<T, 3>
	{
		math::vector<T, 3> operator ()(T(&a)[3]) const
		{
			return { a[0], a[1], a[2] };
		}
	};

	template <typename T>
	struct array_to_vector_converter<T, 4>
	{
		math::vector<T, 4> operator ()(T(&a)[4]) const
		{
			return { a[0], a[1], a[2], a[3] };
		}
	};

	template <typename T, int D>
	class VectorConsumer : public config::RejectingParser
	{
		T elements[D];
		int next_element = 0;

		math::vector<T, D>& v;
		Parser* ret;

		void addElement(T v)
		{
			if (next_element >= D)
				throw parse_error("too many elements for ", D, "D vector (", next_element, " given)");
			elements[next_element++] = v;
		}

	public:
		VectorConsumer(Parser* ret, math::vector<T, D>& v)
			: v(v), ret(ret)
		{
		}

		Parser* parseNumber(int i) override
		{
			addElement(static_cast<T>(i));
			return this;
		}

		Parser* parseNumber(unsigned int i) override
		{
			addElement(static_cast<T>(i));
			return this;
		}

		Parser* parseNumber(std::int64_t i) override
		{
			addElement(static_cast<T>(i));
			return this;
		}

		Parser* parseNumber(std::uint64_t i) override
		{
			addElement(static_cast<T>(i));
			return this;
		}

		Parser* parseNumber(double d) override
		{
			addElement(static_cast<T>(d));
			return this;
		}

		Parser* endArray(std::size_t num_elements) override
		{
			if (next_element != D)
				throw parse_error("expected ", D, " list elements for ", D, "D vector, got ", next_element, " instead");
			v = array_to_vector_converter<T, D>()(elements);
			return ret;
		}
	};


	template <typename T, int M, int N>
	struct row_array_to_matrix_converter;

	template <typename T, int N>
	struct row_array_to_matrix_converter<T, 2, N>
	{
		auto operator ()(math::vector<T, N>(&r)[2]) const
		{
			return math::matrix_from_rows(r[0], r[1]);
		}
	};

	template <typename T, int N>
	struct row_array_to_matrix_converter<T, 3, N>
	{
		auto operator ()(math::vector<T, N>(&r)[3]) const
		{
			return math::matrix_from_rows(r[0], r[1], r[2]);
		}
	};

	template <typename T, int N>
	struct row_array_to_matrix_converter<T, 4, N>
	{
		auto operator ()(math::vector<T, N>(&r)[4]) const
		{
			return math::matrix_from_rows(r[0], r[1], r[2], r[3]);
		}
	};

	template <typename T, int M, int N>
	class MatrixConsumer : public config::RejectingParser
	{
		math::vector<T, N> rows[M];
		int next_row = 0;

		math::matrix<T, M, N>& v;
		Parser* ret;

		std::optional<VectorConsumer<T, N>> row_consumer;

	public:
		template <typename... Args>
		MatrixConsumer(config::Parser* ret, math::matrix<T, M, N>& v)
			: ret(ret), v(v)
		{
		}

		Parser* beginArray() override
		{
			return &row_consumer.emplace(this, rows[next_row++]);
		}

		Parser* endArray(std::size_t num_elements) override
		{
			if (next_row != M)
				throw parse_error("expected ", M, " rows for ", M, 'x', N, " matrix, got ", next_row, " instead");
			v = row_array_to_matrix_converter<T, M, N>()(rows);
			return ret;
		}
	};


	class ColorConsumer : public config::RejectingParser
	{
		std::uint32_t& v;
		Parser* ret;

	public:
		ColorConsumer(Parser* ret, std::uint32_t& v);

		Parser* parseString(std::string&& str) override;
	};


	template <typename Consumer>
	class ObjectParser : public config::RejectingParser
	{
		Consumer consumer;

	public:
		template <typename... Args>
		ObjectParser(config::Parser* ret, Args&&... args)
			: consumer(ret, std::forward<Args>(args)...)
		{
		}

		Parser* beginObject() override
		{
			return &consumer;
		}
	};

	template <typename Consumer>
	class RootObjectParser : public config::RejectingParser
	{
		Consumer consumer;

	public:
		template <typename... Args>
		RootObjectParser(Args&&... args)
			: consumer(this, std::forward<Args>(args)...)
		{
		}

		Parser* beginObject() override
		{
			return &consumer;
		}
	};

	template <typename Consumer>
	class ArrayParser : public config::RejectingParser
	{
		Consumer consumer;

	public:
		template <typename... Args>
		ArrayParser(config::Parser* ret, Args&&... args)
			: consumer(ret, std::forward<Args>(args)...)
		{
		}

		Parser* beginArray() override
		{
			return &consumer;
		}
	};


	template <typename T, int D>
	using VectorParser = ArrayParser<VectorConsumer<T, D>>;

	template <typename T, int M, int N>
	using MatrixParser = ArrayParser<MatrixConsumer<T, M, N>>;


	void read(Parser& parser, std::istream& file);
	void read(Parser& parser, const std::filesystem::path& file);
}

#endif  // INCLUDED_FRAMEWORK_CONFIG
