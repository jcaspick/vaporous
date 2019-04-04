#include "FreeCamera.h"

FreeCamera::FreeCamera(float& screenWidth, float& screenHeight) :
	_position(0, 0, 0),
	_forward(0, 0, 1),
	_up(0, 1, 0),
	_worldUp(0, 1, 0),
	_yaw(0),
	_pitch(0),
	_speed(2.5f),
	_sensitivity(0.1f),
	_fov(45.0f),
	_screenWidth(screenWidth),
	_screenHeight(screenHeight)
{
	calculateVectors();
}

mat4 FreeCamera::getViewMatrix() {
	return glm::lookAt(_position, _position + _forward, _up);
}

mat4 FreeCamera::getProjectionMatrix() {
	return glm::perspective(_fov, _screenWidth / _screenHeight, 0.1f, 1000.0f);
}

void FreeCamera::calculateVectors() {
	glm::vec3 forward;
	forward.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	forward.y = sin(glm::radians(_pitch));
	forward.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_forward = glm::normalize(forward);

	_right = glm::normalize(glm::cross(_forward, _worldUp));
	_up = glm::normalize(glm::cross(_right, _forward));
}