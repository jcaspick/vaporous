#pragma once
#ifndef FREE_CAMERA
#define FREE_CAMERA

#include "Camera.h"
#include <glm/gtx/quaternion.hpp>

using vec3 = glm::vec3;
using vec2 = glm::vec2;
using quat = glm::quat;

class FreeCamera : public Camera {
public:
	FreeCamera();

	virtual mat4 getViewMatrix() override;
	virtual mat4 getProjectionMatrix() override;

	void setScreenSize(vec2 screen);
	void setPosition(vec3 pos);
	void setRotation(quat rot);
	void setFOV(float fov);

private:
	void calculateVectors();

	vec3 _position;
	quat _rotation;
	vec3 _right;
	vec3 _up;
	vec3 _forward;
	vec3 _worldUp;
	float _fov;

	float _screenWidth;
	float _screenHeight;
};

#endif // !FREE_CAMERA