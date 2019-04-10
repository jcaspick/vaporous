#pragma once
#include "Camera.h"

using vec3 = glm::vec3;
using vec2 = glm::vec2;

class TargetCamera : public Camera {
public:
	TargetCamera();
	virtual mat4 getViewMatrix() override;
	virtual mat4 getProjectionMatrix() override;

	void setScreenSize(vec2 screen);
	void setPosition(vec3 pos);
	void setTarget(vec3 post);
	void setFOV(float fov);

private:
	float _screenWidth;
	float _screenHeight;
	vec3 _position;
	vec3 _target;
	float _fov;
};