//////////////////////////////////////////////////////////////////////
/// Desc  Camera
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Transform.hpp"
#include "Vector2f.hpp"
#include <cstdint>

class Camera : public Transform
{
public:
	Camera();
	~Camera();

	void setMoveSpeed(float speed) { moveSpeed_ = speed; }
	float getMoveSpeed() const { return moveSpeed_; }

	void setPerspective(float fov, float aspect, float znear, float zfar);
	void setOrtho(float w, float h, float zn, float zf);

	const Matrix& getProjMatrix() const { return matProj_; }
	const Matrix& getViewMatrix() const;
	const Matrix& getViewProjMatrix() const;

	bool handleMouseButton(int button, int action, int mods);
	bool handleMouseMove(float x, float y);
	bool handleMouseScroll(float xoffset, float yoffset);
	bool handleCameraMove();
    
    float getZNear() const { return zNear_; }
    float getZFar() const { return zFar_; }
    float getFov() const { return fov_; }
    float getAspect() const { return aspect_; }
    const Vector2f& getOrthoSize() const { return orthoSize_; }
    
    Vector3f screenPosToWorld(const Vector3f &screenPos) const;
    Vector3f projectionPosToWorld(const Vector3f &projPos) const;

private:
	mutable Matrix	matView_;
	mutable Matrix  matViewProj_;
	Matrix	matProj_;

	float	moveSpeed_;
	Vector2f lastCursorPos_;
    
    float   zNear_;
    float   zFar_;
    Vector2f orthoSize_;
    
    float   fov_;
    float   aspect_;
};
