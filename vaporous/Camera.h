#pragma once
#ifndef CAMERA
#define CAMERA

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using mat4 = glm::mat4;
using vec3 = glm::vec3;

class Camera {
public:
	virtual mat4 getViewMatrix() { return mat4(1); };
	virtual mat4 getProjectionMatrix() { return mat4(1); };
	virtual vec3 getPosition() { return vec3(0); };
};

#endif // ! CAMERA