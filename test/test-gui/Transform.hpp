//////////////////////////////////////////////////////////////////////
/// Desc  Transform
/// Time  2020/12/10
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Vector3f.hpp"
#include "Matrix.hpp"

class Transform
{
public:
    Transform();
    ~Transform();

    void setPosition(float x, float y, float z);
    void setPosition(const Vector3f &position);
    const Vector3f& getPosition() const { return position_; }

    void translate(const Vector3f &delta);

    void setRotation(float pitch, float yaw, float roll);
    void setRotation(const Vector3f &rotation);
    const Vector3f& getRotation() const { return rotation_;  }
    const Matrix& getRotationMatrix() const { return matRotation_; }

    void setScale(float x, float y, float z);
    void setScale(const Vector3f &scale);
    void setScale(float scale) { setScale(scale, scale, scale); }
    const Vector3f& getScale() const { return scale_; }

    const Vector3f& getRightVector() const { return matRotation_[0]; }
    const Vector3f& getUpVector() const { return matRotation_[1]; }
    const Vector3f& getForwardVector() const { return matRotation_[2]; }

    const Matrix& getModelMatrix() const;

    void lookAt(const Vector3f & position, const Vector3f & target, const Vector3f & up);

protected:
    mutable bool    dirty_;
    mutable Matrix  matModel_;

    Vector3f         position_;
    Vector3f         rotation_;
    Vector3f         scale_;
    Matrix          matRotation_;
};
