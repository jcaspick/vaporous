#pragma once
#ifndef FREE_CAMERA
#define FREE_CAMERA

#include "Camera.h"

using vec3 = glm::vec3;
using vec2 = glm::vec2;

class FreeCamera : public Camera {
public:
	FreeCamera(float& screenWidth, float& screenHeight);
	void update(float dt);
	virtual mat4 getViewMatrix() override;
	virtual mat4 getProjectionMatrix() override;

private:
	void calculateVectors();

	vec3 _position;
	vec3 _forward;
	vec3 _right;
	vec3 _up;
	vec3 _worldUp;
	float _yaw, _pitch;
	float _speed, _sensitivity;
	float _fov;

	float& _screenWidth;
	float& _screenHeight;
};

#endif // !FREE_CAMERA