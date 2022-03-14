//////////////////////////////////////////////////////////////////////
/// Desc  Vector3f
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#include "Vector3f.hpp"
#include "Vector2f.hpp"

/*static*/ Vector3f Vector3f::Zero(0, 0, 0);
/*static*/ Vector3f Vector3f::One(1, 1, 1);
/*static*/ Vector3f Vector3f::XAxis(1, 0, 0);
/*static*/ Vector3f Vector3f::YAxis(0, 1, 0);
/*static*/ Vector3f Vector3f::ZAxis(0, 0, 1);

Vector3f::Vector3f(const Vector2f &v, float _z)
: x(v.x), y(v.y), z(_z)
{}

void Vector3f::normalize()
{
	double l = std::sqrt(double(x) * x + y * y + z * z);
	if(l != 0.0f)
	{
		float d = float(1.0 / l);

		x *= d;
		y *= d;
		z *= d;
	}
}

Vector3f Vector3f::crossProduct(const Vector3f &right) const
{
	Vector3f ret;
	ret.crossProduct(*this, right);
	return ret;
}

void Vector3f::crossProduct(const Vector3f &left, const Vector3f &right)
{
	x = left.y * right.z - left.z * right.y;
	y = left.z * right.x - left.x * right.z;
	z = left.x * right.y - left.y * right.x;
}
