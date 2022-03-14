//////////////////////////////////////////////////////////////////////
/// Desc  Vector2f
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*static*/ Vector2f Vector2f::Zero(0, 0);
/*static*/ Vector2f Vector2f::One(1, 1);
/*static*/ Vector2f Vector2f::XAxis(1, 0);
/*static*/ Vector2f Vector2f::YAxis(0, 1);

Vector2f::Vector2f(const Vector3f &v)
: x(v.x), y(v.y)
{}

void Vector2f::normalize()
{
	float l = length();
	if(l != 0.0f)
	{
		*this *= 1.0f / l;
	}
}
