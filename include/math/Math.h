#pragma once

#include "Math.h"

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

namespace Rainy {

	constexpr float PI = 3.14159265358979f;

	float ToRadian(float angle);

	template<typename T>
	class Vector2
	{
	public:
		Vector2(T p_value = 0)
			:x(p_value), y(p_value)
		{	}

		Vector2(T p_x, T p_y)
			:x(p_x), y(p_y)
		{	}

		Vector2(Vector2<T> const& v)
			:x(v.x), y(v.y)
		{	}

		~Vector2() = default;

		float lenght() const
		{
			return std::sqrt(x * x + y * y);
		}

		float lengthQuad() const
		{
			return x * x + y * y;
		}

		T dot(Vector2<T> const& v) const
		{
			return x * v.x + y * v.y;
		}

		Vector2<T>& Mul(T const& scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		Vector2<T>& normalize()
		{
			float m_lenght = lenght();
			x /= m_lenght;
			y /= m_lenght;
			return *this;
		}

		Vector2<T>& add(Vector2<T> const & v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		std::string str() const
		{
			std::string s;
			s = "( " + std::to_string(x) + ", " + std::to_string(y) + " )";
			return s;
		}

		Vector2<T>& operator*=(T const& scalar)
		{
			return this->Mul(scalar);
		}

		Vector2<T>& operator+=(Vector2<T> const& v)
		{
			return this->add(v);
		}

		Vector2<T>& operator-=(Vector2<T> const& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		Vector2<T> operator-() const
		{
			return Vector2<T>{ -x, -y };
		}

		bool operator==(Vector2<T> const& v) const
		{
			return (x == v.x &&  y == v.y);
		}

		T& operator[](uint32_t i)
		{
			if (i >= 2)
				throw "Out of range in Vector";
			return ((T*)this)[i];
		}

		T& operator[](uint32_t i) const
		{
			if (i >= 2)
				throw "Out of range in Vector";
			return ((T*)this)[i];
		}

		Vector2<T>& operator=(Vector2<T> const& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

	public:
		T x;
		T y;
	};

	template<typename T>
	class Vector3
	{
	public:
		Vector3(T p_value = 0)
			:x(p_value), y(p_value), z(p_value)
		{	}

		Vector3(T p_x, T p_y, T p_z)
			:x(p_x), y(p_y), z(p_z)
		{	}

		Vector3(Vector2<T> const&  v)
			:x(v.x), y(v.y), z(0)
		{	}

		Vector3(Vector3<T> const&  v)
			:x(v.x), y(v.y), z(v.z)
		{	}

		~Vector3() = default;

		float length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		float lengthQuad() const
		{
			return x * x + y * y + z * z;
		}

		T dot(Vector3<T> const& v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		Vector3<T> cross(Vector3<T> const& v) const
		{
			return { y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x
			};
		}

		Vector3<T>& Mul(T const& scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		Vector3<T>& normalize()
		{
			float m_lenght = length();
			x /= m_lenght;
			y /= m_lenght;
			z /= m_lenght;
			return *this;
		}

		Vector3<T>& add(Vector3<T> const& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		std::string str() const
		{
			std::string s;
			s = "( " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + " )";
			return s;
		}

		Vector2<T> getVector2() const
		{
			return { x, y };
		}

		Vector3<T>& operator*=(T const& scalar)
		{
			return this->Mul(scalar);
		}

		Vector3<T>& operator+=(Vector3<T> const& v)
		{
			return this->add(v);
		}

		Vector3<T>& operator-()
		{
			x = -x;
			y = -y;
			z = -z;
			return *this;
		}

		bool operator==(Vector3<T> const& v) const
		{
			return (x == v.x &&  y == v.y && z == v.z);
		}

		T& operator[](uint32_t i)
		{
			if (i >= 3)
				throw "Out of range in Vector";
			return ((T*)this)[i];
		}

		T& operator[](uint32_t i) const
		{
			if (i >= 3)
				throw "Out of range in Vector";
			return ((T*)this)[i];
		}

		Vector3<T>& operator=(Vector2<T> const& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		Vector3<T>& operator=(Vector3<T> const& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}
	public:
		T x;
		T y;
		T z;
	};

	template<typename T>
	class Vector4
	{
	public:
		Vector4(T p_value = 0)
			:x(p_value), y(p_value), z(p_value), w(p_value)
		{	}

		Vector4(T p_x, T p_y, T p_z, T p_w)
			:x(p_x), y(p_y), z(p_z), w(p_w)
		{	}

		Vector4(Vector2<T>const& v, T const& z = 0, T const& w = 0)
			:x(v.x), y(v.y), z(0), w(w)
		{	}

		Vector4(Vector3<T>const&  v, T const& w = 0)
			:x(v.x), y(v.y), z(v.z), w(w)
		{	}

		Vector4(Vector4<T> const& v)
			:x(v.x), y(v.y), z(v.z), w(v.w)
		{	}

		~Vector4() = default;

		float lenght() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		float lengthQuad() const
		{
			return x * x + y * y + z * z + w * w;
		}

		T dot(Vector4<T> const& v) const
		{
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		Vector4<T>& Mul(T const& scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		Vector4<T>& normalize()
		{
			float m_lenght = lenght();
			x /= m_lenght;
			y /= m_lenght;
			z /= m_lenght;
			w /= m_lenght;
			return *this;
		}

		Vector4<T>& add(Vector4<T> const &  v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.z;
			return *this;
		}

		std::string str() const
		{
			std::string s;
			s = "( " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + " )";
			return s;
		}

		Vector2<T> getVector2() const
		{
			return { x, y };
		}

		Vector3<T> getVector3() const
		{
			return { x, y, z };
		}

		Vector4<T>& operator*=(T const& scalar)
		{
			return this->Mul(scalar);
		}

		Vector4<T>& operator+=(Vector4<T> const& v)
		{
			return this->add(v);
		}

		Vector4<T>& operator-()
		{
			x = -x;
			y = -y;
			z = -z;
			w = -w;
			return *this;
		}

		Vector4<T>& operator=(Vector2<T> const& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		Vector4<T>& operator=(Vector3<T> const& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		Vector4<T>& operator=(Vector4<T> const& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
			return *this;
		}

		bool operator==(Vector4<T> const& v) const
		{
			return (x == v.x &&  y == v.y && z == v.z && w == v.w);
		}

		T& operator[](uint32_t i)
		{
			if (i >= 4)
				throw "Out of range in Vector";
			return ((T*)this)[i];
		}

		T& operator[](uint32_t i) const
		{
			if (i >= 4)
				throw "Out of range in Vector";
			return ((T*)this)[i];
		}

	public:
		T x;
		T y;
		T z;
		T w;
	};

	template<typename T>
	class Matrix2 {
	public:
		Matrix2(
			Vector2<T> const& v0 = { 1, 0, 0 },
			Vector2<T> const& v1 = { 0, 1, 0 }
		)
			:m_data(new Vector2<T>[2])
		{
			m_data[0] = v0;
			m_data[1] = v1;
		}

		Matrix2(Matrix2<T> const& m)
			:m_data(new Vector2<T>[2])
		{
			m_data[0] = m[0];
			m_data[1] = m[1];
		}

		~Matrix2()
		{
			delete[] m_data;
		}

		Matrix2<T>& Mul(float const& scalar)
		{
			for (uint32_t i = 0; i < 2; ++i)
				m_data[i] *= scalar;
			return *this;
		}

		Matrix2<T>& Identity()
		{
			m_data[0] = { 1, 0 };
			m_data[1] = { 0, 1 };
			return *this;
		}

		Matrix2<T> Transparancy()
		{
			Vector2<T> v0 = m_data[0];
			Vector2<T> v1 = m_data[1];

			m_data[0] = { v0.x, v1.x};
			m_data[1] = { v0.y, v1.y};

			return *this;
		}

		Matrix2<T>& Transparancy_rnm()
		{
			Vector2<T> v0 = m_data[0];
			Vector2<T> v1 = m_data[1];

			return {
				{ v0.x, v1.x },
				{ v0.y, v1.y }
			};
		}

		Matrix2<T>& Mul(Matrix2<T> const& m)
		{

			Vector2<T>* new_data = new Vector2<T>[2];

			for (uint32_t i = 0; i < 2; ++i)
			{
				new_data[i] = { 0, 0, 0, 0 };
				for (uint32_t j = 0; j < 2; ++j)
				{
					for (uint32_t k = 0; k < 2; ++k)
					{
						new_data[i][j] += m_data[i][k] * m[k][j];
					}
				}
			}

			delete[] m_data;
			m_data = new_data;

			return *this;
		}

		Vector2<T> Mul(Vector2<T> const& p_v)
		{
			Vector2<T> m_m;

			for (uint32_t i = 0; i < 2; ++i)
			{
				for (uint32_t j = 0; j < 2; ++j)
				{
					m_m[i] += p_v[j] * m_data[j][i];
				}
			}

			return m_m;
		}

		T Det() const
		{
			return m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];
		}

		T* GetData()
		{
			return (T*)m_data;
		}

		std::string String() const
		{
			return m_data[0].str() + '\n' + m_data[1].str();
		}

		Vector2<T>& operator[](uint32_t i) { return m_data[i]; }

		Vector2<T>& operator[](uint32_t i) const { return m_data[i]; }

		Matrix2<T>& operator=(Matrix2<T> const& other)
		{
			for (uint32_t i = 0; i < 2; ++i)
				m_data[i] = other[i];
			return *this;
		}

	private:
		Vector2<T> * m_data;
	};

	template<typename T>
	class Matrix3 {
	public:
		Matrix3(
			Vector3<T> const& v0 = { 1, 0, 0 }, Vector3<T> const& v1 = { 0, 1, 0 },
			Vector3<T> const& v2 = { 0, 0, 1 }
		)
			:m_data(new Vector3<T>[3])
		{
			m_data[0] = v0;
			m_data[1] = v1;
			m_data[2] = v2;
		}

		Matrix3(Matrix3<T> const& m)
			:m_data(new Vector3<T>[3])
		{
			m_data[0] = m[0];
			m_data[1] = m[1];
			m_data[2] = m[2];
		}

		~Matrix3()
		{
			delete[] m_data;
		}

		Matrix3<T>& Mul(float const& scalar)
		{
			for (uint32_t i = 0; i < 3; ++i)
				m_data[i] *= scalar;
			return *this;
		}

		Matrix3<T>& Identity()
		{
			m_data[0] = { 1, 0, 0 };
			m_data[1] = { 0, 1, 0 };
			m_data[2] = { 0, 0, 1 };
			return *this;
		}

		Matrix3<T>& Transparancy()
		{
			Vector3<T> v0 = m_data[0];
			Vector3<T> v1 = m_data[1];
			Vector3<T> v2 = m_data[2];

			m_data[0] = { v0.x, v1.x, v2.x };
			m_data[1] = { v0.y, v1.y, v2.y };
			m_data[2] = { v0.z, v1.z, v2.z };

			return *this;
		}

		Matrix3<T> Transparancy_rnm()
		{
			Vector3<T> v0 = m_data[0];
			Vector3<T> v1 = m_data[1];
			Vector3<T> v2 = m_data[2];

			return {
				{ v0.x, v1.x, v2.x },
				{ v0.y, v1.y, v2.y },
				{ v0.z, v1.z, v2.z }
			};
		}

		Matrix3<T>& Mul(Matrix3<T> const& m)
		{

			Vector3<T>* new_data = new Vector3<T>[3];

			for (uint32_t i = 0; i < 3; ++i)
			{
				new_data[i] = { 0, 0, 0, 0 };
				for (uint32_t j = 0; j < 3; ++j)
				{
					for (uint32_t k = 0; k < 3; ++k)
					{
						new_data[i][j] += m_data[i][k] * m[k][j];
					}
				}
			}

			delete[] m_data;
			m_data = new_data;

			return *this;
		}

		Vector3<T> Mul(Vector3<T> const& p_v)
		{
			Vector3<T> m_m;

			for (uint32_t i = 0; i < 3; ++i)
			{
				for (uint32_t j = 0; j < 3; ++j)
				{
					m_m[i] += p_v[j] * m_data[j][i];
				}
			}

			return m_m;
		}

		T Det() const
		{
			return
				+m_data[0][0] * m_data[1][1] * m_data[2][2]
				- m_data[0][0] * m_data[1][2] * m_data[2][1]
				- m_data[0][1] * m_data[1][0] * m_data[2][2]
				+ m_data[0][1] * m_data[1][2] * m_data[2][0]
				+ m_data[0][2] * m_data[1][0] * m_data[2][1]
				- m_data[0][2] * m_data[1][1] * m_data[2][0];
		}

		T* GetData()
		{
			return (T*)m_data;
		}

		std::string String() const
		{
			return m_data[0].str() + '\n' + m_data[1].str() + '\n' + m_data[2].str();
		}

		Vector3<T>& operator[](uint32_t i) { return m_data[i]; }

		Vector3<T>& operator[](uint32_t i) const { return m_data[i]; }

		Matrix3<T>& operator=(Matrix3<T> const& other)
		{
			for (uint32_t i = 0; i < 3; ++i)
				m_data[i] = other[i];
			return *this;
		}

	private:
		Vector3<T> * m_data;
	};

	template<typename T>
	class Matrix4 {
	public:
		Matrix4(
			Vector4<T> const& v0 = { 1, 0, 0, 0 }, Vector4<T> const& v1 = { 0, 1, 0, 0 },
			Vector4<T> const& v2 = { 0, 0, 1, 0 }, Vector4<T> const& v3 = { 0, 0, 0, 1 }
		)
			:m_data(new Vector4<T>[4])
		{
			m_data[0] = v0;
			m_data[1] = v1;
			m_data[2] = v2;
			m_data[3] = v3;
		}

		Matrix4(Matrix4<T> const& m)
			:m_data(new Vector4<T>[4])
		{
			m_data[0] = m[0];
			m_data[1] = m[1];
			m_data[2] = m[2];
			m_data[3] = m[3];
		}

		~Matrix4()
		{
			delete[] m_data;
		}

		Matrix4<T>& Mul(float const& scalar)
		{
			for (uint32_t i = 0; i < 4; ++i)
				m_data[i] *= scalar;
			return *this;
		}

		Matrix4<T>& Identity()
		{
			m_data[0] = { 1, 0, 0, 0 };
			m_data[1] = { 0, 1, 0, 0 };
			m_data[2] = { 0, 0, 1, 0 };
			m_data[3] = { 0, 0, 0, 1 };
			return *this;
		}

		Matrix4<T>& Transparancy()
		{
			Vector4<T> v0 = m_data[0];
			Vector4<T> v1 = m_data[1];
			Vector4<T> v2 = m_data[2];
			Vector4<T> v3 = m_data[3];

			m_data[0] = { v0.x, v1.x, v2.x, v3.x };
			m_data[1] = { v0.y, v1.y, v2.y, v3.y };
			m_data[2] = { v0.z, v1.z, v2.z, v3.z };
			m_data[3] = { v0.w, v1.w, v2.w, v3.w };

			return *this;
		}

		Matrix4<T> Transparancy_rnm()
		{
			Vector4<T> v0 = m_data[0];
			Vector4<T> v1 = m_data[1];
			Vector4<T> v2 = m_data[2];
			Vector4<T> v3 = m_data[3];

			return {
				{ v0.x, v1.x, v2.x, v3.x },
				{ v0.y, v1.y, v2.y, v3.y },
				{ v0.z, v1.z, v2.z, v3.z },
				{ v0.w, v1.w, v2.w, v3.w }
			};
		}

		T Det() const
		{
			Vector4<T> const& v1 = m_data[1];
			Vector4<T> const& v2 = m_data[2];
			Vector4<T> const& v3 = m_data[3];

			T answer = 0;
			short a = -1;

			for (uint32_t i = 0; i < 4; ++i)
			{
				a *= -1;

				uint32_t pos[3] = { 1, 2, 3 };

				for (uint32_t j = 0; j < i; ++j)
					pos[j] --;

				Matrix3<T> m_m = {
					{ v1[pos[0]], v1[pos[1]], v1[pos[2]] },
					{ v2[pos[0]], v2[pos[1]], v2[pos[2]] },
					{ v3[pos[0]], v3[pos[1]], v3[pos[2]] }
				};
				answer += m_data[0][i] * m_m.Det() * a;
			}

			return answer;
		}


		// BEGIN block of test function for Inverse matrix
		Matrix4<T> Adjoint() const
		{
			Matrix4<T> m_m = *this;
			m_m.Transparancy();
			auto algadd = [&m_m](uint32_t p_i, uint32_t p_j)
			{
				Matrix3<T> matrix_for_addition;
				uint32_t i_offset = 0;
				for (uint32_t i = 0; i < 4; ++i)
				{
					if (i == p_i) {
						i_offset = 1;
						continue;
					}

					Vector4<T> const& v_src = m_m[i];
					Vector3<T>& dest = matrix_for_addition[i - i_offset];

					uint32_t j_offset = 0;
					for (uint32_t j = 0; j < 4; ++j)
					{
						if (j == p_j) {
							j_offset = 1;
							continue;
						}

						dest[j - j_offset] = v_src[j];
					}
				}
				return matrix_for_addition.Det();;
			};

			Matrix4<T> adjoint_m;
			short a = 1;
			for (uint32_t i = 0; i < 4; ++i)
			{
				a *= -1;
				for (uint32_t j = 0; j < 4; ++j)
				{
					a *= -1;
					adjoint_m[i][j] = algadd(i, j) * a;
				}
			}
			return adjoint_m;
		}

		Matrix4<T> Inverse() const
		{
			T m_Det = Det();
			return Adjoint().Mul(T(1. / m_Det));
		}

		// END block of test function for Inverse matrix

		Matrix4<T>& Scale(float const& Scale)
		{
			Matrix4<T> m_m = {
				{ Scale, 0, 0, 0 },
				{ 0, Scale, 0, 0 },
				{ 0, 0, Scale, 0 },
				{ 0, 0, 0, 1 }
			};
			return Mul(m_m);
		}

		Matrix4<T>& Scale(Vector4<T> scale)
		{
			Matrix4<T> m_m = {
				{ scale.x, 0, 0, 0 },
				{ 0, scale.y, 0, 0 },
				{ 0, 0, scale.z, 0 },
				{ 0, 0, 0, scale.w }
			};
			return Mul(m_m);
		}

		Matrix4<T>& Mul(Matrix4<T> const& m)
		{

			Vector4<T>* new_data = new Vector4<T>[4];

			for (uint32_t i = 0; i < 4; ++i)
			{
				new_data[i] = { 0, 0, 0, 0 };
				for (uint32_t j = 0; j < 4; ++j)
				{
					for (uint32_t k = 0; k < 4; ++k)
					{
						new_data[i][j] += m_data[i][k] * m[k][j];
					}
				}
			}

			delete[] m_data;
			m_data = new_data;

			return *this;
		}

		Vector4<T> Mul(Vector4<T> const& p_v)
		{
			Vector4<T> m_m;

			for (uint32_t i = 0; i < 4; ++i)
			{
				for (uint32_t j = 0; j < 4; ++j)
				{
					m_m[i] += p_v[j] * m_data[i][j];
				}
			}

			return m_m;
		}

		Matrix4<T>& Translate(Vector3<T> const& p_v)
		{
			Matrix4<T> m_m;
			m_m.m_data[0][3] = p_v.x;
			m_m.m_data[1][3] = p_v.y;
			m_m.m_data[2][3] = p_v.z;
			return Mul(m_m);
		}

		Matrix4<T>& Rotate(float const & angle, Vector3<T> const & axis)
		{
			using std::cos;
			using std::sin;

			float rad_angle = ToRadian(angle);
			Matrix4<T> m_m = {
				{
					cos(rad_angle) + (1 - cos(rad_angle)) * axis.x * axis.x ,
					(1 - cos(rad_angle)) * axis.x * axis.y - sin(rad_angle) * axis.z,
					(1 - cos(rad_angle)) * axis.x * axis.z + sin(rad_angle) * axis.y,
					0
				},
				{
					(1 - cos(rad_angle)) * axis.x * axis.y + sin(rad_angle) * axis.z,
					cos(rad_angle) + (1 - cos(rad_angle)) * axis.y * axis.y,
					(1 - cos(rad_angle)) * axis.z * axis.y - sin(rad_angle) * axis.x,
					0
				},
				{
					(1 - cos(rad_angle)) * axis.x * axis.z - sin(rad_angle) * axis.y,
					(1 - cos(rad_angle)) * axis.z * axis.y + sin(rad_angle) * axis.x,
					cos(rad_angle) + (1 - cos(rad_angle)) * axis.z * axis.z,
					0
				},
				{
					0, 0, 0, 1
				}
			};

			return Mul(m_m);
		}

		T* GetData() const
		{
			return (T*)m_data;
		}

		std::string String() const
		{
			return m_data[0].str() + '\n' + m_data[1].str() + '\n' + m_data[2].str() + '\n' + m_data[3].str();
		}

		Vector4<T>& operator[](uint32_t i) { return m_data[i]; }

		Vector4<T>& operator[](uint32_t i) const { return m_data[i]; }

		Matrix4<T>& operator=(Matrix4<T> const& other)
		{
			for (uint32_t i = 0; i < 4; ++i)
				m_data[i] = other[i];
			return *this;
		}

	private:
		Vector4<T> * m_data;
	};

	using Vector2i = Vector2<int>;
	using Vector2f = Vector2<float>;
	using Vector3f = Vector3<float>;
	using Vector4f = Vector4<float>;
	using Matrix3f = Matrix3<float>;
	using Matrix4f = Matrix4<float>;

	template<typename T>
	Vector2<T> operator+(Vector2<T> const& v0, Vector2<T> const& v1)
	{
		Vector2<T> result;
		for (uint32_t i = 0; i < 2; ++i)
			result[i] = v0[i] + v1[i];
		return result;
	}

	template<typename T>
	bool operator==(Vector2<T> const& v0, Vector2<T> const& v1)
	{
		return v0.x == v1.x && v0.y == v1.y;
	}

	template<typename T>
	bool operator!=(Vector2<T> const& v0, Vector2<T> const& v1)
	{
		return !(v0 == v1);
	}

	template<typename T>
	Vector3<T> operator+(Vector3<T> const& v0, Vector3<T> const& v1)
	{
		Vector3<T> result;
		for (uint32_t i = 0; i < 3; ++i)
			result[i] = v0[i] + v1[i];
		return result;
	}

	template<typename T>
	bool operator==(Vector3<T> const& v0, Vector3<T> const& v1)
	{
		return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
	}

	template<typename T>
	bool operator!=(Vector3<T> const& v0, Vector3<T> const& v1)
	{
		return !(v0 == v1);
	}

	template<typename T>
	Vector4<T> operator+(Vector4<T> const& v0, Vector4<T> const& v1)
	{
		Vector4<T> result;
		for (uint32_t i = 0; i < 4; ++i)
			result[i] = v0[i] + v1[i];
		return result;
	}

	template<typename T>
	bool operator==(Vector4<T> const& v0, Vector4<T> const& v1)
	{
		return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z && v0.w == v1.w;
	}

	template<typename T>
	bool operator!=(Vector4<T> const& v0, Vector4<T> const& v1)
	{
		return !(v0 == v1);
	}

	template<typename T>
	Vector2<T> operator-(Vector2<T> const& v0, Vector2<T> const& v1)
	{
		Vector2<T> result;
		for (uint32_t i = 0; i < 2; ++i)
			result[i] = v0[i] - v1[i];
		return result;
	}

	template<typename T>
	Vector3<T> operator-(Vector3<T> const& v0, Vector3<T> const& v1)
	{
		Vector3<T> result;
		for (uint32_t i = 0; i < 3; ++i)
			result[i] = v0[i] - v1[i];
		return result;
	}

	template<typename T>
	Vector4<T> operator-(Vector4<T> const& v0, Vector4<T> const& v1)
	{
		Vector4<T> result;
		for (uint32_t i = 0; i < 4; ++i)
			result[i] = v0[i] - v1[i];
		return result;
	}

	template<typename T>
	Vector2<T> operator*(Vector2<T> const& v0, T const& scalar)
	{
		return {
			v0.x * scalar,
			v0.y * scalar
		};
	}

	template<typename T>
	Vector3<T> operator*(Vector3<T> const& v0, T const& scalar)
	{
		return {
			v0.x * scalar,
			v0.y * scalar,
			v0.z * scalar
		};
	}

	template<typename T>
	Vector4<T> operator*(Vector4<T> const& v0, T const& scalar)
	{
		return {
			v0.x * scalar,
			v0.y * scalar,
			v0.z * scalar,
			v0.y * scalar
		};
	}

	template<typename T>
	Matrix4<T> operator*(Matrix4<T> const& v0, Matrix4<T> const& v1)
	{
		Matrix4<T> newMatrix;

		for (uint32_t i = 0; i < 4; ++i)
		{
			newMatrix[i] = { 0, 0, 0, 0 };
			for (uint32_t j = 0; j < 4; ++j)
			{
				for (uint32_t k = 0; k < 4; ++k)
				{
					newMatrix[i][j] += v0[i][k] * v1[k][j];
				}
			}
		}

		return newMatrix;
	}

	template<typename T>
	Vector2<T> operator/(Vector2<T> const& v0, T const& scalar)
	{
		return {
			v0.x / scalar,
			v0.y / scalar
		};
	}

	template<typename T>
	Vector3<T> operator/(Vector3<T> const& v0, T const& scalar)
	{
		return {
			v0.x / scalar,
			v0.y / scalar,
			v0.z / scalar
		};
	}

	template<typename T>
	Vector4<T> operator/(Vector4<T> const& v0, T const& scalar)
	{
		return {
			v0.x / scalar,
			v0.y / scalar,
			v0.z / scalar,
			v0.y / scalar
		};
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, Vector2<T> const& v)
	{
		os << v.str();
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, Vector3<T> const& v)
	{
		os << v.str();
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, Vector4<T> const& v)
	{
		os << v.str();
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, Matrix2<T> const& m)
	{
		os << "begin matrix .. \n";
		for (uint32_t i = 0; i < 2; ++i) {
			Vector2<T> const& v = m[i];
			os << v.str() << '\n';
		}
		os << "end matrix .. \n";
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, Matrix3<T> const& m)
	{
		os << "begin matrix .. \n";
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3<T> const& v = m[i];
			os << v.str() << '\n';
		}
		os << "end matrix .. \n";
		return os;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, Matrix4<T> const& m)
	{
		os << "begin matrix .. \n";
		for (uint32_t i = 0; i < 4; ++i) {
			Vector4<T> const& v = m[i];
			os << v.str() << '\n';
		}
		os << "end matrix .. \n";
		return os;
	}

	template<typename T>
	class AABB2D
	{
	public:
		T GetWidth() const { return Max.x - Min.x; }

		T GetHeight() const { return Max.y - Min.y; }

		void Move(Vector2<T> offset)
		{
			Min += offset;
			Max += offset;
		}

	public:
		Vector2<T> Min;
		Vector2<T> Max;
	};

	using AABB2Df = AABB2D<float>;
	using AABB2Di = AABB2D<int32_t>;

	/*bool RectangleAreasOverlay(RectangleAreai first, RectangleAreai second);
	bool RectangleAreasOverlay(RectangleAreaf first, RectangleAreaf second);
	bool RectangleAreasOverlay(RectangleAreai first, RectangleAreai second, RectangleAreai& dest);*/
	template<typename T>
	bool AABB2DInter(AABB2D<T> first, AABB2D<T> second)
	{
		if (first.Max.x < second.Min.x || first.Min.x > second.Max.x) return false;
		if (first.Max.y < second.Min.y || first.Min.y > second.Max.y) return false;
		return true;
	}

	template<typename T>
	bool AABB2DZeroSpace(Rainy::AABB2D<T> const& aabb)
	{
		return ((aabb.Max.x - aabb.Min.x) <= 0) || ((aabb.Max.y - aabb.Min.y) <= 0);
	};

	bool AABB2DOverlay(AABB2Df first, AABB2Df second, AABB2Df& dest);

	bool AABB2DOverlay(AABB2Di first, AABB2Di second, AABB2Di& dest);

	std::pair<AABB2Df, AABB2Df> AABB2DSplit(AABB2Df aabb, float splitVal,
		bool hSplit = true /* horizontal split - default */);

	std::pair<AABB2Di, AABB2Di> AABB2DSplit(AABB2Di aabb, int32_t splitVal,
		bool hSplit = true /* horizontal split - default */);

	std::vector<AABB2Di> AABB2DExcludeOverlay(AABB2Di aabb, AABB2Di overlay);

	bool InterRayAndSurface(Vector3f surfPoint, Vector3f surfNormal, Vector3f rayStartPoint,
		Vector3f rayDir, Vector3f& dest, float& rayScale);

	bool InterRayAndSphere(Vector3f sphereCenter, float radius, Vector3f rayStartPoint,
		Vector3f rayDir, Vector3f& dest, float& rayScale);

	Vector3f BarycentricCoord(Vector3f const& p, Vector3f const& p0, Vector3f const& p1, Vector3f const& p2);

	Matrix4f CreateModelMatrix(Vector3f const& position, Vector3f const& rotation, float const& scale);
	Matrix4f CreateModelMatrix(Vector3f const& position, Vector3f const& rotation, Vector3f const& scale);
	Matrix4f CreateViewMatrix(Vector3f const& pos, float const& x_rot, float const& y_rot);
	Matrix4f CreateLookAtMatrix(Vector3f pos, Vector3f at, Vector3f up);
	Matrix4f CreatePerspectiveMatrix(float const& ar, float const& fov, float const& nearPoint, float const& farPoint);
	Matrix4f CreatePerspectiveMatrix(float left, float right, float bottom, float top, float nearPoint, float farPoint);
	Matrix4f CreateOrthographicMatrix(float left, float right, float bottom, float top, float nearPoint, float farPoint);

	Vector3f ViewportSpaceToWorldSpace(Vector2f const& viewport_position,
		Matrix4f const& projection_matrix, Matrix4f const& view_matrix,
		uint16_t viewport_width, uint16_t viewport_height);

	Vector3f GetLookDirection(Vector3f rotation);

	float Centerf(float min, float max);

}
