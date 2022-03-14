//////////////////////////////////////////////////////////////////////
/// Desc  Vector2f
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

class Vector3f;

class Vector2f
{
public:
	float x, y;

	Vector2f() : x(0), y(0) {}
	explicit Vector2f(float v) : x(v), y(v) {}
	Vector2f(float _x, float _y) : x(_x), y(_y) {}
	explicit Vector2f(const Vector3f &v);

	float& operator [] (size_t i){ return reinterpret_cast<float*>(this)[i]; }
	float  operator [] (size_t i) const { return reinterpret_cast<const float*>(this)[i]; }

	float lengthSq() const { return x * x + y * y; }
	float length() const { return sqrtf(lengthSq()); }

	void zero() { *this = Zero; }
	void set(float _x, float _y){ x = _x; y = _y; }

	void normalize();

	Vector2f operator + (const Vector2f &v) const { return Vector2f(x + v.x, y + v.y); }
	Vector2f operator - (const Vector2f &v) const { return Vector2f(x - v.x, y - v.y); }
	Vector2f operator * (const Vector2f &v) const { return Vector2f(x * v.x, y * v.y); }
	Vector2f operator / (const Vector2f &v) const { return Vector2f(x / v.x, y / v.y); }

	Vector2f operator * (float v) const { return Vector2f(x * v, y * v); }
	Vector2f operator / (float v) const { return *this *  (1.0f / v); }

	const Vector2f& operator += (const Vector2f &v) { x += v.x; y += v.y; return *this; }
	const Vector2f& operator -= (const Vector2f &v) { x -= v.x; y -= v.y; return *this; }
	const Vector2f& operator *= (const Vector2f &v) { x *= v.x; y *= v.y; return *this; }
	const Vector2f& operator /= (const Vector2f &v) { x /= v.x; y /= v.y; return *this; }

	const Vector2f& operator *= (float v) { x *= v, y *= v; return *this; }
	const Vector2f& operator /= (float v) { return *this *= (1.0f / v); }

public:
	static Vector2f Zero;
	static Vector2f One;
	static Vector2f XAxis;
	static Vector2f YAxis;
};

#endif // VECTOR2_H
