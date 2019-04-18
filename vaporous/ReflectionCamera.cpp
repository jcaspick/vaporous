#include "ReflectionCamera.h"
#include "Utilities.h"

ReflectionCamera::ReflectionCamera() :
	_position(0, 0, 0),
	_fov(45.0f),
	_screenWidth(1280),
	_screenHeight(720)
{}

mat4 ReflectionCamera::getViewMatrix() {
	switch (_faceIndex) {
		// positive X
	case 0:
		return glm::lookAt(_position, _position + vec3(1, 0, 0), vec3(0, -1, 0));
		break;
		// negative X
	case 1:
		return glm::lookAt(_position, _position + vec3(-1, 0, 0), vec3(0, -1, 0));
		break;
		// positive Y
	case 2:
		return glm::lookAt(_position, _position + vec3(0, 1, 0), vec3(0, 0, 1));
		break;
		// negative Y
	case 3:
		return glm::lookAt(_position, _position + vec3(0, -1, 0), vec3(0, 0, -1));
		break;
		// positive Z
	case 4:
		return glm::lookAt(_position, _position + vec3(0, 0, 1), vec3(0, -1, 0));
		break;
		// negative Z
	case 5:
		return glm::lookAt(_position, _position + vec3(0, 0, -1), vec3(0, -1, 0));
		break;
	}
	return mat4(1);
}

mat4 ReflectionCamera::getProjectionMatrix() {
	return glm::perspective(glm::radians(_fov),
		_screenWidth / _screenHeight, 0.05f, 1000.0f);
}

void ReflectionCamera::setScreenSize(vec2 screen) {
	_screenWidth = screen.x;
	_screenHeight = screen.y;
}

void ReflectionCamera::setPosition(vec3 pos) {
	_position = pos;
}

void ReflectionCamera::setFOV(float fov) {
	_fov = fov;
}

void ReflectionCamera::setFaceIndex(int i) {
	_faceIndex = i;
}