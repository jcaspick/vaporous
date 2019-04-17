#include "TargetCamera.h"

TargetCamera::TargetCamera() :
	_screenWidth(1280),
	_screenHeight(720),
	_position(vec3(0)),
	_target(vec3(0, 0, 1)),
	_fov(45.0f)
{}

mat4 TargetCamera::getViewMatrix() {
	vec3 up(0, 1, 0);
	if (_position.x == _target.x && _position.z == _target.z)
		up = vec3(0, 0, 1);

	return glm::lookAt(_position, _target, up);
}

mat4 TargetCamera::getProjectionMatrix() {
	return glm::perspective(glm::radians(_fov),
		_screenWidth / _screenHeight, 0.1f, 1000.0f);
}

void TargetCamera::setScreenSize(vec2 screen) {
	_screenWidth = screen.x;
	_screenHeight = screen.y;
}

void TargetCamera::setPosition(vec3 pos) {
	_position = pos;
}

void TargetCamera::setTarget(vec3 pos) {
	_target = pos;
}

void TargetCamera::setFOV(float fov) {
	_fov = fov;
}

vec3 TargetCamera::getPosition() {
	return _position;
}