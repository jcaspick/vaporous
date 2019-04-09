#include "FreeCamera.h"
#include "Utilities.h"

FreeCamera::FreeCamera() :
	_position(0, 0, 0),
	_forward(0, 0, 1),
	_right(1, 0, 0),
	_up(0, 1, 0),
	_worldUp(0, 1, 0),
	_fov(45.0f),
	_screenWidth(1280),
	_screenHeight(720)
{
	calculateVectors();
}

mat4 FreeCamera::getViewMatrix() {
	return glm::lookAt(_position, _position + _forward, _up);
}

mat4 FreeCamera::getProjectionMatrix() {
	return glm::perspective(glm::radians(_fov), 
		_screenWidth / _screenHeight, 0.1f, 1000.0f);
}

void FreeCamera::setScreenSize(vec2 screen) {
	_screenWidth = screen.x;
	_screenHeight = screen.y;
}

void FreeCamera::setPosition(vec3 pos) {
	_position = pos;
}

void FreeCamera::setRotation(quat rot) {
	_rotation = rot;
	calculateVectors();
}

void FreeCamera::setFOV(float fov) {
	_fov = fov;
}

void FreeCamera::calculateVectors() {
	_right = Util::transformVec3(vec3(1, 0, 0), glm::toMat4(_rotation));
	_up = Util::transformVec3(vec3(0, 1, 0), glm::toMat4(_rotation));
	_forward = Util::transformVec3(vec3(0, 0, -1), glm::toMat4(_rotation));
}