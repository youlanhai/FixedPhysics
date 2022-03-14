//////////////////////////////////////////////////////////////////////
/// Desc  Transform
/// Time  2020/12/10
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "Transform.hpp"

Transform::Transform()
    : dirty_(false)
    , scale_(Vector3f::One)
    , matRotation_(Matrix::Identity)
{

}

Transform::~Transform()
{

}

void Transform::setPosition(float x, float y, float z)
{
    position_.set(x, y, z);
    dirty_ = true;
}

void Transform::setPosition(const Vector3f & position)
{
    position_ = position;
    dirty_ = true;
}

void Transform::translate(const Vector3f & delta)
{
    setPosition(position_ + delta);
}

void Transform::setRotation(float pitch, float yaw, float roll)
{
    rotation_.set(pitch, yaw, roll);
    matRotation_.setRotate(pitch, yaw, roll);
    dirty_ = true;
}

void Transform::setRotation(const Vector3f &rotation)
{
    rotation_ = rotation;
    matRotation_.setRotate(rotation.x, rotation.y, rotation.z);
    dirty_ = true;
}

void Transform::setScale(float x, float y, float z)
{
    scale_.set(x, y, z);
    dirty_ = true;
}

void Transform::setScale(const Vector3f &scale)
{
    scale_ = scale;
    dirty_ = true;
}

const Matrix& Transform::getModelMatrix() const
{
    if (dirty_)
    {
        dirty_ = false;

        Matrix matScale;
        matScale.setScale(scale_);

        matModel_.multiply(matRotation_, matScale);
        matModel_[3] = position_;
    }
    return matModel_;
}

void Transform::lookAt(const Vector3f & position, const Vector3f & target, const Vector3f & up)
{
    dirty_ = true;
    position_ = position;

    Vector3f forward = target - position;
    forward.normalize();

    Vector3f right = up.crossProduct(forward);
    right.normalize();

    Vector3f newUp = forward.crossProduct(right);
    newUp.normalize();

    matRotation_.setIdentity();
    matRotation_[0] = right;
    matRotation_[1] = newUp;
    matRotation_[2] = forward;

    rotation_ = matRotation_.getRotate();
}
