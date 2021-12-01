


#ifndef INCLUDED_FRAMEWORK_MATH_MATRIX
#define INCLUDED_FRAMEWORK_MATH_MATRIX

#pragma once

#include "vector.h"


namespace math
{
	template <typename T, int M, int N>
	struct matrix;

	template <typename T>
	struct matrix<T, 2, 2>
	{
		T _11, _12,
		  _21, _22;

		matrix() = default;

		constexpr matrix(T m11, T m12, T m21, T m22)
			: _11(m11), _12(m12), _21(m21), _22(m22)
		{
		}

		constexpr explicit matrix(T a)
			: matrix { a, a, a, a }
		{
		}
	};

	template <typename T>
	constexpr const vector<T, 2> row_1(const matrix<T, 2, 2>& M) { return { M._11, M._12 }; }
	template <typename T>
	constexpr vector<T&, 2> row_1(matrix<T, 2, 2>& M) { return { M._11, M._12 }; }

	template <typename T>
	constexpr const vector<T, 2> row_2(const matrix<T, 2, 2>& M) { return { M._21, M._22 }; }
	template <typename T>
	constexpr vector<T&, 2> row_2(matrix<T, 2, 2>& M) { return { M._21, M._22 }; }

	template <typename T>
	constexpr const vector<T, 2> column_1(const matrix<T, 2, 2>& M) { return { M._11, M._21 }; }
	template <typename T>
	constexpr vector<T&, 2> column_1(matrix<T, 2, 2>& M) { return { M._11, M._21 }; }

	template <typename T>
	constexpr const vector<T, 2> column_2(const matrix<T, 2, 2>& M) { return { M._12, M._22 }; }
	template <typename T>
	constexpr vector<T&, 2> column_2(matrix<T, 2, 2>& M) { return { M._12, M._22 }; }

	template <typename T>
	constexpr const matrix<T, 2, 2> matrix_from_rows(const vector<T, 2>& r_1, const vector<T, 2>& r_2)
	{
		return { r_1.x, r_1.y, r_2.x, r_2.y };
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> matrix_from_columns(const vector<T, 2>& c_1, const vector<T, 2>& c_2)
	{
		return { c_1.x, c_2.x, c_1.y, c_2.y };
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> operator -(const matrix<T, 2, 2>& M)
	{
		return { -M._11, -M._12, -M._21, -M._22 };
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> operator +(const matrix<T, 2, 2>& A, const matrix<T, 2, 2>& B)
	{
		return { A._11 + B._11, A._12 + B._12, A._21 + B._21, A._22 + B._22 };
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> operator -(const matrix<T, 2, 2>& A, const matrix<T, 2, 2>& B)
	{
		return { A._11 - B._11, A._12 - B._12, A._21 - B._21, A._22 - B._22 };
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> operator *(const matrix<T, 2, 2>& A, const matrix<T, 2, 2>& B)
	{
		return { dot(row_1(A), column_1(B)), dot(row_1(A), column_2(B)),
		         dot(row_2(A), column_1(B)), dot(row_2(A), column_2(B)) };
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> operator *(T s, const matrix<T, 2, 2>& M)
	{
		return { s * M._11, s * M._12, s * M._21, s * M._22 };
	}

	template <typename T>
	constexpr const vector<T, 2> operator *(const matrix<T, 2, 2>& M, const vector<T, 2>& v)
	{
		return { dot(row_1(M), v), dot(row_2(M), v) };
	}

	template <typename T>
	constexpr bool operator ==(const matrix<T, 2, 2>& A, const matrix<T, 2, 2>& B)
	{
		return A._11 == B._11 && A._12 == B._12 && A._21 == B._21 && A._22 == B._22;
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> transpose(const matrix<T, 2, 2>& M)
	{
		return matrix_from_columns(row_1(M), row_2(M));
	}

	template <typename T>
	constexpr T det(const matrix<T, 2, 2>& M)
	{
		return M._11 * M._22 - M._21 * M._12;
	}

	template <typename T>
	constexpr const matrix<T, 2, 2> adj(const matrix<T, 2, 2>& M)
	{
		return { M._22, -M._12, -M._21, M._11 };
	}

	template <typename T>
	constexpr auto trace(const matrix<T, 2, 2>& M)
	{
		return M._11 + M._22;
	}


	template <typename T>
	struct matrix<T, 3U, 3U>
	{
		T _11, _12, _13,
		  _21, _22, _23,
		  _31, _32, _33;

		matrix() = default;

		constexpr matrix(T m11, T m12, T m13, T m21, T m22, T m23, T m31, T m32, T m33)
			: _11(m11), _12(m12), _13(m13), _21(m21), _22(m22), _23(m23), _31(m31), _32(m32), _33(m33)
		{
		}

		constexpr explicit matrix(T a)
			: matrix { a, a, a, a, a, a, a, a, a }
		{
		}
	};

	template <typename T>
	constexpr const vector<T, 3> row_1(const matrix<T, 3, 3>& M) { return { M._11, M._12, M._13 }; }
	template <typename T>
	constexpr vector<T&, 3> row_1(matrix<T, 3, 3>& M) { return { M._11, M._12, M._13 }; }

	template <typename T>
	constexpr const vector<T, 3> row_2(const matrix<T, 3, 3>& M) { return { M._21, M._22, M._23 }; }
	template <typename T>
	constexpr vector<T&, 3> row_2(matrix<T, 3, 3>& M) { return { M._21, M._22, M._23 }; }

	template <typename T>
	constexpr const vector<T, 3> row_3(const matrix<T, 3, 3>& M) { return { M._31, M._32, M._33 }; }
	template <typename T>
	constexpr vector<T&, 3> row_3(matrix<T, 3, 3>& M) { return { M._31, M._32, M._33 }; }

	template <typename T>
	constexpr const vector<T, 3> column_1(const matrix<T, 3, 3>& M) { return { M._11, M._21, M._31 }; }
	template <typename T>
	constexpr vector<T&, 3> column_1(matrix<T, 3, 3>& M) { return { M._11, M._21, M._31 }; }

	template <typename T>
	constexpr const vector<T, 3> column_2(const matrix<T, 3, 3>& M) { return { M._12, M._22, M._32 }; }
	template <typename T>
	constexpr vector<T&, 3> column_2(matrix<T, 3, 3>& M) { return { M._12, M._22, M._32 }; }

	template <typename T>
	constexpr const vector<T, 3> column_3(const matrix<T, 3, 3>& M) { return { M._13, M._23, M._33 }; }
	template <typename T>
	constexpr vector<T&, 3> column_3(matrix<T, 3, 3>& M) { return { M._13, M._23, M._33 }; }

	template <typename T>
	constexpr const matrix<T, 3, 3> matrix_from_rows(const vector<T, 3>& r_1, const vector<T, 3>& r_2, const vector<T, 3>& r_3)
	{
		return { r_1.x, r_1.y, r_1.z, r_2.x, r_2.y, r_2.z, r_3.x, r_3.y, r_3.z };
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> matrix_from_columns(const vector<T, 3>& c_1, const vector<T, 3>& c_2, const vector<T, 3>& c_3)
	{
		return { c_1.x, c_2.x, c_3.x, c_1.y, c_2.y, c_3.y, c_1.z, c_2.z, c_3.z };
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> operator -(const matrix<T, 3, 3>& M)
	{
		return { -M._11, -M._12, -M._13, -M._21, -M._22, -M._23, -M._31, -M._32, -M._33 };
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> operator +(const matrix<T, 3, 3>& A, const matrix<T, 3, 3>& B)
	{
		return { A._11 + B._11, A._12 + B._12, A._13 + B._13, A._21 + B._21, A._22 + B._22, A._23 + B._23, A._31 + B._31, A._32 + B._32, A._33 + B._33 };
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> operator -(const matrix<T, 3, 3>& A, const matrix<T, 3, 3>& B)
	{
		return { A._11 - B._11, A._12 - B._12, A._13 - B._13, A._21 - B._21, A._22 - B._22, A._23 - B._23, A._31 - B._31, A._32 - B._32, A._33 - B._33 };
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> operator *(const matrix<T, 3, 3>& A, const matrix<T, 3, 3>& B)
	{
		return { dot(row_1(A), column_1(B)), dot(row_1(A), column_2(B)), dot(row_1(A), column_3(B)),
		         dot(row_2(A), column_1(B)), dot(row_2(A), column_2(B)), dot(row_2(A), column_3(B)),
		         dot(row_3(A), column_1(B)), dot(row_3(A), column_2(B)), dot(row_3(A), column_3(B)) };
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> operator *(float s, const matrix<T, 3, 3>& M)
	{
		return { s * M._11, s * M._12, s * M._13, s * M._21, s * M._22, s * M._23, s * M._31, s * M._32, s * M._33 };
	}

	template <typename T>
	constexpr const vector<T, 3> operator *(const matrix<T, 3, 3>& M, const vector<T, 3>& v)
	{
		return { dot(row_1(M), v), dot(row_2(M), v), dot(row_3(M), v) };
	}

	template <typename T>
	constexpr bool operator ==(const matrix<T, 3, 3>& A, const matrix<T, 3, 3>& B)
	{
		return A._11 == B._11 && A._12 == B._12 && A._13 == B._13 && A._21 == B._21 && A._22 == B._22 && A._23 == B._23 && A._31 == B._31 && A._32 == B._32 && A._33 == B._33;
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> transpose(const matrix<T, 3, 3>& M)
	{
		return matrix_from_columns(row_1(M), row_2(M), row_3(M));
	}

	template <typename T>
	constexpr T det(const matrix<T, 3, 3>& M)
	{
		return M._11 * det(matrix<T, 2, 2>(M._22, M._23, M._32, M._33)) -
		       M._12 * det(matrix<T, 2, 2>(M._21, M._23, M._31, M._33)) +
		       M._13 * det(matrix<T, 2, 2>(M._21, M._22, M._31, M._32));
	}

	template <typename T>
	constexpr const matrix<T, 3, 3> adj(const matrix<T, 3, 3>& M)
	{
		return transpose(
			matrix<T, 3, 3> {
				 det(matrix<T, 2, 2>(M._22, M._23, M._32, M._33)),
				-det(matrix<T, 2, 2>(M._21, M._23, M._31, M._33)),
				 det(matrix<T, 2, 2>(M._21, M._22, M._31, M._32)),

				-det(matrix<T, 2, 2>(M._12, M._13, M._32, M._33)),
				 det(matrix<T, 2, 2>(M._11, M._13, M._31, M._33)),
				-det(matrix<T, 2, 2>(M._11, M._12, M._31, M._32)),

				 det(matrix<T, 2, 2>(M._12, M._13, M._22, M._23)),
				-det(matrix<T, 2, 2>(M._11, M._13, M._21, M._23)),
				 det(matrix<T, 2, 2>(M._11, M._12, M._21, M._22))
			}
		);
	}

	template <typename T>
	constexpr auto trace(const matrix<T, 3, 3>& M)
	{
		return M._11 + M._22 + M._33;
	}


	template <typename T>
	struct matrix<T, 4U, 4U>
	{
		T _11, _12, _13, _14,
		  _21, _22, _23, _24,
		  _31, _32, _33, _34,
		  _41, _42, _43, _44;

		matrix() = default;

		constexpr matrix(T m11, T m12, T m13, T m14, T m21, T m22, T m23, T m24, T m31, T m32, T m33, T m34, T m41, T m42, T m43, T m44)
			: _11(m11), _12(m12), _13(m13), _14(m14), _21(m21), _22(m22), _23(m23), _24(m24), _31(m31), _32(m32), _33(m33), _34(m34), _41(m41), _42(m42), _43(m43), _44(m44)
		{
		}

		constexpr explicit matrix(T a)
			: matrix { a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a }
		{
		}
	};

	template <typename T>
	constexpr const vector<T, 4> row_1(const matrix<T, 4, 4>& M) { return { M._11, M._12, M._13, M._14 }; }
	template <typename T>
	constexpr vector<T&, 4> row_1(matrix<T, 4, 4>& M) { return { M._11, M._12, M._13, M._14 }; }

	template <typename T>
	constexpr const vector<T, 4> row_2(const matrix<T, 4, 4>& M) { return { M._21, M._22, M._23, M._24 }; }
	template <typename T>
	constexpr vector<T&, 4> row_2(matrix<T, 4, 4>& M) { return { M._21, M._22, M._23, M._24 }; }

	template <typename T>
	constexpr const vector<T, 4> row_3(const matrix<T, 4, 4>& M) { return { M._31, M._32, M._33, M._34 }; }
	template <typename T>
	constexpr vector<T&, 4> row_3(matrix<T, 4, 4>& M) { return { M._31, M._32, M._33, M._34 }; }

	template <typename T>
	constexpr const vector<T, 4> row_4(const matrix<T, 4, 4>& M) { return { M._41, M._42, M._43, M._44 }; }
	template <typename T>
	constexpr vector<T&, 4> row_4(matrix<T, 4, 4>& M) { return { M._41, M._42, M._43, M._44 }; }

	template <typename T>
	constexpr const vector<T, 4> column_1(const matrix<T, 4, 4>& M) { return { M._11, M._21, M._31, M._41 }; }
	template <typename T>
	constexpr vector<T&, 4> column_1(matrix<T, 4, 4>& M) { return { M._11, M._21, M._31, M._41 }; }

	template <typename T>
	constexpr const vector<T, 4> column_2(const matrix<T, 4, 4>& M) { return { M._12, M._22, M._32, M._42 }; }
	template <typename T>
	constexpr vector<T&, 4> column_2(matrix<T, 4, 4>& M) { return { M._12, M._22, M._32, M._42 }; }

	template <typename T>
	constexpr const vector<T, 4> column_3(const matrix<T, 4, 4>& M) { return { M._13, M._23, M._33, M._43 }; }
	template <typename T>
	constexpr vector<T&, 4> column_3(matrix<T, 4, 4>& M) { return { M._13, M._23, M._33, M._43 }; }

	template <typename T>
	constexpr const vector<T, 4> column_4(const matrix<T, 4, 4>& M) { return { M._14, M._24, M._34, M._44 }; }
	template <typename T>
	constexpr vector<T&, 4> column_4(matrix<T, 4, 4>& M) { return { M._14, M._24, M._34, M._44 }; }

	template <typename T>
	constexpr const matrix<T, 4, 4> matrix_from_rows(const vector<T, 4>& r_1, const vector<T, 4>& r_2, const vector<T, 4>& r_3, const vector<T, 4>& r_4)
	{
		return { r_1.x, r_1.y, r_1.z, r_1.w, r_2.x, r_2.y, r_2.z, r_2.w, r_3.x, r_3.y, r_3.z, r_3.w, r_4.x, r_4.y, r_4.z, r_4.w };
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> matrix_from_columns(const vector<T, 4>& c_1, const vector<T, 4>& c_2, const vector<T, 4>& c_3, const vector<T, 4>& c_4)
	{
		return { c_1.x, c_2.x, c_3.x, c_4.x, c_1.y, c_2.y, c_3.y, c_4.y, c_1.z, c_2.z, c_3.z, c_4.z, c_1.w, c_2.w, c_3.w, c_4.w };
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> operator -(const matrix<T, 4, 4>& M)
	{
		return { -M._11, -M._12, -M._13, -M._14, -M._21, -M._22, -M._23, -M._24, -M._31, -M._32, -M._33, -M._34, -M._41, -M._42, -M._43, -M._44 };
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> operator +(const matrix<T, 4, 4>& A, const matrix<T, 4, 4>& B)
	{
		return { A._11 + B._11, A._12 + B._12, A._13 + B._13, A._14 + B._14, A._21 + B._21, A._22 + B._22, A._23 + B._23, A._24 + B._24, A._31 + B._31, A._32 + B._32, A._33 + B._33, A._34 + B._34, A._41 + B._41, A._42 + B._42, A._43 + B._43, A._44 + B._44 };
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> operator -(const matrix<T, 4, 4>& A, const matrix<T, 4, 4>& B)
	{
		return { A._11 - B._11, A._12 - B._12, A._13 - B._13, A._14 - B._14, A._21 - B._21, A._22 - B._22, A._23 - B._23, A._24 - B._24, A._31 - B._31, A._32 - B._32, A._33 - B._33, A._34 - B._34, A._41 - B._41, A._42 - B._42, A._43 - B._43, A._44 - B._44 };
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> operator *(const matrix<T, 4, 4>& A, const matrix<T, 4, 4>& B)
	{
		return { dot(row_1(A), column_1(B)), dot(row_1(A), column_2(B)), dot(row_1(A), column_3(B)), dot(row_1(A), column_4(B)),
		         dot(row_2(A), column_1(B)), dot(row_2(A), column_2(B)), dot(row_2(A), column_3(B)), dot(row_2(A), column_4(B)),
		         dot(row_3(A), column_1(B)), dot(row_3(A), column_2(B)), dot(row_3(A), column_3(B)), dot(row_3(A), column_4(B)),
		         dot(row_4(A), column_1(B)), dot(row_4(A), column_2(B)), dot(row_4(A), column_3(B)), dot(row_4(A), column_4(B)) };
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> operator *(T s, const matrix<T, 4, 4>& M)
	{
		return { s * M._11, s * M._12, s * M._13, s * M._14, s * M._21, s * M._22, s * M._23, s * M._24, s * M._31, s * M._32, s * M._33, s * M._34, s * M._41, s * M._42, s * M._43, s * M._44 };
	}

	template <typename T>
	constexpr const vector<T, 4> operator *(const matrix<T, 4, 4>& M, const vector<T, 4>& v)
	{
		return { dot(row_1(M), v), dot(row_2(M), v), dot(row_3(M), v), dot(row_4(M), v) };
	}

	template <typename T>
	constexpr bool operator ==(const matrix<T, 4, 4>& A, const matrix<T, 4, 4>& B)
	{
		return A._11 == B._11 && A._12 == B._12 && A._13 == B._13 && A._14 == B._14 && A._21 == B._21 && A._22 == B._22 && A._23 == B._23 && A._24 == B._24 && A._31 == B._31 && A._32 == B._32 && A._33 == B._33 && A._34 == B._34 && A._41 == B._41 && A._42 == B._42 && A._43 == B._43 && A._44 == B._44;
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> transpose(const matrix<T, 4, 4>& M)
	{
		return matrix_from_columns(row_1(M), row_2(M), row_3(M), row_4(M));
	}

	template <typename T>
	constexpr T det(const matrix<T, 4, 4>& M)
	{
		return M._11 * det(matrix<T, 3, 3>(M._22, M._23, M._24, M._32, M._33, M._34, M._42, M._43, M._44)) -
		       M._12 * det(matrix<T, 3, 3>(M._21, M._23, M._24, M._31, M._33, M._34, M._41, M._43, M._44)) +
		       M._13 * det(matrix<T, 3, 3>(M._21, M._22, M._24, M._31, M._32, M._34, M._41, M._42, M._44)) -
		       M._14 * det(matrix<T, 3, 3>(M._21, M._22, M._23, M._31, M._32, M._33, M._41, M._42, M._43));
	}

	template <typename T>
	constexpr const matrix<T, 4, 4> adj(const matrix<T, 4, 4>& M)
	{
		return transpose(
			matrix<T, 4, 4> {
				 det(matrix<T, 3, 3>(M._22, M._23, M._24, M._32, M._33, M._34, M._42, M._43, M._44)),
				-det(matrix<T, 3, 3>(M._21, M._23, M._24, M._31, M._33, M._34, M._41, M._43, M._44)),
				 det(matrix<T, 3, 3>(M._21, M._22, M._24, M._31, M._32, M._34, M._41, M._42, M._44)),
				-det(matrix<T, 3, 3>(M._21, M._22, M._23, M._31, M._32, M._33, M._41, M._42, M._43)),

				-det(matrix<T, 3, 3>(M._12, M._13, M._14, M._32, M._33, M._34, M._42, M._43, M._44)),
				 det(matrix<T, 3, 3>(M._11, M._13, M._14, M._31, M._33, M._34, M._41, M._43, M._44)),
				-det(matrix<T, 3, 3>(M._11, M._12, M._14, M._31, M._32, M._34, M._41, M._42, M._44)),
				 det(matrix<T, 3, 3>(M._11, M._12, M._13, M._31, M._32, M._33, M._41, M._42, M._43)),

				 det(matrix<T, 3, 3>(M._12, M._13, M._14, M._22, M._23, M._24, M._42, M._43, M._44)),
				-det(matrix<T, 3, 3>(M._11, M._13, M._14, M._21, M._23, M._24, M._41, M._43, M._44)),
				 det(matrix<T, 3, 3>(M._11, M._12, M._14, M._21, M._22, M._24, M._41, M._42, M._44)),
				-det(matrix<T, 3, 3>(M._11, M._12, M._13, M._21, M._22, M._23, M._41, M._42, M._43)),

				-det(matrix<T, 3, 3>(M._12, M._13, M._14, M._22, M._23, M._24, M._32, M._33, M._34)),
				 det(matrix<T, 3, 3>(M._11, M._13, M._14, M._21, M._23, M._24, M._31, M._33, M._34)),
				-det(matrix<T, 3, 3>(M._11, M._12, M._14, M._21, M._22, M._24, M._31, M._32, M._34)),
				 det(matrix<T, 3, 3>(M._11, M._12, M._13, M._21, M._22, M._23, M._31, M._32, M._33))
			}
		);
	}

	template <typename T>
	constexpr auto trace(const matrix<T, 4, 4>& M)
	{
		return M._11 + M._22 + M._33 + M._44;
	}


	template <typename T, int M, int N>
	constexpr const matrix<T, M, N> operator *(const matrix<T, M, N>& A, T s)
	{
		return s * A;
	}

	template <typename T, int M, int N>
	constexpr decltype(auto) operator +=(matrix<T, M, N>& A, const matrix<T, M, N>& B)
	{
		return A = A + B;
	}

	template <typename T, int M, int N>
	constexpr decltype(auto) operator -=(matrix<T, M, N>& A, const matrix<T, M, N>& B)
	{
		return A = A - B;
	}

	template <typename T, int M, int N>
	constexpr decltype(auto) operator *=(matrix<T, M, N>& A, T s)
	{
		return A = A * s;
	}

	template <typename T, int M, int N>
	constexpr bool operator !=(const matrix<T, M, N>& A, const matrix<T, M, N>& B)
	{
		return !(A == B);
	}


	template <typename T, int N>
	constexpr matrix<T, N, N> inverse(const matrix<T, N, N>& M)
	{
		// TODO: optimize; compute det using adj
		return rcp(det(M)) * adj(M);
	}


	using float2x2 = matrix<float, 2, 2>;
	using float3x3 = matrix<float, 3, 3>;
	using float4x4 = matrix<float, 4, 4>;


	template <typename T>
	constexpr T identity = one<T>;

	template <typename T>
	constexpr matrix<T, 2, 2> identity<matrix<T, 2, 2>> = { one<T>, zero<T>, zero<T>, one<T> };

	template <typename T>
	constexpr matrix<T, 3, 3> identity<matrix<T, 3, 3>> = { one<T>, zero<T>, zero<T>, zero<T>, one<T>, zero<T>, zero<T>, zero<T>, one<T> };

	template <typename T>
	constexpr matrix<T, 4, 4> identity<matrix<T, 4, 4>> = { one<T>, zero<T>, zero<T>, zero<T>, zero<T>, one<T>, zero<T>, zero<T>, zero<T>, zero<T>, one<T>, zero<T>, zero<T>, zero<T>, zero<T>, one<T> };
}

using math::float2x2;
using math::float3x3;
using math::float4x4;

#endif // INCLUDED_FRAMEWORK_MATH_MATRIX
