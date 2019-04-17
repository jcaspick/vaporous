#pragma once
#ifndef FREE_CAMERA
#define FREE_CAMERA

#include "Camera.h"
#include <glm/gtx/quaternion.hpp>

using vec3 = glm::vec3;
using vec2 = glm::vec2;
using quat = glm::quat;

class ReflectionCamera : public Camera {
public:
	ReflectionCamera();

	virtual mat4 getViewMatrix() override;
	virtual mat4 getProjectionMatrix() override;

	void setScreenSize(vec2 screen);
	void setPosition(vec3 pos);
	void setFOV(float fov);
	void setFaceIndex(int i);

private:
	int _faceIndex;
	vec3 _position;
	float _fov;

	float _screenWidth;
	float _screenHeight;
};

#endif // !FREE_CAMERA