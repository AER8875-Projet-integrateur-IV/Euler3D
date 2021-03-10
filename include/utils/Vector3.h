//
// Created by amin on 2/21/21.
//
#pragma once

#include <cmath>
#include <iostream>


namespace E3D {

	template<class T>
	class Vector3;

	template<class T>
	std::ostream &operator<<(std::ostream &, const Vector3<T> &);

	template<class T>
	class Vector3 {
		// Class to deal with 2D vectors
		// Should be declared and defined in header file because its templated
	public:
		T x, y, z;

		Vector3() : x(0.0), y(0.0), z(0.0) {}
		Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

		//Copy constructor
		Vector3(const Vector3 &v) : x(v.x), y(v.y), z(v.z) {}

		// Copy assignment operator
		Vector3 &operator=(const Vector3 &v) {
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}


		Vector3 operator+(Vector3 &v) {
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		Vector3 operator+(Vector3 &&v) {
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		void operator+=(Vector3 &&v) {
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
		}

		void operator+=(Vector3 &v) {
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
		}

		void operator*=(int &&v) {
			this->x *= v;
			this->y *= v;
			this->z *= v;
		}

		bool operator==(const Vector3 &v) const {
			return (this->x == v.x && this->y == v.y) && this->z == v.z;
		}


		Vector3 operator-(Vector3 &v) {
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		static T dot(Vector3 &v1, Vector3 &v2) {
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		}

		static Vector3<T> crossProduct(Vector3 &A, Vector3 &B) {
			return Vector3(A.y * B.z - A.z * B.y,
			               A.z * B.x - A.x * B.z,
			               A.x * B.y - A.y * B.x);
		}


		Vector3
		operator+(T s) {
			return Vector3(x + s, y + s, z + s);
		}
		Vector3 operator-(T s) {
			return Vector3(x - s, y - s, z - s);
		}
		Vector3 operator*(T s) {
			return Vector3(x * s, y * s, z * s);
		}
		Vector3 operator/(T s) {
			return Vector3(x / s, y / s, z / s);
		}

		void set(T x, T y, T z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}


		double length() const {
			return std::sqrt(x * x + y * y + z * z);
		}


		friend std::ostream &operator<<<>(std::ostream &os, const Vector3<T> &v);
	};

	// Print overloading
	template<class T>
	std::ostream &operator<<(std::ostream &os, const E3D::Vector3<T> &v) {
		os << "{ " << v.x << " , " << v.y << " , " << v.z << " }";
		return os;
	}


}// Namespace E3D
