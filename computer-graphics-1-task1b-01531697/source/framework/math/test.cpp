


#include <typeinfo>
#include <exception>
#include <random>
#include <iostream>

#include <framework/math/math.h>
#include <framework/math/vector.h>
#include <framework/math/matrix.h>


namespace
{
	template <typename T, typename G>
	T randn(G&& g)
	{
		std::normal_distribution<T> dist;
		return dist(g);
	}

	template <typename F, int D>
	struct vrandn;

	template <typename F>
	struct vrandn<F, 2>
	{
		template <typename G>
		math::vector<F, 2> operator()(G&& g)
		{
			return { randn<F>(g), randn<F>(g) };
		}
	};

	template <typename F>
	struct vrandn<F, 3>
	{
		template <typename G>
		math::vector<F, 3> operator()(G&& g)
		{
			return { randn<F>(g), randn<F>(g), randn<F>(g) };
		}
	};

	template <typename F>
	struct vrandn<F, 4>
	{
		template <typename G>
		math::vector<F, 4> operator()(G&& g)
		{
			return { randn<F>(g), randn<F>(g), randn<F>(g), randn<F>(g) };
		}
	};

	template <typename T, int M, int N>
	class mrandn
	{
		template <typename G, typename... Rows>
		math::matrix<T, M, N> gen(G&& g, Rows&&... rows)
		{
			if constexpr (sizeof...(Rows) == M)
				return math::matrix_from_rows(std::forward<Rows>(rows)...);
			else
				return gen(std::forward<G>(g), std::forward<Rows>(rows)..., vrandn<T, N> {}(g));
		}

	public:
		template <typename G>
		math::matrix<T, M, N> operator()(G&& g)
		{
			return gen(std::forward<G>(g));
		}
	};

	template <typename T>
	constexpr auto relative_difference(T a, T b)
	{
		if (a == math::zero<T> || b == math::zero<T>)
			return math::abs(a - b);
		return math::abs(a - b) / math::max(math::abs(a), math::abs(b));
	}

	template <typename F, int D>
	struct testCreation
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			vrandn<F, D> rand;

			auto v1 = rand(g);
			auto v2 = math::vector<F, D>(v1);
			auto v3 = v1;

			if (v1 != v2 || v2 != v3 || v3 != v1)
				return false;

			v1 = rand(g);
			if (v1 == v2 || v1 == v3)
				return false;

			return true;
		}
	};

	template <typename F, int D>
	struct testAddition
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			vrandn<F, D> rand;

			auto v1 = rand(g);
			auto v2 = rand(g);
			auto v3 = v1 + v2;

			for (int i = 0; i < D; ++i)
			{
				auto v1_ptr = &v1.x;
				auto v2_ptr = &v2.x;
				auto v3_ptr = &v3.x;
				if (v3_ptr[i] != v1_ptr[i] + v2_ptr[i])
					return false;
			}
			return true;
		}
	};

	template <typename F, int D>
	struct testSubtraction
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			vrandn<F, D> rand;

			auto v1 = rand(g);
			auto v2 = rand(g);
			auto v3 = v1 - v2;
			auto v4 = v2 - v1;

			for (int i = 0; i < D; ++i)
			{
				auto v1_ptr = &v1.x;
				auto v2_ptr = &v2.x;
				auto v3_ptr = &v3.x;
				auto v4_ptr = &v4.x;
				if (v3_ptr[i] != v1_ptr[i] - v2_ptr[i] ||
					 v3_ptr[i] != -v4_ptr[i])
					return false;
			}
			return true;
		}
	};

	template <typename F, int D>
	struct testMultiplication
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			float f = randn<F>(g);

			vrandn<F, D> rand;

			auto v1 = rand(g);
			auto v2 = f * v1;

			auto v3 = v1 * f;
			for (int i = 0; i < D; ++i)
			{
				auto v1_ptr = &v1.x;
				auto v2_ptr = &v2.x;
				auto v3_ptr = &v3.x;
				if (v1_ptr[i] * f != v2_ptr[i] ||
					 v1_ptr[i] * f != v3_ptr[i])
					return false;
			}
			return true;
		}
	};

	template <typename F, int D>
	struct testDotProduct
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			vrandn<F, D> rand;

			auto v1 = rand(g);
			auto v2 = rand(g);
			auto dot_product = dot(v1, v2);

			auto self_dot_1 = dot(v1, v1);
			if (relative_difference(self_dot_1, length(v1) * length(v1)) > 0.0001f)
				return false;

			auto self_dot_2 = dot(v2, v2);
			if (relative_difference(self_dot_2, length(v2) * length(v2)) > 0.0001f)
				return false;

			F expected_dot = 0;
			for (int i = 0; i < D; ++i)
			{
				auto v1_ptr = &v1.x;
				auto v2_ptr = &v2.x;
				expected_dot += v1_ptr[i] * v2_ptr[i];
			}
			if (expected_dot != dot_product)
				return false;
			return true;
		}
	};

	template <typename F, int D>
	struct testCrossProduct
	{
		static_assert(D == 3);

		template <typename G>
		bool operator()(G&& g) const
		{
			vrandn<F, D> rand;

			auto v1 = rand(g);
			auto v2 = rand(g);
			auto v12 = cross(v1, v2);
			auto v21 = cross(v2, v1);

			if (relative_difference(dot(v12, v1), 0.0f) > 0.000001f ||
				 relative_difference(dot(v12, v2), 0.0f) > 0.000001f)
				return false;

			if (relative_difference(length(v12 + v21), 0.0f) > 0.000001f)
				return false;

			if (v12.x != v1.y*v2.z - v1.z*v2.y ||
				 v12.y != v1.z*v2.x - v1.x*v2.z ||
				 v12.z != v1.x*v2.y - v1.y*v2.x )
				return false;
			return true;
		}
	};

	template <typename T, int M, int N>
	struct testMatrixVector
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			using matrix_t = math::matrix<T, M, N>;

			vrandn<T, N> rand;

			auto v1 = rand(g);

			auto I = math::identity<matrix_t>;

			if (I * v1 != v1)
				return false;

			return true;
		}
	};

	template <typename T, int M, int N>
	struct testMatrixTranspose
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			mrandn<T, M, N> rand;

			auto M = rand(g);

			if (transpose(transpose(M)) != M)
				return false;

			return true;
		}
	};

	template <typename T, int M, int N>
	struct testMatrixInverse
	{
		template <typename G>
		bool operator()(G&& g) const
		{
			using matrix_t = math::matrix<T, M, N>;

			mrandn<T, M, N> rand;

			auto M = rand(g);

			auto e = (M * inverse(M)) * math::vector<T, N>(math::one<T>);

			return relative_difference(dot(e, e), 1.0f) < 0.000001f;
		}
	};

	template <typename F, typename G>
	int runTest(F&& f, G&& g)
	{
		int failed = 0;
		if (!f(std::move(g)))
		{
			failed++;
			std::cout << "test FAIL: ";
		}
		else
			std::cout << "test pass: ";
		std::cout << typeid(F).name() << std::endl;
		return failed;
	}

	template <typename F, int D>
	int testVectors()
	{
		std::mt19937 g(42);

		auto failed = runTest(testCreation<F, D> {}, g);
		failed += runTest(testAddition<F, D> {}, g);
		failed += runTest(testSubtraction<F, D> {}, g);
		failed += runTest(testMultiplication<F, D> {}, g);
		failed += runTest(testDotProduct<F, D> {}, g);
		if constexpr (D == 3)
			failed += runTest(testCrossProduct<F, D> {}, g);

		std::cout << "vector<" << typeid(F).name() << ", " << D <<"> failed " << failed << " tests" << std::endl << std::endl;
		return failed;
	}

	template <typename T, int M, int N>
	int testMatrices()
	{
		std::mt19937 g(42);

		auto failed = runTest(testMatrixVector<T, M, N> {}, g);
		failed += runTest(testMatrixTranspose<T, M, N> {}, g);
		failed += runTest(testMatrixInverse<T, M, N> {}, g);

		std::cout << "matrix<" << typeid(T).name() << ", " << M << ", " << N << "> failed " << failed << " tests" << std::endl << std::endl;
		return failed;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		std::cout << "pi is " << math::pi<float> << std::endl;

		int failed = 0;
		failed += testVectors<float, 2>();
		failed += testVectors<float, 3>();
		failed += testVectors<float, 4>();

		failed += testMatrices<float, 2, 2>();
		failed += testMatrices<float, 3, 3>();
		failed += testMatrices<float, 4, 4>();

		std::cout << "\nIN TOTAL FAILED " << failed << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		return -1;
	}
	catch (...)
	{
		std::cerr << "ERROR: unknown exception\n";
		return -128;
	}

	return 0;
}
