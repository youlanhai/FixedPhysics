//////////////////////////////////////////////////////////////////////
/// Desc  Vector3f
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class Vector2f;

class Vector3f
{
public:
	float x, y, z;

	Vector3f() : x(0), y(0), z(0) {}
	explicit Vector3f(float v) : x(v), y(v), z(v) {}
	Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3f(const Vector2f &v, float _z);

	Vector3f operator - () const { return Vector3f(-x, -y, -z);  }
	float& operator [] (size_t i){ return reinterpret_cast<float*>(this)[i]; }
	float  operator [] (size_t i) const { return reinterpret_cast<const float*>(this)[i]; }

	float lengthSq() const { return x * x + y * y + z *z; }
	float length() const { return sqrtf(lengthSq()); }

    float distanceToSq(const Vector3f &v) const { return x * v.x + y * v.y + z * v.z; }
    float distanceTo(const Vector3f &v) const { return sqrtf(distanceToSq(v)); }

	void setZero() { *this = Zero; }
	void set(float _x, float _y, float _z){ x = _x; y = _y; z = _z; }

	void normalize();

	float dotProduct(const Vector3f &right) const { return x * right.x + y * right.y + z * right.z; }

	Vector3f crossProduct(const Vector3f &right) const;
	void crossProduct(const Vector3f &left, const Vector3f &right);

	Vector3f operator + (const Vector3f &v) const { return Vector3f(x + v.x, y + v.y, z + v.z); }
	Vector3f operator - (const Vector3f &v) const { return Vector3f(x - v.x, y - v.y, z - v.z); }
	Vector3f operator * (const Vector3f &v) const { return Vector3f(x * v.x, y * v.y, z * v.z); }
	Vector3f operator / (const Vector3f &v) const { return Vector3f(x / v.x, y / v.y, z / v.z); }

	Vector3f operator * (float v) const { return Vector3f(x * v, y * v, z * v); }
	Vector3f operator / (float v) const { return *this * (1.0f / v); }

	const Vector3f& operator += (const Vector3f &v) { x += v.x; y += v.y; z += v.z; return *this; }
	const Vector3f& operator -= (const Vector3f &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	const Vector3f& operator *= (const Vector3f &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	const Vector3f& operator /= (const Vector3f &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	const Vector3f& operator *= (float v) { x *= v, y *= v; z *= v; return *this; }
	const Vector3f& operator /= (float v) { return *this *= (1.0f / v); }
	
public:
	static Vector3f Zero;
	static Vector3f One;
	static Vector3f XAxis;
	static Vector3f YAxis;
	static Vector3f ZAxis;
};

#endif // VECTOR3_H
